#include "jt808registrationrequest.h"

JT808RegistrationRequest::JT808RegistrationRequest(const TerminalInfo &info) : JT808MessageFormatter(info)
{

}

JT808RegistrationRequest::~JT808RegistrationRequest()
{

}

std::vector<uint8_t> JT808RegistrationRequest::getRequest()
{
    clearMessageStream();

    //Body
    bodyStream.push_back(terminalInfo.provinceID);
    bodyStream.push_back(terminalInfo.cityID);

    std::vector<uint8_t> manufacturerIDVec = std::move(tools::getUint8VectorFromString(terminalInfo.manufacturerID));
    bodyStream.insert(bodyStream.end(), manufacturerIDVec.begin(), manufacturerIDVec.end());

    std::vector<uint8_t> terminalModelVec = std::move(tools::getUint8VectorFromString(terminalInfo.terminalModel));
    bodyStream.insert(bodyStream.end(), terminalModelVec.begin(), terminalModelVec.end());

    std::vector<uint8_t> terminalIDVec = std::move(tools::getUint8VectorFromString(terminalInfo.terminalID));
    bodyStream.insert(bodyStream.end(), terminalIDVec.begin(), terminalIDVec.end());

    bodyStream.push_back(terminalInfo.licencePlateColor);

    bodyStream.push_back(0xFF);
    bodyStream.push_back(0xEE);
    bodyStream.push_back(0xDD);
    bodyStream.push_back(0xCC);
    bodyStream.push_back(0xBB);
    bodyStream.push_back(0xAA);

    //Header
    setHeader(0x0100);

    //Full message
    formFullMessage();

    return messageStream;
}
