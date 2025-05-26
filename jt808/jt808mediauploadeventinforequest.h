#ifndef JT808MEDIAUPLOADEVENTINFOREQUEST_H
#define JT808MEDIAUPLOADEVENTINFOREQUEST_H

#include "jt808messageformatter.h"

class JT808MediaUploadEventInfoRequest : public JT808MessageFormatter
{
public:
    JT808MediaUploadEventInfoRequest(const TerminalInfo &info);
    ~JT808MediaUploadEventInfoRequest();

    std::vector<uint8_t> getRequest() override;

private:
    const uint32_t multimediaID = 0x00000100;

};

#endif // JT808MEDIAUPLOADEVENTINFOREQUEST_H
