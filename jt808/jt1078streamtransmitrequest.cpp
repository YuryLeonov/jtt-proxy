#include "jt1078streamtransmitrequest.h"

JT1078StreamTransmitRequest::JT1078StreamTransmitRequest(const TerminalInfo &tInfo, const RTPParams &p, const std::vector<uint8_t> &d) :
    JT808MessageFormatter(terminalInfo),
    rtpParams(p),
    rtpData(std::move(d))
{

}

JT1078StreamTransmitRequest::~JT1078StreamTransmitRequest()
{

}

std::vector<uint8_t> JT1078StreamTransmitRequest::getRequest()
{
    clearMessageStream();

    //body
    const uint32_t frameHeaderID = 0x30;
    tools::addToStdVector(bodyStream, frameHeaderID);



    setHeader(0x9101);

    formFullMessage();

    return messageStream;
}
