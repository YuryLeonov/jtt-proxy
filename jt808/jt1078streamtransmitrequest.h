#ifndef JT1078STREAMTRANSMITREQUEST_H
#define JT1078STREAMTRANSMITREQUEST_H

#include "jt808messageformatter.h"

namespace  rtp {
    enum class FrameType
    {
        IFrame = 0,
        PFrame,
        BFrame,
        Undefined
    };

    enum class SubcontractType
    {
        Atomic = 0,
        First,
        Last,
        Intermediate
    };

    struct RTPParams
    {
        uint16_t serialNumber = 0;
        uint8_t logicalNumber = 0;
        uint16_t lastIFrameInterval = 0;
        uint16_t lastFrameInterval = 0;
        uint64_t timestamp = 0;
        bool mMarker = false;
        FrameType frameType;
        SubcontractType subcontractType;
    };
}


class  JT1078StreamTransmitRequest : public JT808MessageFormatter
{
public:
    JT1078StreamTransmitRequest(const TerminalInfo &tInfo, const rtp::RTPParams &p, const std::vector<uint8_t> &d);
    ~JT1078StreamTransmitRequest();

    std::vector<uint8_t> getRequest() override;

private:
    rtp::RTPParams rtpParams;
    std::vector<uint8_t> rtpData;
};

#endif // JT1078STREAMTRANSMITREQUEST_H
