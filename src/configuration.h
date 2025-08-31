#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "easylogging++.h"

struct Configuration
{
    std::string terminalPhoneNumber = "";
    uint16_t provinceID = 0;
    uint16_t cityID = 0;
    std::string manufacturerID = "";
    std::string terminalModel = "";
    std::string terminalID = "";
    uint8_t licencePlateColor = 0;
    std::string vin = "";

    std::string eventsServerIP = "";
    int eventsServerPort = 0;
    int eventsServerReconnectTimeout = 3000;
    int eventsServerSurveyInterval = 5000;

    std::string platformServerIP = "";
    int platformServerPort = 0;
    int platformHeartBeatTimeout = 0;
    int platformReconnectTimeout = 1000;
    std::string videoServerConnectionType = "udp";
    std::vector<std::string> rtspLinks;

    std::string acc = "off";
    bool isPositioned = false;
    std::string latitude = "south";
    std::string longitude = "west";
    std::string operationStatus = "off";
    bool isCoordinatesEncrepted = false;
    int loadLevel = 0;
    std::string vehicleOilCircuitStatus = "off";
    std::string vehicleCircuitStatus = "off";
    std::string doorStatus = "locked";
    bool isFrontDoorOpened = false;
    bool isMiddleDoorOpened = false;
    bool isBackDoorOpened = false;
    bool isDriverDoorOpened = false;
    bool isFifthDoorOpened = false;
    bool isGPSUsing = false;
    bool isBeidouUsing = false;
    bool isGlonassUsing = false;
    bool isGalileoUsing = false;
    uint8_t satellitesCount = 0;
    int alarmVideosCount = 0;
 };

#endif // CONFIGURATION_H
