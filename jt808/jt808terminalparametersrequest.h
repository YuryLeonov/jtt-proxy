#ifndef JT808TERMINARPARAMETERSREQUEST_H
#define JT808TERMINARPARAMETERSREQUEST_H

#include "jt808messageformatter.h"
#include "terminalparams.h"

class JT808TerminalParametersRequest : public JT808MessageFormatter
{
public:
    JT808TerminalParametersRequest(const TerminalInfo &info, const TerminalParameters &p, uint8_t paramsCount);
    JT808TerminalParametersRequest(const TerminalInfo &info, const TerminalParameters &p, uint8_t paramsCount, uint16_t rID);
    ~JT808TerminalParametersRequest();

    std::vector<uint8_t> getRequest() override;

private:
    TerminalParameters params;

    uint8_t paramsCount = 0;
    uint16_t replyID = 0;
    bool isResponse = false;

};

#endif // JT808TERMINARPARAMETERSREQUEST_H
