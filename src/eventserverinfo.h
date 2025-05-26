#ifndef EVENTSERVERINFO_H
#define EVENTSERVERINFO_H

struct EventServerInfo
{
    std::string ipAddress = "";
    int port = 0;
    std::string eventsTableName = "";
    int reconnectTimeout = 5000;
    int surveyInterval = 5000;
    std::string videoRootPath = "";
};

#endif // EVENTSERVERINFO_H
