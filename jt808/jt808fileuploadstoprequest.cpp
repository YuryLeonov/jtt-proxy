#include "jt808fileuploadstoprequest.h"

JT808FileUploadStopRequest::JT808FileUploadStopRequest(const TerminalInfo &info) :
    JT808MessageFormatter(info)
{

}

JT808FileUploadStopRequest::~JT808FileUploadStopRequest()
{

}

std::vector<uint8_t> JT808FileUploadStopRequest::getRequest()
{
    clearMessageStream();

    //Body

    //Header
    setHeader(0x1212);

    //Full message
    formFullMessage();

    return messageStream;
}
