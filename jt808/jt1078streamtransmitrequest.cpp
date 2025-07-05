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
    //V bits
    tools::setBit(byte1, 7);

    //CC bits
    tools::setBit(byte1, 0);

    bodyStream.push_back(byte1);


    uint8_t byte2 = 0;
    if(rtpParams.mMarker)
        tools::setBit(byte2, 7);
    bodyStream.push_back(byte2);

    //Package serial number
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

    //Logical channel
    bodyStream.push_back(rtpParams.logicalNumber);

    //Type of data and subcontract
    uint8_t byte3 = 0;
    if(rtpParams.frameType == FrameType::PFrame) {
        tools::setBit(byte3, 4);
    } else if(rtpParams.frameType == FrameType::BFrame) {
        tools::setBit(byte3, 5);
    }
    bodyStream.push_back(byte3);

    //timestamp
    for(int i = 0; i < 8; ++i) {
        bodyStream.push_back(0x01);
    }

    //Last I frame interval
    tools::addToStdVector(bodyStream, rtpParams.lastIFrameInterval);

    //Previous frame interval
    tools::addToStdVector(bodyStream, rtpParams.lastFrameInterval);

    //Data body length
    const uint16_t dataLength = rtpData.size();
    tools::addToStdVector(bodyStream, dataLength);

    //Frame body
    bodyStream.insert(bodyStream.end(), rtpData.begin(), rtpData.end());



    setHeader(0x9101);

    formFullMessage();

    return messageStream;
}
