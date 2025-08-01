#include "module.h"
#include "jt808headerparser.h"

#include "easylogging++.h"
#include "tools.h"

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
    wsClient = std::make_shared<WebSocketClient>(eventServerInfo.ipAddress, eventServerInfo.port, eventServerInfo.eventsTableName);
    wsClient->setReconnectTimeout(eventServerInfo.reconnectTimeout);
    wsClient->setSurveyInterval(eventServerInfo.surveyInterval);
    wsClient->setExternalMessageAlarmHandler(std::bind(&Module::wsClientMessageAlarmHandler, this, ::_1, ::_2));
    wsClient->setExternalMessageMediaInfoHandler(std::bind(&Module::wsClientMessageMediaInfoHandler, this, ::_1, ::_2));
    wsClient->connect();
}

void Module::wsClientMessageAlarmHandler(const std::string &eventID, const std::string &message)
{
    static uint8_t alarmSerialNum = 0;
    JT808EventSerializer serializer;
    serializer.setTerminalPhoneNumber(terminalInfo.phoneNumber);
    serializer.setTerminalID(terminalInfo.terminalID);
    std::vector<uint8_t> vec = std::move(serializer.serializeToBitStream(message, alarmSerialNum++));

    if(vec.empty()) {
        std::cout << "Задетектированных событий нет" << std::endl;
        return;
    }

    currentAlarmBody = serializer.getBodyStream();

    //Отправка на платформу
    platformConnector.sendAlarmMessage(eventID,  vec, serializer.getAddInfoStream());
}

void Module::wsClientMessageMediaInfoHandler(const std::string &eventID, const std::string &message)
{
    json data = json::parse(message);

    std::string pathToVideo = data.at("path2video");
    std::cout << "Найден видеоролик события: " << pathToVideo << std::endl;

//    if(!std::filesystem::exists(pathToVideo)) {
//        LOG(ERROR) << "Не найден видео файл " << pathToVideo << std::endl;
//        return;
//    }

    platformConnector.sendAlarmVideoFile(eventID, pathToVideo);

}

void Module::initPlatformClient()
{
    platformConnector.setConfiguration(terminalInfo, platformInfo);
    platformConnector.connectToPlatform();
}

void Module::handlePlatformAnswer(const std::vector<uint8_t> &answer)
{
    JT808Header header = JT808HeaderParser::getHeader(answer);

    std::cout << "На сервер прилетел запрос от платформы: " << std::hex << header.messageID << std::endl;

}
