#include "jt808heartbeatrequest.h"

JT808HeartbeatRequest::JT808HeartbeatRequest(const TerminalInfo &info) : JT808MessageFormatter(info)
{

}

JT808HeartbeatRequest::~JT808HeartbeatRequest()
{

}

std::vector<uint8_t> JT808HeartbeatRequest::getRequest()
{
    clearMessageStream();

    setHeader(0x0002);

    formFullMessage();

    return messageStream;
}
