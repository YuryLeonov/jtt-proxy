#ifndef JT808FILEUPLOADINFOREQUEST_H
#define JT808FILEUPLOADINFOREQUEST_H

#include "jt808messageformatter.h"

#include <stdexcept>

class FileNotFoundException : public std::runtime_error
{
public:
    FileNotFoundException(const std::string errMessage) : std::runtime_error(errMessage) {}
};




class JT808FileUploadInfoRequest : public JT808MessageFormatter
{
public:
    JT808FileUploadInfoRequest(const std::string &pathToFile, const TerminalInfo &info);
    ~JT808FileUploadInfoRequest();

    std::vector<uint8_t> getRequest() override;


private:
    std::string filePath = "";

};

#endif // JT808FILEUPLOADINFOREQUEST_H
