#ifndef ALARMTYPES_H
#define ALARMTYPES_H

#include <unordered_map>

namespace alarms {

    std::unordered_map<int, uint8_t> alarmsMap
    {
        {1, 0x00},
        {1, 0x01}
    };
}

#endif // ALARMTYPES_H
