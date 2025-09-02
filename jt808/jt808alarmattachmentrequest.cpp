#include "jt808alarmattachmentrequest.h"

#include "tools.h"
#include "alarmtypes.h"
#include <filesystem>

#include <iostream>

#include <algorithm>

JT808AlarmAttachmentRequest::JT808AlarmAttachmentRequest(const uint8_t &jtType, const std::string &fPath, const std::vector<uint8_t> &alID, const std::vector<uint8_t> &alNuMber,uint8_t alType, uint8_t attachmentsNum, const TerminalInfo &info) :
    JT808MessageFormatter(info),
    jt808AlarmType(jtType),
    filePath(fPath),
    alarmID(std::move(alID)),
    alarmType(alType),
    alarmNumber(std::move(alNuMber)),
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

    std::string alarmNumStr = tools::hex_bytes_to_string(alarmNumber);
    alarmNumStr.erase(std::remove_if(alarmNumStr.begin(), alarmNumStr.end(), ::isspace), alarmNumStr.end());

    std::string alarmTypeStr = "";
    std::string channelStr = "";
    std::string serialStr = "";

    if(alarmType == 0x65) {
        channelStr = "65_";
//        alarmTypeStr = std::string("65").append(std::to_string(static_cast<int>(jt808AlarmType))) + "_";
        alarmTypeStr = std::string("65").append("02") + "_";
        serialStr = "1_";
    } else if(alarmType == 0x64) {
        channelStr = "64_";
        alarmTypeStr = std::string("64").append(std::to_string(static_cast<int>(jt808AlarmType))) + "_";
        serialStr = "2_";
    }

    const std::string fileName = std::string("02_") + channelStr + alarmTypeStr + serialStr + alarmNumStr + std::string("_") + std::string("h264");
    std::cout << fileName << std::endl;
    const uint8_t fileNameSize = fileName.length();
    const uint32_t fileSize = std::filesystem::file_size(filePath);

    bodyStream.push_back(fileNameSize);
    const std::vector<uint8_t> fileNameBytes = tools::getUint8VectorFromString(fileName);
    bodyStream.insert(bodyStream.end(), fileNameBytes.begin(), fileNameBytes.end());
    tools::addToStdVector(bodyStream, fileSize);

    //Header
    setHeader(0x01210);

    //Full message
    formFullMessage();

    return messageStream;
}
