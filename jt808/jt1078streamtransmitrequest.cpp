#include "jt1078streamtransmitrequest.h"

#include <iostream>

JT1078StreamTransmitRequest::JT1078StreamTransmitRequest(const TerminalInfo &tInfo, const RTPParams &p, const std::vector<uint8_t> &d) :
    JT808MessageFormatter(tInfo),
    rtpParams(p),
    rtpData(d)
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

    uint8_t byte1 = 0;
    tools::setBit(byte1, 0);
    tools::setBit(byte1, 7);
    bodyStream.push_back(byte1);

    uint8_t byte2 = 0;
    bodyStream.push_back(byte2);

    tools::addToStdVector(bodyStream, rtpParams.serialNumber);

    //PhoneNumber
    std::vector<std::string> numbers = tools::split(terminalInfo.phoneNumber, '-');
    if(numbers.size() == 6) {
        for(const auto &numStr : numbers) {
            const uint8_t num = static_cast<uint8_t>(std::stoi(numStr));
            uint8_t bcdNum = tools::to_bcd(num);
            bodyStream.push_back(bcdNum);
        }
    }

    bodyStream.push_back(rtpParams.logicalNumber);

    uint8_t byte3 = 0;
    bodyStream.push_back(byte3);

    for(int i = 0; i < 8; ++i) {
        bodyStream.push_back(0x01);
    }

    tools::addToStdVector(bodyStream, rtpParams.lastIFrameInterval);
    tools::addToStdVector(bodyStream, rtpParams.lastFrameInterval);

    const uint16_t dataLength = rtpData.size();
    tools::addToStdVector(bodyStream, dataLength);

    bodyStream.insert(bodyStream.end(), rtpData.begin(), rtpData.end());


    setHeader(0x9101);

    formFullMessage();

    return messageStream;
}
