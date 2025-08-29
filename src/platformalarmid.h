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
    std::string time;
    std::vector<uint8_t> alarmID;
    std::vector<std::string> videoPaths;

    void printInfo() {
        std::cout << "LocalID: " << databaseID << std::endl;
        std::cout << "AlarmType: " << std::hex << static_cast<int>(alarmJT808Type) << std::endl;
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

struct UnuploadedAlarm
{
    std::vector<uint8_t> alarmID;
    std::vector<uint8_t> alarmNumber;
};

struct PlatformAlarmID
{
    std::vector<uint8_t> id;
    std::vector<uint8_t> number;
    std::time_t time;
};

#endif // PLATFORMALARMID_H
