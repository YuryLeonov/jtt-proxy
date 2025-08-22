#ifndef TERMINALINFO_H
#define TERMINALINFO_H

#include <string>
#include <inttypes.h>

struct LocalServerInfo
{
    std::string host;
    uint32_t port;
    int connectionsCount = 3;
};

struct TerminalInfo
{
    std::string phoneNumber;
    uint16_t provinceID;
    uint16_t cityID;
    std::string manufacturerID;
    std::string terminalModel;
    std::string terminalID;
    uint8_t licencePlateColor;
    std::string vin;
};

#endif // TERMINALINFO_H
