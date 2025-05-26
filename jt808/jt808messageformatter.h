#ifndef JT808MESSAGEFORMATTER_H
#define JT808MESSAGEFORMATTER_H

#include <inttypes.h>
#include <vector>

#include "TerminalInfo.h"
#include "tools.h"

class JT808MessageFormatter
{
public:
    JT808MessageFormatter(const TerminalInfo &info);
    virtual ~JT808MessageFormatter();

    virtual std::vector<uint8_t> getRequest() = 0;

    std::vector<uint8_t> getBody() const;

protected:
    void setHeader(uint16_t messageID);
    void setBodyInfo(uint16_t &info);
    void setCheckSum();
    void formFullMessage();
    void clearMessageStream();

protected:
    std::vector<uint8_t> messageStream;
    std::vector<uint8_t> bodyStream;
    std::vector<uint8_t> headerStream;
    uint8_t checkSum = 0;
    uint16_t messageSerialNum = 0;

    TerminalInfo terminalInfo;
};

#endif // JT808MESSAGEFORMATTER_H
