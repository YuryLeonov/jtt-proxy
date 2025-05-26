#ifndef JT808REGISTRATIONREQUEST_H
#define JT808REGISTRATIONREQUEST_H

#include "jt808messageformatter.h"

class JT808RegistrationRequest : public JT808MessageFormatter
{
public:
    JT808RegistrationRequest(const TerminalInfo &info);
    ~JT808RegistrationRequest();

    std::vector<uint8_t> getRequest() override;

};

#endif // JT808REGISTRATIONREQUEST_H
