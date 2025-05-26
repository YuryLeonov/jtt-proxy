#ifndef VIDEOSERVERINFO_H
#define VIDEOSERVERINFO_H

class string;
class uint16_t;
class uint8_t;

struct VideoServerInfo
{
    string ipAddress;
    uint16_t tcpPort;
    uint16_t udpPort;
    uint8_t channelNumber;
    uint8_t dataType;
    uint8_t streamType;
}

#endif // VIDEOSERVERINFO_H
