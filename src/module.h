#ifndef MODULE_H
#define MODULE_H

#include <TerminalInfo.h>
#include "PlatformInfo.h"
#include "eventserverinfo.h"
#include "websocketclient.h"
#include "jt808client.h"
#include "jt808server.h"
#include "jt808serializer.h"
#include "alarmtypes.h"

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
    void wsClientMessageAlarmHandler(const alarms::AlarmType &type, const std::string &message);
    void wsClientMessageMediaInfoHandler(const std::string &eventID, const std::string &message);

    void initPlatformClient();
    void jt808clientAlarmSaveHandler(const std::string &eventUUID, const std::string &eventID, const std::string &timestamp, const std::string &status);
    void jt808clientAlarmConfirmHandler(const std::string &eventUUID, const std::string &status);

private:
    std::shared_ptr<WebSocketClient> wsClient;
    JT808Client platformConnector;
    std::unique_ptr<JT808Server> platformServer;

    TerminalInfo terminalInfo;
    platform::PlatformInfo platformInfo;
    EventServerInfo eventServerInfo;

    bool test = false;

    int testCounter = 0;
};

#endif // MODULE_H
