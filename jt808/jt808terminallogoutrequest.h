#ifndef JT808TERMINALLOGOUTREQUEST_H
#define JT808TERMINALLOGOUTREQUEST_H

#include "jt808messageformatter.h"

class JT808TerminalLogoutRequest : public JT808MessageFormatter
{
public:
    JT808TerminalLogoutRequest(const TerminalInfo &info);
    ~JT808TerminalLogoutRequest();

    std::vector<uint8_t> getRequest() override;

};

#endif // JT808TERMINALLOGOUTREQUEST_H
