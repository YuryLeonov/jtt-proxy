#ifndef JT808ALARMATTACHMENTREQUEST_H
#define JT808ALARMATTACHMENTREQUEST_H

#include "jt808messageformatter.h"

class JT808AlarmAttachmentRequest : public JT808MessageFormatter
{
public:
    JT808AlarmAttachmentRequest(const std::string &fPath, const std::vector<uint8_t> &alID, const std::vector<uint8_t> &alNuMber, uint8_t attachmentsNum, const TerminalInfo &info);
    ~JT808AlarmAttachmentRequest();


    std::vector<uint8_t> getRequest() override;

private:
    std::vector<uint8_t> alarmID;
    std::vector<uint8_t> alarmNumber;
    uint8_t attachmentsNumber;
    std::string filePath = "";

};

#endif // JT808ALARMATTACHMENTREQUEST_H
