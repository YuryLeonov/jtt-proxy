#include "jt1078uploadedresourceslistrequest.h"

JT1078UploadedResourcesListRequest::JT1078UploadedResourcesListRequest(uint16_t serialNum, const UploadedResource &rsc, const TerminalInfo &info) :
    JT808MessageFormatter(info),
    messageSerialNumber(serialNum),
    resource(rsc)
{

}

JT1078UploadedResourcesListRequest::~JT1078UploadedResourcesListRequest()
{

}

std::vector<uint8_t> JT1078UploadedResourcesListRequest::getRequest()
{
    clearMessageStream();

    //Body
    tools::addToStdVector(bodyStream, messageSerialNumber);

    const uint32_t resourcesIdle = 0;
    tools::addToStdVector(bodyStream, resourcesIdle);

    bodyStream.push_back(resource.channelNumber);

    bodyStream.push_back(tools::to_bcd(resource.startYear));
    bodyStream.push_back(tools::to_bcd(resource.startMonth));
    bodyStream.push_back(tools::to_bcd(resource.startDay));
    bodyStream.push_back(tools::to_bcd(resource.startHour));
    bodyStream.push_back(tools::to_bcd(resource.startMin));
    bodyStream.push_back(tools::to_bcd(resource.startSec));

    bodyStream.push_back(tools::to_bcd(resource.stopYear));
    bodyStream.push_back(tools::to_bcd(resource.stopMonth));
    bodyStream.push_back(tools::to_bcd(resource.stopDay));
    bodyStream.push_back(tools::to_bcd(resource.stopHour));
    bodyStream.push_back(tools::to_bcd(resource.stopMin));
    bodyStream.push_back(tools::to_bcd(resource.stopSec));

    tools::addToStdVector(bodyStream, resource.alarmSign);

    bodyStream.push_back(resource.resourceType);
    bodyStream.push_back(resource.streamType);
    bodyStream.push_back(resource.memoryType);
    bodyStream.push_back(resource.fileSize);

    //Header
    setHeader(0x1205);

    //Full message
    formFullMessage();

    return messageStream;
}
