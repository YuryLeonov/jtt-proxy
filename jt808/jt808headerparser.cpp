#include "jt808headerparser.h"

JT808HeaderParser::JT808HeaderParser()
{

}

JT808HeaderParser::~JT808HeaderParser()
{

}

JT808Header JT808HeaderParser::getHeader(const std::vector<uint8_t> &message)
{
    JT808Header header;

    header.messageID = (message[1] << 8) | message[2];
    header.messageSerialNumber = (message[9] << 8) | message[10];

    return header;
}
