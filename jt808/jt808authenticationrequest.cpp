#include "jt808authenticationrequest.h"

JT808AuthenticationRequest::JT808AuthenticationRequest(const std::vector<uint8_t> &key, const TerminalInfo &info) : JT808MessageFormatter(info),
    authenticationKey(key)
{

}

JT808AuthenticationRequest::~JT808AuthenticationRequest()
{

}

std::vector<uint8_t> JT808AuthenticationRequest::getRequest()
{
    clearMessageStream();

    //body
    bodyStream.insert(bodyStream.end(), authenticationKey.begin(), authenticationKey.end());

    //Header
    setHeader(0x0102);

    //Full message
    formFullMessage();

    return messageStream;
}
