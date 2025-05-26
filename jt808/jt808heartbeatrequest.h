#ifndef JT808HEARTBEATREQUEST_H
#define JT808HEARTBEATREQUEST_H

#include "jt808messageformatter.h"

class JT808HeartbeatRequest : public JT808MessageFormatter
{
public:
    JT808HeartbeatRequest(const TerminalInfo &info);
    ~JT808HeartbeatRequest();

    std::vector<uint8_t> getRequest() override;

};

#endif // JT808HEARTBEATREQUEST_H
