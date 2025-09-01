#ifndef TERMINALINFO_H
#define TERMINALINFO_H

#include <string>
#include <inttypes.h>

struct TerminalStatus
{
    bool isACCOn = true;
    bool isPositioned = true;
    bool isSouthLatitude = false;
    bool isWestLongitude = false;
    bool isRunningStatus = false;
    bool isCoordinatesEncrypted = false;
    int loadLevel = 0;
    bool vehicleOilCurcuit = false;
    bool vehicleCurcuit = false;
    bool isDoorLocked = false;
    bool isFrontDoorOpened = false;
    bool isMiddleDoorOpened = false;
    bool isBackDoorOpened = false;
    bool isDriverDoorOpened = false;
    bool isFifthDoorOpened = false;
    bool isGPSUsing = true;
    bool isBeidouUsing = false;
    bool isGlonassUsing = false;
    bool isGalileoUsing = false;
    uint8_t satellitesCount = 0;
    int alarmVideosCount = 0;
    int alarmVideosWaitInterval = 30000;
};

struct TerminalInfo
{
    std::string phoneNumber;
    uint16_t provinceID;
    uint16_t cityID;
    std::string manufacturerID;
    std::string terminalModel;
    std::string terminalID;
    uint8_t licencePlateColor;
    std::string vin;
    TerminalStatus status;
};

#endif // TERMINALINFO_H
