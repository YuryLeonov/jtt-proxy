#include "jt808mediauploadeventinforequest.h"

JT808MediaUploadEventInfoRequest::JT808MediaUploadEventInfoRequest(const TerminalInfo &info) : JT808MessageFormatter(info)
{

}

JT808MediaUploadEventInfoRequest::~JT808MediaUploadEventInfoRequest()
{

}

std::vector<uint8_t> JT808MediaUploadEventInfoRequest::getRequest()
{
    clearMessageStream();

    tools::addToStdVector(bodyStream, multimediaID);
    bodyStream.push_back(0x02);
    bodyStream.push_back(0x06);
    bodyStream.push_back(0x01);
    bodyStream.push_back(0x01);

    setHeader(0x0800);

    formFullMessage();

    return messageStream;
}
