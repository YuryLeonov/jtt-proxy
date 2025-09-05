#include "module.h"
#include "jt808headerparser.h"

#include "easylogging++.h"
#include "tools.h"
#include "alarmtypes.h"
#include "platformalarmid.h"

#include <filesystem>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

Module::Module(TerminalInfo tInfo, platform::PlatformInfo pInfo, EventServerInfo esInfo) :
    terminalInfo(tInfo),
    platformInfo(pInfo),
    eventServerInfo(esInfo)
{
    initWebSocketClient();
    initPlatformClient();
}

Module::~Module()
{

}

void Module::setTerminalInfo(const TerminalInfo &info)
{
    terminalInfo = info;
    platformConnector.setTerminalInfo(terminalInfo);
}

void Module::setPlatformInfo(const platform::PlatformInfo &info)
{
    platformInfo = info;
    platformConnector.setPlatformInfo(platformInfo);
}

void Module::setEventServerInfo(const EventServerInfo &info)
 {
    eventServerInfo = info;
}

void Module::initWebSocketClient()
{
    wsClient = std::make_shared<WebSocketClient>(eventServerInfo.ipAddress, eventServerInfo.port);
    wsClient->setReconnectTimeout(eventServerInfo.reconnectTimeout);
    wsClient->setSurveyInterval(eventServerInfo.surveyInterval);
    wsClient->setAlarmVideosCount(terminalInfo.status.alarmVideosCount);
    wsClient->setAlarmVideosWaitInterval(terminalInfo.status.alarmVideosWaitInterval);
    wsClient->setExternalMessageAlarmHandler(std::bind(&Module::wsClientMessageAlarmHandler, this, ::_1, ::_2));
    wsClient->setExternalMessageMediaInfoHandler(std::bind(&Module::wsClientMessageMediaInfoHandler, this, ::_1, ::_2));
    wsClient->connect();
}

void Module::wsClientMessageAlarmHandler(const alarms::AlarmType &type, const std::string &message)
{
    if(!platformConnector.isPlatformConnected()) {
        LOG(ERROR) << "Ошибка отправки события на платформу: не соединения" << std::endl;
        return;
    }

    static uint8_t alarmSerialNum = 0;
    JT808EventSerializer serializer;
    serializer.setTerminalInfo(terminalInfo);
    serializer.setLocationInfoStatus(JT808EventSerializer::Alarm);
    std::vector<uint8_t> vec = std::move(serializer.serializeToBitStream(message, alarmSerialNum++));

    if(vec.empty()) {
        LOG(ERROR) << "Ошибка формирования сообщения о событии.";
        return;
    }

    if(alarmSerialNum > 255) {
        alarmSerialNum = 0;
    }

    SendedToPlatformAlarm alarm;
    alarm.databaseID = type.id;
    alarm.alarmID = serializer.getAlarmID();
    alarm.alarmType = serializer.getAlarmTypeID();
    alarm.time = serializer.getAlarmTime();
    alarm.alarmJT808Type = serializer.getAlarmType();
    auto now = std::chrono::system_clock::now();
    alarm.updateTime = std::chrono::system_clock::to_time_t(now);

    //Отправка на платформу
    platformConnector.sendAlarmMessage(vec, serializer.getAddInfoStream(), alarm);
}

void Module::wsClientMessageMediaInfoHandler(const std::string &eventID, const std::string &message)
{
    json data = json::parse(message);

    std::string pathToVideo = data.at("path2video");

    if(testCounter % 2 == 0) {
        pathToVideo = "/home/rossi-cpp-dev/projects/lms/mtp-808-proxy/tests/test1.mp4";
    } else {
        pathToVideo = "/home/rossi-cpp-dev/projects/lms/mtp-808-proxy/tests/test1.jpg";
    }
    testCounter++;

    platformConnector.addVideoFile(eventID, pathToVideo);
}

void Module::initPlatformClient()
{
    platformConnector.setConfiguration(terminalInfo, platformInfo);
    platformConnector.connectToPlatform();
    platformConnector.setAlarmRegisterHandler(std::bind(&Module::jt808clientAlarmSaveHandler, this, ::_1, ::_2, ::_3, ::_4));
    platformConnector.setAlarmConfirmHandler(std::bind(&Module::jt808clientAlarmConfirmHandler, this, ::_1, ::_2));
}

void Module::jt808clientAlarmSaveHandler(const std::string &eventUUID, const std::string &eventID, const std::string &timestamp, const std::string &status)
{
    if(wsClient) {
        wsClient->sendRequestForAlarmSave(eventUUID, eventID, timestamp, status);
    }
}

void Module::jt808clientAlarmConfirmHandler(const std::string &eventUUID, const std::string &status)
{
    if(wsClient) {
        wsClient->sendRequestForAlarmConfirm(eventUUID, status);
    }
}
