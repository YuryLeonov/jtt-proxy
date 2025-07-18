#ifndef PLATFORMINFO_H
#define PLATFORMINFO_H

#include <string>
#include <vector>

namespace platform
{
    enum class ConnectionType
    {
        TCP = 0,
        UDP
    };

    struct VideoServer
    {
        std::vector<std::string> rtspLinks;
        ConnectionType connType;
    };

    struct PlatformInfo
    {
        std::string ipAddress = "";
        int port = 0;
        int heartBeatTimeout = 1000;
        int reconnectTimeout = 2000;
        VideoServer videoServer;
    };
}


#endif // PLATFORMINFO_H
