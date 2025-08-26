#ifndef EVENTSERVERINFO_H
#define EVENTSERVERINFO_H

struct EventServerInfo
{
    std::string ipAddress = "";
    int port = 0;
    int reconnectTimeout = 5000;
    int surveyInterval = 5000;
};

#endif // EVENTSERVERINFO_H
