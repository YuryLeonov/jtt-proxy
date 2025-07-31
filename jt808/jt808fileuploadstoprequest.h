#ifndef JT808FILEUPLOADSTOPREQUEST_H
#define JT808FILEUPLOADSTOPREQUEST_H

#include "jt808messageformatter.h"

class JT808FileUploadStopRequest : public JT808MessageFormatter
{
public:
    JT808FileUploadStopRequest(const std::string &pathToFile, const TerminalInfo &info);
    ~JT808FileUploadStopRequest();

    std::vector<uint8_t> getRequest() override;

private:
    std::string filePath = "";

};

#endif // JT808FILEUPLOADSTOPREQUEST_H
