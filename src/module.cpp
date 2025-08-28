#include "module.h"
#include "jt808headerparser.h"

#include "easylogging++.h"
#include "tools.h"
#include "alarmtypes.h"

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
    wsClient->setExternalMessageAlarmHandler(std::bind(&Module::wsClientMessageAlarmHandler, this, ::_1, ::_2));
    wsClient->setExternalMessageMediaInfoHandler(std::bind(&Module::wsClientMessageMediaInfoHandler, this, ::_1, ::_2));
    wsClient->setExternalMessageEventRemoved(std::bind(&Module::wsClientMessageEventRemovedHandler, this, ::_1));
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
    serializer.setLocationInfoStatus(JT808EventSerializer::Alarm);
    std::vector<uint8_t> vec = std::move(serializer.serializeToBitStream(message, alarmSerialNum++));

    if(vec.empty()) {
        LOG(ERROR) << "Ошибка формирования сообщения о событии.";
        return;
    }

    if(alarmSerialNum > 255) {
        alarmSerialNum = 0;
    }

    //Отправка на платформу
    platformConnector.sendAlarmMessage(type,  vec, serializer.getAddInfoStream());
}

void Module::wsClientMessageMediaInfoHandler(const std::string &eventID, const std::string &message)
{
    json data = json::parse(message);

    std::string pathToVideo = data.at("path2video");

//    pathToVideo = "/home/rossi-cpp-dev/projects/lms/video/1-2-3-4-5-6-7-8-9-10-11-12-13-14.mp4";

    if(!std::filesystem::exists(pathToVideo)) {
        LOG(ERROR) << "Не найден файл " << pathToVideo << " на диске" << std::endl;
        return;
    }

    std::thread uploadThread(&JT808Client::sendAlarmVideoFile, &platformConnector, eventID, pathToVideo);
    uploadThread.detach();

}

void Module::wsClientMessageEventRemovedHandler(const std::string &eventID)
{
    platformConnector.removeEvent(eventID);
}

void Module::initPlatformClient()
{
    platformConnector.setConfiguration(terminalInfo, platformInfo);
    platformConnector.connectToPlatform();
}
