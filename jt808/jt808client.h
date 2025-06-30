#ifndef JT808CONNECTIONHANDLER_H
#define JT808CONNECTIONHANDLER_H

#include "TerminalInfo.h"
#include "PlatformInfo.h"
#include "terminalparams.h"

#include <vector>
#include <thread>
#include <stdexcept>

class JT808ConnectionErrorException : public std::runtime_error
{
public:
    JT808ConnectionErrorException(const std::string errMessage);
};

class JT808Client
{
public:
    JT808Client();
    JT808Client(const TerminalInfo &tInfo, const PlatformInfo &pInfo);
    ~JT808Client();

    void connectToPlatform();

    void setConfiguration(const TerminalInfo &tInfo, const PlatformInfo &pInfo);
    void setTerminalInfo(TerminalInfo info);
    void setPlatformInfo(PlatformInfo info);
    void setTerminalParameters();

    void sendAlarmMessage(const std::vector<uint8_t> &request, const std::vector<uint8_t> &alarmBody);

private:
    bool checkIfAuthenticationKeyExists();

    bool connectToHost();
    void reconnectToHost();
    bool connectDomain();
    bool connectIp();

    void sendVideoFile(const std::string &filePath, const std::vector<uint8_t> &alarmBody);

    void sendRegistrationRequest();
    void parseRegistrationAnswer(std::vector<uint8_t> answer);
    void writeAuthenticationKeyToFile(const std::string &path);

    void sendAuthenticationRequest();
    void sendHeartBeatRequest();
    void sendTerminalParametersRequest();

    void startPlatformAnswerHandler();
    void handlePlatformAnswer(const std::vector<uint8_t> &answer);
    bool parseGeneralResponse(const std::vector<uint8_t> &response);

    bool isIPAddress(const std::string &socketAddr);

private:
    TerminalInfo terminalInfo;
    PlatformInfo platformInfo;
    TerminalParameters terminalParams;

    int socketFd;
    bool isConnected = false;

    bool isFileUploadingInProgress = false;

    bool isAuthenticationKeyExists = false;
    std::vector<uint8_t> authenticationKey;

    std::vector<uint8_t> currentAlarmBody;

    std::thread heartBeatThread;
    std::thread platformAnswerHandlerThread;
};

#endif // JT808CONNECTIONHANDLER_H
