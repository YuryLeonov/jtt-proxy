#include "jt808fileuploadstoprequest.h"

#include <filesystem>

namespace fs = std::filesystem;

JT808FileUploadStopRequest::JT808FileUploadStopRequest(const std::string &pathToFile, const TerminalInfo &info) :
    JT808MessageFormatter(info),
    filePath(pathToFile)
{

}

JT808FileUploadStopRequest::~JT808FileUploadStopRequest()
{

}

std::vector<uint8_t> JT808FileUploadStopRequest::getRequest()
{
    clearMessageStream();

    //Body
    fs::path file_path = filePath;
    const std::string fileName = file_path.filename();
    const uint8_t fileNameSize = fileName.length();
    const uint32_t fileSize = static_cast<uint32_t>(fs::file_size(filePath));
    const uint8_t fileType = 0x02;

    bodyStream.push_back(fileNameSize);
    const std::vector<uint8_t> fileNameBytes = tools::getUint8VectorFromString(fileName);
    bodyStream.insert(bodyStream.end(), fileNameBytes.begin(), fileNameBytes.end());
    bodyStream.push_back(fileType);
    tools::addToStdVector(bodyStream, fileSize);

    //Header
    setHeader(0x1212);

    //Full message
    formFullMessage();

    return messageStream;
}
