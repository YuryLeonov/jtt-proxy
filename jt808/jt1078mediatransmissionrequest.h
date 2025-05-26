#ifndef JT1078MEDIATRANSMISSIONREQUEST_H
#define JT1078MEDIATRANSMISSIONREQUEST_H

#include "jt808messageformatter.h"
#include "mediatransmissionparams.h"

class JT1078MediaTransmissionRequest : public JT808MessageFormatter
{
public:
    JT1078MediaTransmissionRequest(const TerminalInfo &tInfo, const MediaTransmissionParams &p, const std::vector<uint8_t> &d);
    ~JT1078MediaTransmissionRequest();

    std::vector<uint8_t> getRequest() override;

private:
    MediaTransmissionParams params;

    const uint16_t frameID = 0x30;

    std::vector<uint8_t> data;

};

#endif // JT1078MEDIATRANSMISSIONREQUEST_H
