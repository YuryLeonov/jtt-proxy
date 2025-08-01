#ifndef PLATFORMALARMID_H
#define PLATFORMALARMID_H

#include <vector>
#include "inttypes.h"

struct PlatformAlarmID
{
    std::vector<uint8_t> id;
    std::vector<uint8_t> number;
};

#endif // PLATFORMALARMID_H
