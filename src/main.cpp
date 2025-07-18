#include <iostream>

#include "easylogging++.h"
#include "configurationparser.h"
#include "confmodificationwatcher.h"
#include "module.h"
#include "TerminalInfo.h"
#include "PlatformInfo.h"
#include "eventserverinfo.h"
#include "eventrequests.h"
#include "tools.h"
#include "tests/jt808serializertest.h"
#include "tests/jt808requeststest.h"
#include "jt808client.h"
#include <csignal>
#include <systemd/sd-daemon.h>

#include "realtimevideostreamer.h"

//#define REQUESTTEST

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
    configuration.printInfo();

    TerminalInfo terminalInfo;
    terminalInfo.phoneNumber = configuration.terminalPhoneNumber;
    terminalInfo.provinceID = configuration.provinceID;
    terminalInfo.cityID = configuration.cityID;
    terminalInfo.manufacturerID = configuration.manufacturerID;
    terminalInfo.terminalModel = configuration.terminalModel;
    terminalInfo.terminalID = configuration.terminalID;
    terminalInfo.licencePlateColor = configuration.licencePlateColor;
    terminalInfo.vin = configuration.vin;

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
    eventServerInfo.eventsTableName = configuration.eventsServerTableName;
    eventServerInfo.reconnectTimeout = configuration.eventsServerReconnectTimeout;
    eventServerInfo.surveyInterval = configuration.eventsServerSurveyInterval;
    eventServerInfo.videoRootPath = configuration.videoRootPath;

    LocalServerInfo localServerInfo;
    localServerInfo.host = configuration.localServerHost;
    localServerInfo.port = configuration.localServerPort;
    localServerInfo.connectionsCount = configuration.localServerConnectionsCount;
    terminalInfo.localServerInfo = localServerInfo;

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
}

void setLogger()
{
    el::Configurations config("../logger/logger.conf");
    if(!config.parseFromFile("../logger/logger.conf")) {
        std::cerr << "Ошибка парсинга файла логгера" << std::endl;
    }
    el::Loggers::reconfigureLogger("default", config);
    el::Loggers::flushAll();
}

INITIALIZE_EASYLOGGINGPP

int main(int argc, char **argv)
{
    signal(SIGINT, signalHandler);


#ifdef REQUESTTEST
    uint8_t byte = 0;
    tools::setBit(byte, 7);
    tools::setBit(byte, 0);

    std::cout << static_cast<int>(byte) << std::endl;

    return 0;
#endif

    //--------Logger settings--------------------
    setLogger();

    //-------- Configuration settings----------
    std::string pathToConf = "";
    if(argc != 2) {
        std::cerr << "Ошибочный запуск программы!!!\nmtp-808-proxy absolute_path_to_config" << std::endl;
        return 0;
    } else {
        pathToConf = std::string(argv[1]);
    }

    FullConfiguration fullConf = getFullConfiguration(pathToConf);

    //---------Dispatcher module------------------
    Module module(fullConf.terminalInfo, fullConf.platformInfo, fullConf.eventServerInfo);

    //---------Configuration changes watcher------
    ConfModificationWatcher confWatcher(pathToConf);
    std::thread confWatcherThread([&confWatcher, &module, &pathToConf](){
        while(isRunning) {
            if(confWatcher.checkIfFileWasModified()) {
                FullConfiguration fullConf = getFullConfiguration(pathToConf);
                module.setTerminalInfo(fullConf.terminalInfo);
                module.setPlatformInfo(fullConf.platformInfo);
                module.setEventServerInfo(fullConf.eventServerInfo);
            }
            this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });

//    std::thread watchdogNotofierThread(sdNotify, 10000);

    while(isRunning) {
    }


    confWatcherThread.join();
//    watchdogNotofierThread.join();

    return 0;
}
