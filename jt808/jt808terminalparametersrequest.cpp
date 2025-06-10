#include "jt808terminalparametersrequest.h"
#include "tools.h"

JT808TerminalParametersRequest::JT808TerminalParametersRequest(const TerminalInfo &info, const TerminalParameters &p, uint8_t pCount) : JT808MessageFormatter(info),
    params(p),
    paramsCount(pCount),
    isResponse(false)
{

}

JT808TerminalParametersRequest::JT808TerminalParametersRequest(const TerminalInfo &info, const TerminalParameters &p, uint8_t pCount, uint16_t rID) : JT808MessageFormatter(info),
    params(p),
    paramsCount(pCount),
    replyID(rID),
    isResponse(true)
{

}

JT808TerminalParametersRequest::~JT808TerminalParametersRequest()
{

}

std::vector<uint8_t> JT808TerminalParametersRequest::getRequest()
{
    messageStream.clear();
    bodyStream.clear();

    //Body
    uint8_t paramLength = 0;
    uint32_t paramID;

    if(isResponse) {
        tools::addToStdVector(bodyStream, replyID);
    }

    bodyStream.push_back(paramsCount);

    paramID = 0x0001;
    tools::addToStdVector(bodyStream, paramID);
    paramLength = sizeof(params.terminalHeartbeatTimeout);
    bodyStream.push_back(paramLength);
    tools::addToStdVector(bodyStream, params.terminalHeartbeatTimeout);

//    paramID = 0x0013;
//    tools::addToStdVector(bodyStream, paramID);
//    std::vector<uint8_t> ipCode = tools::getUint8VectorFromString(params.primaryServerIpAddress);
//    paramLength = ipCode.size();
//    bodyStream.push_back(paramLength);
//    bodyStream.insert(bodyStream.end(), ipCode.begin(), ipCode.end());

//    paramID = 0x0018;
//    tools::addToStdVector(bodyStream, paramID);
//    paramLength = sizeof(params.serverTCPPort);
//    bodyStream.push_back(paramLength);
//    tools::addToStdVector(bodyStream, params.serverTCPPort);

//    paramID = 0x0019;
//    tools::addToStdVector(bodyStream, paramID);
//    paramLength = sizeof(params.serverUDPPort);
//    bodyStream.push_back(paramLength);
//    tools::addToStdVector(bodyStream, params.serverUDPPort);

    //Header
    setHeader(0x8103);

    //Full message
    formFullMessage();

    return messageStream;
}
