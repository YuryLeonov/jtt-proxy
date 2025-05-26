#include "jt808generalresponserequest.h"

#include "tools.h"

JT808GeneralResponseRequest::JT808GeneralResponseRequest(const TerminalInfo &info, uint16_t rsn, uint16_t rID, JT808GeneralResponseRequest::Result res) : JT808MessageFormatter(info),
    replySerialNumber(rsn),
    replyID(rID),
    result(res)
{

}

JT808GeneralResponseRequest::~JT808GeneralResponseRequest()
{

}

std::vector<uint8_t> JT808GeneralResponseRequest::getRequest()
{
    clearMessageStream();

    //body
    tools::addToStdVector(bodyStream, replySerialNumber);
    tools::addToStdVector(bodyStream, replyID);
    uint8_t r = static_cast<uint8_t>(result);
    bodyStream.push_back(r);

    //Header
    setHeader(0x0001);

    //Full message
    formFullMessage();

    return messageStream;
}
