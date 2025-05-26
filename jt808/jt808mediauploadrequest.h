#ifndef JT808MEDIAUPLOADREQUEST_H
#define JT808MEDIAUPLOADREQUEST_H

#include "jt808messageformatter.h"
#include <string>

class JT808MediaUploadRequest : public JT808MessageFormatter
{
public:
    JT808MediaUploadRequest(const TerminalInfo &info, const std::vector<uint8_t> &chk, const std::vector<uint8_t> &aBody);
    ~JT808MediaUploadRequest();

    std::vector<uint8_t> getRequest() override;

private:
    const uint32_t multimediaID = 0x00000100;

    std::vector<uint8_t> chunk;
    std::vector<uint8_t> alarmBody;
};

#endif // JT808MEDIAUPLOADREQUEST_H
