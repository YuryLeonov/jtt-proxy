#ifndef JT1078UPLOADEDRESOURCESLISTREQUEST_H
#define JT1078UPLOADEDRESOURCESLISTREQUEST_H

#include "jt808messageformatter.h"

struct UploadedResource
{
    uint8_t channelNumber;
    uint8_t startYear;
    uint8_t startMonth;
    uint8_t startDay;
    uint8_t startHour;
    uint8_t startMin;
    uint8_t startSec;
    uint8_t stopYear;
    uint8_t stopMonth;
    uint8_t stopDay;
    uint8_t stopHour;
    uint8_t stopMin;
    uint8_t stopSec;
    uint64_t alarmSign;
    uint8_t resourceType;
    uint8_t streamType;
    uint8_t memoryType;
    uint32_t fileSize;
};

class JT1078UploadedResourcesListRequest : public JT808MessageFormatter
{
public:
    JT1078UploadedResourcesListRequest(uint16_t serialNum, const UploadedResource &rsc, const TerminalInfo &info);
    ~JT1078UploadedResourcesListRequest();

    std::vector<uint8_t> getRequest() override;


private:
    uint16_t messageSerialNumber;

    UploadedResource resource;

};

#endif // JT1078UPLOADEDRESOURCESLISTREQUEST_H
