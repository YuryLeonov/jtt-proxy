#include "jt808alarmattachmentrequest.h"

JT808AlarmAttachmentRequest::JT808AlarmAttachmentRequest(const std::vector<uint8_t> &alID, const std::vector<uint8_t> &alNuMber,const std::vector<uint8_t> &addInfo, uint8_t attachmentsNum, const TerminalInfo &info) :
    JT808MessageFormatter(info),
    alarmID(std::move(alID)),
    alarmNumber(std::move(alNuMber)),
    additionalInfo(std::move(addInfo)),
    attachmentsNumber(attachmentsNum)
{

}

JT808AlarmAttachmentRequest::~JT808AlarmAttachmentRequest()
{

}

std::vector<uint8_t> JT808AlarmAttachmentRequest::getRequest()
{
    clearMessageStream();

    //Body
    std::vector<uint8_t> terminalIDBytes = tools::getUint8VectorFromString(terminalInfo.terminalID);
    bodyStream.insert(bodyStream.begin(), terminalIDBytes.begin(), terminalIDBytes.end());

    bodyStream.insert(bodyStream.end(), alarmID.begin(), alarmID.end());
    bodyStream.insert(bodyStream.end(), alarmNumber.begin(), alarmNumber.end());


    bodyStream.push_back(0x00);
    bodyStream.push_back(attachmentsNumber);

    bodyStream.insert(bodyStream.end(), additionalInfo.begin(), additionalInfo.end());

    //Header
    setHeader(0x01210);

    //Full message
    formFullMessage();

    return messageStream;
}
