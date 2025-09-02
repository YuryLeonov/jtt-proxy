#ifndef JT808ALARMATTACHMENTREQUEST_H
#define JT808ALARMATTACHMENTREQUEST_H

#include "jt808messageformatter.h"
#include "inttypes.h"

class JT808AlarmAttachmentRequest : public JT808MessageFormatter
{
public:
    JT808AlarmAttachmentRequest(const uint8_t &jtType, const std::vector<std::string> &paths, const std::vector<uint8_t> &alID, const std::vector<uint8_t> &alNumber, uint8_t alType, const TerminalInfo &info);
    ~JT808AlarmAttachmentRequest();


    std::vector<uint8_t> getRequest() override;

private:
    std::vector<uint8_t> alarmID;
    std::vector<uint8_t> alarmNumber;
    uint8_t alarmType = 0x00;
    uint8_t jt808AlarmType = 0x10;
    std::vector<std::string> videoPaths;
};

#endif // JT808ALARMATTACHMENTREQUEST_H
