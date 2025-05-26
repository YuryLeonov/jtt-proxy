#include "jt1078mediatransmissionrequest.h"

#include "tools.h"

JT1078MediaTransmissionRequest::JT1078MediaTransmissionRequest(const TerminalInfo &tInfo, const MediaTransmissionParams &p, const std::vector<uint8_t> &d) : JT808MessageFormatter(tInfo),
    params(p),
    data(std::move(d))
{

}

JT1078MediaTransmissionRequest::~JT1078MediaTransmissionRequest()
{

}

std::vector<uint8_t> JT1078MediaTransmissionRequest::getRequest()
{
    clearMessageStream();

    //Body
    tools::addToStdVector(bodyStream, frameID);

    uint8_t h1 = 0;
    tools::setBit(h1, 7);
    tools::setBit(h1, 0);
    bodyStream.push_back(h1);

    uint8_t h2 = 0;
    bodyStream.push_back(h2);

    tools::addToStdVector(bodyStream, params.packageSerialNumber);

    std::vector<std::string> numbers = tools::split(terminalInfo.phoneNumber, '-');
    if(numbers.size() == 6) {
        for(const auto &numStr : numbers) {
            const uint8_t num = static_cast<uint8_t>(std::stoi(numStr));
            uint8_t bcdNum = tools::to_bcd(num);
            bodyStream.push_back(bcdNum);
        }
    }

    bodyStream.push_back(params.logicalChannelNumber);

    uint8_t h3 = 0;
    switch (params.frameType) {
    case FrameType::IFRAME:
        break;
    case FrameType::PFRAME:
        tools::setBit(h3, 4);
        break;
    case FrameType::BFRAME:
        tools::setBit(h3, 5);
        break;
    case FrameType::AUDIOFRAME:
        tools::setBit(h3, 4);
        tools::setBit(h3, 5);
        break;
    case FrameType::TRANSPARENT:
        tools::setBit(h3, 6);
        break;
    default:
        break;
    }

    switch (params.packageType) {
    case PackageType::ATOMIC:
        break;
    case PackageType::FIRST:
        tools::setBit(h3, 0);
        break;
    case PackageType::LAST:
        tools::setBit(h3, 1);
        break;
    case PackageType::INTERMEDIATE:
        tools::setBit(h3, 0);
        tools::setBit(h3, 1);
        break;
    default:
        break;
    }

    bodyStream.push_back(h3);


    const uint16_t dataLength = data.size();
    tools::addToStdVector(bodyStream, dataLength);

    bodyStream.insert(bodyStream.end(), data.begin(), data.end());


    //Header
    setHeader(0x9101);

    //Full message
    formFullMessage();

    return messageStream;
}
