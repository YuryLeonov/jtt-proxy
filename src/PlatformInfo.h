#ifndef PLATFORMINFO_H
#define PLATFORMINFO_H

#include <string>

struct PlatformInfo
{
    std::string ipAddress = "";
    int port = 0;
    int heartBeatTimeout = 1000;
    int reconnectTimeout = 2000;
};

#endif // PLATFORMINFO_H
