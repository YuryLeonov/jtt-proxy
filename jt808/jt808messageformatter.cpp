#include "jt808messageformatter.h"
#include <easylogging++.h>

JT808MessageFormatter::JT808MessageFormatter(const TerminalInfo &info) :
    terminalInfo(info)
{

}

JT808MessageFormatter::~JT808MessageFormatter()
{

}

std::vector<uint8_t> JT808MessageFormatter::getBody() const
{
    return bodyStream;
}

void JT808MessageFormatter::setHeader(uint16_t messageID)
{
    headerStream.clear();

    //MessageID
    tools::addToStdVector(headerStream, messageID);

    //Information of body
    uint16_t bodyInfo = 0;
    setBodyInfo(bodyInfo);
    tools::addToStdVector(headerStream, bodyInfo);

    //PhoneNumber
    std::vector<std::string> numbers = tools::split(terminalInfo.phoneNumber, '-');
    if(numbers.size() == 6) {
        for(const auto &numStr : numbers) {
            const uint8_t num = static_cast<uint8_t>(std::stoi(numStr));
            uint8_t bcdNum = tools::to_bcd(num);
            headerStream.push_back(bcdNum);
        }
    }

    //Serial number
    messageSerialNum = tools::random_hex_uint16();
    tools::addToStdVector(headerStream, messageSerialNum);

    //Packets incapsulation if there is
//    if(isPacketsIncapsulated) {

//    }
}

void JT808MessageFormatter::setBodyInfo(uint16_t &info)
{
    //body length
    const uint16_t bodySize = static_cast<uint16_t>(bodyStream.size());
    if(tools::getBit(bodySize,0)) tools::setBit(info, 0);
    if(tools::getBit(bodySize,1)) tools::setBit(info, 1);
    if(tools::getBit(bodySize,2)) tools::setBit(info, 2);
    if(tools::getBit(bodySize,3)) tools::setBit(info, 3);
    if(tools::getBit(bodySize,4)) tools::setBit(info, 4);
    if(tools::getBit(bodySize,5)) tools::setBit(info, 5);
    if(tools::getBit(bodySize,6)) tools::setBit(info, 6);
    if(tools::getBit(bodySize,7)) tools::setBit(info, 7);
    if(tools::getBit(bodySize,8)) tools::setBit(info, 8);

    //EncryptionInfo

    //subcontract
//    isPacketsIncapsulated = true;
    //reserve
}

void JT808MessageFormatter::setCheckSum()
{
    checkSum = 0x00;

    for(uint8_t elem : messageStream) {
        checkSum ^= elem;
    }

    messageStream.push_back(checkSum);
}

void JT808MessageFormatter::formFullMessage()
{
    messageStream.insert(messageStream.end(), headerStream.begin(), headerStream.end());
    messageStream.insert(messageStream.end(), bodyStream.begin(), bodyStream.end());
    setCheckSum();

    tools::replaceByteInVectorWithTwo(messageStream, 0x7d, 0x7d, 0x01);
    tools::replaceByteInVectorWithTwo(messageStream, 0x7e, 0x7d, 0x02);
    if(tools::isByteInStream(messageStream, 0x7e)) {
        LOG(ERROR) << "Ошибка замены байт в сообщении" << std::endl;
    }

    messageStream.insert(messageStream.begin(), 0x7E);
    messageStream.insert(messageStream.end(), 0x7E);
}

void JT808MessageFormatter::clearMessageStream()
{
    messageStream.clear();
    bodyStream.clear();
}
