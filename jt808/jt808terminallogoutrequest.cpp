#include "jt808terminallogoutrequest.h"

JT808TerminalLogoutRequest::JT808TerminalLogoutRequest(const TerminalInfo &info) : JT808MessageFormatter(info)
{

}

JT808TerminalLogoutRequest::~JT808TerminalLogoutRequest()
{

}

std::vector<uint8_t> JT808TerminalLogoutRequest::getRequest()
{
    clearMessageStream();

    setHeader(0x0003);

    formFullMessage();

    return messageStream;
}
