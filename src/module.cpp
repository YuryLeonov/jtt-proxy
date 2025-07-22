#include "module.h"
#include "jt808headerparser.h"

#include "easylogging++.h"

#include "tools.h"

Module::Module(TerminalInfo tInfo, platform::PlatformInfo pInfo, EventServerInfo esInfo) :
    terminalInfo(tInfo),
    platformInfo(pInfo),
    eventServerInfo(esInfo)
{
    std::string protocolTransport = "";
    if(platformInfo.videoServer.connType == platform::ConnectionType::TCP) {
        protocolTransport = "TCP";
    } else if(platformInfo.videoServer.connType == platform::ConnectionType::UDP) {
        protocolTransport = "UDP";
    }
    std::cout << "Transport for video: " << protocolTransport << std::endl;

//    initWebSocketClient();
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
    wsClient->setExternalMessageRecievedHandler(std::bind(&Module::wsClientMessageHandler, this, ::_1));
    wsClient->connect();
}

void Module::wsClientMessageHandler(const std::string &message)
{
    JT808EventSerializer serializer;
    serializer.setTerminalPhoneNumber(terminalInfo.phoneNumber);
    std::vector<uint8_t> vec = std::move(serializer.serializeToBitStream(message));

    if(vec.empty()) {
        std::cout << "Задетектированных событий нет" << std::endl;
        return;
    }

    //Отправка на платформу
    platformConnector.sendAlarmMessage(vec, serializer.getBodyStream());
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
