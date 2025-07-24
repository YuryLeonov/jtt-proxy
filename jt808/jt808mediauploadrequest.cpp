#include "jt808mediauploadrequest.h"
#include "tools.h"

#include <iostream>

JT808MediaUploadRequest::JT808MediaUploadRequest(uint32_t mID, const TerminalInfo &info, const std::vector<uint8_t> &chk, const std::vector<uint8_t> &aBody) :
    JT808MessageFormatter(info),
    multimediaID(mID)
{
    chunk = std::move(chk);
    alarmBody = std::move(aBody);
}

JT808MediaUploadRequest::~JT808MediaUploadRequest()
{

}

std::vector<uint8_t> JT808MediaUploadRequest::getRequest()
{
    clearMessageStream();

    tools::addToStdVector(bodyStream, multimediaID);
    bodyStream.push_back(0x02);
    bodyStream.push_back(0x04);
    bodyStream.push_back(0x01);
    bodyStream.push_back(0x01);

    bodyStream.insert(bodyStream.end(), alarmBody.begin(), alarmBody.end());
    bodyStream.insert(bodyStream.end(), chunk.begin(), chunk.end());

    setHeader(0x0801);

    //Full message
    formFullMessage();

    return messageStream;
}
