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
    conf.eventsServerTableName = eventsServerJson.at("eventsTable");
    conf.eventsServerReconnectTimeout = eventsServerJson.at("reconnectTimeout");
    conf.eventsServerSurveyInterval = eventsServerJson.at("surveyInterval");
    conf.videoRootPath = eventsServerJson.at("videoRootPath");

    json platformJson = confJson.at("platform");
    conf.platformServerIP = platformJson.at("serverIP");
    conf.platformServerPort = platformJson.at("serverPort");
    conf.platformHeartBeatTimeout = platformJson.at("heartbeatTimeout");
    conf.platformReconnectTimeout = platformJson.at("reconnectTimeout");
    json videoServerJson = platformJson.at("videoserver");
    conf.videoServerConnectionType = videoServerJson.at("transport");
    conf.rtspLink = videoServerJson.at("rtsp");

    json localServerInfoJson = confJson.at("localServer");
    conf.localServerHost = localServerInfoJson.at("host");
    conf.localServerPort = localServerInfoJson.at("port");
    conf.localServerConnectionsCount = localServerInfoJson.at("connectionsAllowed");

    return conf;
}
