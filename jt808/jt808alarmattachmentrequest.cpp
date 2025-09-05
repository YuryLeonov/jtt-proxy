#include "jt808alarmattachmentrequest.h"

#include "tools.h"
#include "alarmtypes.h"
#include <filesystem>

#include <iostream>

#include <algorithm>

JT808AlarmAttachmentRequest::JT808AlarmAttachmentRequest(const uint8_t &jtType, const std::vector<std::string> &paths, const std::vector<uint8_t> &alID, const std::vector<uint8_t> &alNumber,uint8_t alType, const TerminalInfo &info) :
    JT808MessageFormatter(info),
    jt808AlarmType(jtType),
    alarmID(std::move(alID)),
    alarmType(alType),
    videoPaths(paths),
    alarmNumber(std::move(alNumber))
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
    std::cout << "TerminalID = ";
    tools::printHexBitStream(terminalIDBytes);

    bodyStream.insert(bodyStream.begin(), terminalIDBytes.begin(), terminalIDBytes.end());

    bodyStream.insert(bodyStream.end(), alarmID.begin(), alarmID.end());
    bodyStream.insert(bodyStream.end(), alarmNumber.begin(), alarmNumber.end());

    std::cout << "alarmID = ";
    tools::printHexBitStream(alarmID);

    std::cout << "AlarmNumber = ";
    tools::printHexBitStream(alarmNumber);

    bodyStream.push_back(0x00); //Normal file
    bodyStream.push_back(static_cast<uint8_t>(videoPaths.size()));


    int i = 0;
    for(const auto &path : videoPaths) {
        std::vector<uint8_t> fileInfoStream;

        std::string alarmNumStr = tools::getStringFromBitStream(alarmID);
        alarmNumStr.erase(std::remove_if(alarmNumStr.begin(), alarmNumStr.end(), ::isspace), alarmNumStr.end());

        std::string alarmTypeStr = "";
        std::string channelStr = "";
        std::string serialStr = "";

        std::stringstream ss;
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(jt808AlarmType);
        const std::string alarmCode = ss.str();

        if(alarmType == 0x65) {
            channelStr = "65_";
            alarmTypeStr = std::string("65").append(alarmCode) + "_";
            if(i == 0)
                serialStr = "0_";
            else
                serialStr = "1_";
        } else if(alarmType == 0x64) {
            channelStr = "64_";
            alarmTypeStr = std::string("64").append(alarmCode) + "_";
            serialStr = "1_";
        }

        ++i;

        const std::string fileName = std::string("02_") + channelStr + alarmTypeStr + serialStr + alarmNumStr + std::string("_") + std::string("h264");
        std::cout << fileName << std::endl;
        int fileNameSize = fileName.size();
        std::cout << "Длина имени файла: " << fileNameSize << std::endl;
        const uint32_t fileSize = std::filesystem::file_size(path);

        fileInfoStream.push_back(static_cast<uint8_t>(fileNameSize));
        const std::vector<uint8_t> fileNameBytes = tools::getUint8VectorFromString(fileName);
        fileInfoStream.insert(fileInfoStream.end(), fileNameBytes.begin(), fileNameBytes.end());
        tools::addToStdVector(fileInfoStream, fileSize);

        bodyStream.insert(bodyStream.end(), fileInfoStream.begin(), fileInfoStream.end());
    }


    //Header
    setHeader(0x01210);

    //Full message
    formFullMessage();

    return messageStream;
}
