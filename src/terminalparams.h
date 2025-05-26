#ifndef TERMINALPARAMS_H
#define TERMINALPARAMS_H

#include <inttypes.h>

struct TerminalParameters
{
    uint32_t terminalHeartbeatTimeout = 5; //In seconds
    std::string primaryServerIpAddress = "";
    uint32_t serverTCPPort = 0;
    uint32_t serverUDPPort = 0;
};

#endif // TERMINALPARAMS_H
