#ifndef JT808GENERALRESPONSEREQUEST_H
#define JT808GENERALRESPONSEREQUEST_H

#include "jt808messageformatter.h"

class JT808GeneralResponseRequest : public JT808MessageFormatter
{
public:

    enum class Result
    {
        Success = 0,
        Failure,
        MessageError,
        NotSupported
    };

    JT808GeneralResponseRequest(const TerminalInfo &info, uint16_t rsn, uint16_t rID, Result res);
    ~JT808GeneralResponseRequest();

    std::vector<uint8_t> getRequest() override;

private:
    uint16_t replySerialNumber = 0;
    uint16_t replyID = 0;
    Result result;

};

#endif // JT808GENERALRESPONSEREQUEST_H
