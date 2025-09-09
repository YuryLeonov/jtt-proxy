#include <iostream>
#include <cstdlib>

#include "configurationparser.h"
#include "confmodificationwatcher.h"
#include "module.h"
#include "TerminalInfo.h"
#include "PlatformInfo.h"
#include <csignal>
#include <systemd/sd-daemon.h>
#include "logger.h"

const std::string VERSION = "1.1";

struct FullConfiguration
{
    TerminalInfo terminalInfo;
    platform::PlatformInfo platformInfo;
    EventServerInfo eventServerInfo;
};

const FullConfiguration getFullConfiguration(const std::string &confFilePath)
{
    FullConfiguration fullConf;

    ConfigurationParser confParser(confFilePath);

    try {
        confParser.initConfJson();
    } catch(ConfFileOpenErrorException &exception) {
        LOG(ERROR) << exception.what() << std::endl;
        return fullConf;
    }

    const Configuration configuration = confParser.parseConfiguration();

    TerminalInfo terminalInfo;
    terminalInfo.phoneNumber = configuration.terminalPhoneNumber;
    terminalInfo.provinceID = configuration.provinceID;
    terminalInfo.cityID = configuration.cityID;
    terminalInfo.manufacturerID = configuration.manufacturerID;
    terminalInfo.terminalModel = configuration.terminalModel;
    terminalInfo.terminalID = configuration.terminalID;
    terminalInfo.licencePlateColor = configuration.licencePlateColor;
    terminalInfo.vin = configuration.vin;

    TerminalStatus terminalStatus;
    terminalStatus.isACCOn = (configuration.acc == "on") ? true : false;
    terminalStatus.isPositioned = configuration.isPositioned;
    terminalStatus.isSouthLatitude = (configuration.latitude == "south") ? true : false;
    terminalStatus.isWestLongitude = (configuration.longitude == "west") ? true : false;
    terminalStatus.isRunningStatus = (configuration.operationStatus == "on") ? true : false;
    terminalStatus.isCoordinatesEncrypted = configuration.isCoordinatesEncrepted;
    terminalStatus.loadLevel = configuration.loadLevel;
    terminalStatus.vehicleOilCurcuit = (configuration.vehicleOilCircuitStatus == "on") ? true : false;
    terminalStatus.vehicleCurcuit = (configuration.vehicleCircuitStatus == "on") ? true : false;
    terminalStatus.isDoorLocked = (configuration.doorStatus == "locked") ? true : false;
    terminalStatus.isFrontDoorOpened = configuration.isFrontDoorOpened;
    terminalStatus.isMiddleDoorOpened = configuration.isMiddleDoorOpened;
    terminalStatus.isBackDoorOpened = configuration.isBackDoorOpened;
    terminalStatus.isDriverDoorOpened = configuration.isDriverDoorOpened;
    terminalStatus.isFifthDoorOpened = configuration.isFifthDoorOpened;
    terminalStatus.isGPSUsing = configuration.isGPSUsing;
    terminalStatus.isBeidouUsing = configuration.isBeidouUsing;
    terminalStatus.isGlonassUsing = configuration.isGlonassUsing;
    terminalStatus.isGalileoUsing = configuration.isGalileoUsing;
    terminalStatus.satellitesCount = configuration.satellitesCount;
    terminalStatus.alarmVideosCount = configuration.alarmVideosCount;
    terminalStatus.alarmVideosWaitInterval = configuration.alarmVideosWaitInterval;
    terminalInfo.status = terminalStatus;

    platform::PlatformInfo platformInfo;
    platformInfo.ipAddress = configuration.platformServerIP;
    platformInfo.port = configuration.platformServerPort;
    platformInfo.heartBeatTimeout = configuration.platformHeartBeatTimeout;
    platformInfo.reconnectTimeout = configuration.platformReconnectTimeout;
    platform::VideoServer videoServer;
    videoServer.rtspLinks = std::move(configuration.rtspLinks);
    if(configuration.videoServerConnectionType == "tcp")
        videoServer.connType = platform::ConnectionType::TCP;
    else
        videoServer.connType = platform::ConnectionType::UDP;
    platformInfo.videoServer = videoServer;

    EventServerInfo eventServerInfo;
    eventServerInfo.ipAddress = configuration.eventsServerIP;
    eventServerInfo.port = configuration.eventsServerPort;
    eventServerInfo.reconnectTimeout = configuration.eventsServerReconnectTimeout;
    eventServerInfo.surveyInterval = configuration.eventsServerSurveyInterval;

    fullConf.terminalInfo = terminalInfo;
    fullConf.platformInfo = platformInfo;
    fullConf.eventServerInfo = eventServerInfo;

    return fullConf;
}

using namespace std;

bool isRunning = true;

void sdNotify(int timeout)
{
    sd_notify(0, "READY=1");

    while(isRunning) {
        std::cout << "Отправка пинга в вотчдог" << std::endl;
        sd_notify(0, "WATCHDOG=1");
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    }
}


void signalHandler(int sigNum) {
    std::cout << "Interrupt signal: " << sigNum << " recieved..." << std::endl;

    isRunning = false;
    exit(0);
}

INITIALIZE_EASYLOGGINGPP
int main(int argc, char **argv)
{
    signal(SIGINT, signalHandler);

    //--------Logger settings--------------------
    logger::setLogger();

    //-------- Configuration settings----------
    std::string pathToConf = "";
    if(argc != 2) {
        std::cerr << "Ошибочный запуск программы!!!\nmtp-808-proxy absolute_path_to_config" << std::endl;
        return 0;
    } else {
        pathToConf = std::string(argv[1]);
        if(pathToConf == "-v") {
            std::cout << "Версия: " << VERSION << std::endl;
            return 0;
        }
    }

    FullConfiguration fullConf = getFullConfiguration(pathToConf);

    //---------Dispatcher module------------------
    Module module(fullConf.terminalInfo, fullConf.platformInfo, fullConf.eventServerInfo);

    //---------Configuration changes watcher------
//    ConfModificationWatcher confWatcher(pathToConf);
//    std::thread confWatcherThread([&confWatcher, &module, &pathToConf](){
//        while(isRunning) {
//            if(confWatcher.checkIfFileWasModified()) {
//                FullConfiguration fullConf = getFullConfiguration(pathToConf);
//                module.setTerminalInfo(fullConf.terminalInfo);
//                module.setPlatformInfo(fullConf.platformInfo);
//                module.setEventServerInfo(fullConf.eventServerInfo);
//            }
//            this_thread::sleep_for(std::chrono::milliseconds(1000));
//        }
//    });

//    std::thread watchdogNotofierThread(sdNotify, 10000);

    std::thread logsUpdateThread([](){

        while(isRunning) {
            logger::checkAndUpdateLogFile();
            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    });
    logsUpdateThread.detach();

    while(isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }


//    confWatcherThread.join();
//    watchdogNotofierThread.join();

    return 0;
}
