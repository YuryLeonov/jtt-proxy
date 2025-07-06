#ifndef MODULE_H
#define MODULE_H

#include <TerminalInfo.h>
#include "PlatformInfo.h"
#include "eventserverinfo.h"
#include "websocketclient.h"
#include "jt808client.h"
#include "jt808server.h"
#include "jt808serializer.h"

class Module
{
public:
    Module(TerminalInfo tInfo, platform::PlatformInfo pInfo, EventServerInfo esInfo);
    ~Module();

    void setTerminalInfo(const TerminalInfo &info);
    void setPlatformInfo(const platform::PlatformInfo &info);
    void setEventServerInfo(const EventServerInfo &info);

private:
    void initWebSocketClient();
    void wsClientMessageHandler(const std::string &message);
    void initPlatformClient();
    void initPlatformServer();

    void handlePlatformAnswer(const std::vector<uint8_t> &answer);

private:
    std::shared_ptr<WebSocketClient> wsClient;
    JT808Client platformConnector;
    std::unique_ptr<JT808Server> platformServer;

    TerminalInfo terminalInfo;
    platform::PlatformInfo platformInfo;
    EventServerInfo eventServerInfo;

    std::vector<uint8_t> currentAlarmBody;

    bool test = false;

};

#endif // MODULE_H
