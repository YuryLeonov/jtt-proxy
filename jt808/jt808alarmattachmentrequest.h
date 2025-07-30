#ifndef JT808ALARMATTACHMENTREQUEST_H
#define JT808ALARMATTACHMENTREQUEST_H

#include "jt808messageformatter.h"

class JT808AlarmAttachmentRequest : public JT808MessageFormatter
{
public:
    JT808AlarmAttachmentRequest(const std::vector<uint8_t> &alID, const std::vector<uint8_t> &alNuMber, const std::vector<uint8_t> &addInfo, uint8_t attachmentsNum, const TerminalInfo &info);
    ~JT808AlarmAttachmentRequest();


    std::vector<uint8_t> getRequest() override;

private:
    std::vector<uint8_t> alarmID;
    std::vector<uint8_t> alarmNumber;
    std::vector<uint8_t> additionalInfo;
    uint8_t attachmentsNumber;


};

#endif // JT808ALARMATTACHMENTREQUEST_H
