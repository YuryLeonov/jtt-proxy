#include "configurationparser.h"

#include <fstream>
#include <iostream>

#include "easylogging++.h"

ConfigurationParser::ConfigurationParser(const std::string &path) : filePath(path)
{

}

void ConfigurationParser::initConfJson()
{
    std::ifstream stream(filePath, std::ifstream::in);

    if(!stream.is_open()) {
        const std::string errorMessage = "Ошибка открытия конфигурационного файла " + filePath;
        throw ConfFileOpenErrorException(errorMessage.c_str());
    }

    LOG(INFO) << "Файл конфигурации открыт: " << filePath;

    stream >> confJson;

    stream.close();
}

Configuration ConfigurationParser::parseConfiguration()
{
    Configuration conf;

    json terminalInfoJson = confJson.at("terminal");
    conf.terminalPhoneNumber = terminalInfoJson.at("phoneNumber");
    conf.provinceID = terminalInfoJson.at("provinceID");
    conf.cityID = terminalInfoJson.at("cityID");
    conf.manufacturerID = terminalInfoJson.at("manufacturerID");
    conf.terminalModel = terminalInfoJson.at("terminalModel");
    conf.terminalID = terminalInfoJson.at("terminalID");
    conf.licencePlateColor = terminalInfoJson.at("licensePlateColor");
    conf.vin = terminalInfoJson.at("VIN");

    json eventsServerJson = confJson.at("eventsServer");
    conf.eventsServerIP = eventsServerJson.at("serverIP");
    conf.eventsServerPort = eventsServerJson.at("serverPort");
    conf.eventsServerReconnectTimeout = eventsServerJson.at("reconnectTimeout");
    conf.eventsServerSurveyInterval = eventsServerJson.at("surveyInterval");

    json platformJson = confJson.at("platform");
    conf.platformServerIP = platformJson.at("serverIP");
    conf.platformServerPort = platformJson.at("serverPort");
    conf.platformHeartBeatTimeout = platformJson.at("heartbeatTimeout");
    conf.platformReconnectTimeout = platformJson.at("reconnectTimeout");
    json videoServerJson = platformJson.at("videoserver");
    conf.videoServerConnectionType = videoServerJson.at("transport");
    for(auto &el : videoServerJson["rtsp"]) {
        conf.rtspLinks.push_back(el);
    }

    json statusFlagJson = confJson.at("statusFlag");
    conf.acc = statusFlagJson.at("ACC");
    conf.isPositioned = statusFlagJson.at("Positioned");
    conf.latitude = statusFlagJson.at("latitude");
    conf.longitude = statusFlagJson.at("longitude");
    conf.operationStatus = statusFlagJson.at("OperationStatus");
    conf.isCoordinatesEncrepted = statusFlagJson.at("CoordinatesEncryption");
    conf.loadLevel = statusFlagJson.at("LoadLevel");
    conf.vehicleOilCircuitStatus = statusFlagJson.at("VehicleOilCircuitStatus");
    conf.vehicleCircuitStatus = statusFlagJson.at("VehicleCircuitStatus");
    conf.doorStatus = statusFlagJson.at("DoorStatus");
    conf.isFrontDoorOpened = statusFlagJson.at("IsFrontDoorOpened");
    conf.isMiddleDoorOpened = statusFlagJson.at("IsMiddleDoorOpened");
    conf.isBackDoorOpened = statusFlagJson.at("IsBackDoorOpened");
    conf.isDriverDoorOpened = statusFlagJson.at("IsDriverDoorOpened");
    conf.isFifthDoorOpened = statusFlagJson.at("IsFifthDoorOpened");
    conf.isGPSUsing = statusFlagJson.at("GPSStatus");
    conf.isBeidouUsing = statusFlagJson.at("BeidouStatus");
    conf.isGlonassUsing = statusFlagJson.at("GlonassStatus");
    conf.isGalileoUsing = statusFlagJson.at("GalileoStatus");
    conf.satellitesCount = statusFlagJson.at("satellitesCount");
    conf.alarmVideosCount = statusFlagJson.at("alarmVideosCount");

    return conf;
}
