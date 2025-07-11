#include "module.h"
#include "jt808headerparser.h"

#include "easylogging++.h"

#include "tools.h"

Module::Module(TerminalInfo tInfo, platform::PlatformInfo pInfo, EventServerInfo esInfo) :
    terminalInfo(tInfo),
    platformInfo(pInfo),
    eventServerInfo(esInfo)
{
    std::cout << "RTSP = " << platformInfo.videoServer.rtspLink << std::endl;
    if(platformInfo.videoServer.connType == platform::ConnectionType::TCP) {
        std::cout << "Transport = TCP" << std::endl;
    } else if(platformInfo.videoServer.connType == platform::ConnectionType::UDP) {
        std::cout << "Transport = UDP" << std::endl;
    }
//    initPlatformServer();
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

//    std::cout << "Получил сообщение от сервера: " << std::endl;
//    std::cout << message << std::endl;

    //Отправка на платформу
    platformConnector.sendAlarmMessage(vec, serializer.getBodyStream());
}

void Module::initPlatformClient()
{
    platformConnector.setConfiguration(terminalInfo, platformInfo);
    platformConnector.connectToPlatform();
}

void Module::initPlatformServer()
{
    platformServer = std::make_unique<JT808Server>(terminalInfo.localServerInfo.host, terminalInfo.localServerInfo.port, terminalInfo.localServerInfo.connectionsCount);
    platformServer->start();
    platformServer->setPlatformRequestHandler([this](const std::vector<uint8_t> &answer){
        handlePlatformAnswer(std::move(answer));
    });
    std::thread platformServerThread([this](){
        platformServer->run();
    });
    platformServerThread.detach();
}

void Module::handlePlatformAnswer(const std::vector<uint8_t> &answer)
{
    JT808Header header = JT808HeaderParser::getHeader(answer);

    std::cout << "На сервер прилетел запрос от платформы: " << std::hex << header.messageID << std::endl;

}
