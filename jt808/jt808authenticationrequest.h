#ifndef JT808AUTHENTICATIONREQUEST_H
#define JT808AUTHENTICATIONREQUEST_H

#include "jt808messageformatter.h"

class JT808AuthenticationRequest : public JT808MessageFormatter
{
public:
    JT808AuthenticationRequest(const std::vector<uint8_t> &key, const TerminalInfo &info);
    ~JT808AuthenticationRequest();

    std::vector<uint8_t> getRequest() override;

private:
    std::vector<uint8_t> authenticationKey;

};

#endif // JT808AUTHENTICATIONREQUEST_H
