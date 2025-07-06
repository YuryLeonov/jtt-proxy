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
    std::string eventsServerTableName = "";
    int eventsServerReconnectTimeout = 3000;
    int eventsServerSurveyInterval = 5000;
    std::string videoRootPath = "";

    std::string platformServerIP = "";
    int platformServerPort = 0;
    int platformHeartBeatTimeout = 0;
    int platformReconnectTimeout = 1000;
    std::string videoServerConnectionType = "udp";
    std::string rtspLink = "";

    std::string localServerHost = "";
    int localServerPort = 8095;
    int localServerConnectionsCount = 3;

    void printInfo() const {
        LOG(INFO) << "Terminal phone number: " << terminalPhoneNumber;
        LOG(INFO) << "Province ID: " << provinceID;
        LOG(INFO) << "City ID: " << cityID;
        LOG(INFO) << "Manufacturer ID: " << manufacturerID;
        LOG(INFO) << "Terminal model: " << terminalModel;
        LOG(INFO) << "Terminal ID: " << terminalID;
        LOG(INFO) << "Licence plate color: " << licencePlateColor;
        LOG(INFO) << "VIN: " << vin;

        LOG(INFO) << "Events server ip: " << eventsServerIP;
        LOG(INFO) << "Events server port: " << eventsServerPort;
        LOG(INFO) << "Events server table name: " << eventsServerTableName;
        LOG(INFO) << "Events server reconnect timeout: " << eventsServerReconnectTimeout;
        LOG(INFO) << "Events server survey interval: " << eventsServerSurveyInterval;
        LOG(INFO) << "Root path to video files: " << videoRootPath;

        LOG(INFO) << "Platform server ip: " << platformServerIP;
        LOG(INFO) << "Platform server port: " << platformServerPort;
        LOG(INFO) << "Platform server heartbeat timeout: " << platformHeartBeatTimeout;
        LOG(INFO) << "Platform reconnect timeout: " << platformReconnectTimeout;
        LOG(INFO) << "Video server connection type: " << videoServerConnectionType;
        LOG(INFO) << "RTSP link to translate to video server: " << rtspLink;

        LOG(INFO) << "Local server host: " << localServerHost;
        LOG(INFO) << "Local server port: " << localServerPort;
        LOG(INFO) << "Local server connections allowed: " << localServerConnectionsCount << std::endl;
    }
 };

#endif // CONFIGURATION_H
