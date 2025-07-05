#ifndef JT1078STREAMTRANSMITREQUEST_H
#define JT1078STREAMTRANSMITREQUEST_H

#include "jt808messageformatter.h"

enum class FrameType
{
    IFrame = 0,
    PFrame,
    BFrame,
    Undefined
};

struct RTPParams
{
    uint16_t serialNumber = 0;
    uint8_t logicalNumber = 0;
    uint16_t lastIFrameInterval = 0;
    uint16_t lastFrameInterval = 0;
    bool mMarker = false;
    FrameType frameType;
};

class  JT1078StreamTransmitRequest : public JT808MessageFormatter
{
public:
    JT1078StreamTransmitRequest(const TerminalInfo &tInfo, const RTPParams &p, const std::vector<uint8_t> &d);
    ~JT1078StreamTransmitRequest();

    std::vector<uint8_t> getRequest() override;

private:
    RTPParams rtpParams;
    std::vector<uint8_t> rtpData;
};

#endif // JT1078STREAMTRANSMITREQUEST_H
