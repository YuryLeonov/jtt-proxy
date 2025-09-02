#ifndef PLATFORMALARMID_H
#define PLATFORMALARMID_H

#include <vector>
#include <chrono>
#include "inttypes.h"
#include "tools.h"

struct SendedToPlatformAlarm
{
    std::string databaseID;
    uint8_t alarmJT808Type;
    uint8_t alarmType = 0x00;
    std::string time;
    std::vector<uint8_t> alarmID;
    std::vector<std::string> videoPaths;
    std::time_t updateTime;

    void printInfo() const {
        std::cout << "LocalID: " << databaseID << std::endl;
        std::cout << "JT808AlarmTypeID: " << std::hex << static_cast<int>(alarmType) << std::endl;
        std::cout << "JT808AlarmType: " << std::hex << static_cast<int>(alarmJT808Type) << std::endl;
        std::cout << "Alarm time: " << time << std::endl;
        std::cout << "AlarmID: ";
        tools::printHexBitStream(alarmID);
        std::cout << "VideoPaths: " << std::endl;
        for(const auto &path : videoPaths) {
            std::cout << "\t" << path << std::endl;
        }
        std::cout << std::endl;
    }

};

struct UploadingRequest
{
    std::vector<uint8_t> alarmID;
    std::vector<uint8_t> alarmNumber;
    std::time_t updateTime;

    void printInfo() const {
        std::cout << "Request: " << std::endl;
        tools::printHexBitStream(alarmID);
        tools::printHexBitStream(alarmNumber);
        std::cout << std::endl;
    }

};

#endif // PLATFORMALARMID_H
