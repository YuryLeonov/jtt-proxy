#ifndef JT808FILEUPLOADSTOPREQUEST_H
#define JT808FILEUPLOADSTOPREQUEST_H

#include "jt808messageformatter.h"

class JT808FileUploadStopRequest : public JT808MessageFormatter
{
public:
    JT808FileUploadStopRequest(const TerminalInfo &info);
    ~JT808FileUploadStopRequest();

    std::vector<uint8_t> getRequest() override;

};

#endif // JT808FILEUPLOADSTOPREQUEST_H
