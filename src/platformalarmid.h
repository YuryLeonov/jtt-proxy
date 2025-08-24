#ifndef PLATFORMALARMID_H
#define PLATFORMALARMID_H

#include <vector>
#include <chrono>
#include "inttypes.h"

struct PlatformAlarmID
{
    std::vector<uint8_t> id;
    std::vector<uint8_t> number;
    std::time_t time;
};

#endif // PLATFORMALARMID_H
