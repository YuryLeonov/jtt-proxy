#ifndef JT808CONNECTIONHANDLER_H
#define JT808CONNECTIONHANDLER_H

#include "TerminalInfo.h"
#include "PlatformInfo.h"
#include "terminalparams.h"
#include "realtimevideostreamer.h"

#include <thread>
#include <stdexcept>
#include <memory>
#include <map>


class JT808ConnectionErrorException : public std::runtime_error
{
public:
    JT808ConnectionErrorException(const std::string errMessage);
};

class JT808Client
{
public:
    JT808Client();
    JT808Client(const TerminalInfo &tInfo, const platform::PlatformInfo &pInfo);
    ~JT808Client();

    void connectToPlatform();

    void setConfiguration(const TerminalInfo &tInfo, const platform::PlatformInfo &pInfo);
    void setTerminalInfo(TerminalInfo info);
    void setPlatformInfo(platform::PlatformInfo info);
    void setTerminalParameters();

    bool sendAlarmMessage(const std::vector<uint8_t> &request, const std::vector<uint8_t> &addInfo);
    void sendAlarmVideoFile(const std::string &filePath, const std::vector<uint8_t> &alarmBody);

private:
    bool checkIfAuthenticationKeyExists();

    bool connectToHost();
    void reconnectToHost();
    bool connectDomain();
    bool connectIp();

    bool connectToStorageServer(const std::string host, int port);

    void sendVideoFile(const std::string &filePath, const std::vector<uint8_t> &alarmBody);

    void sendRegistrationRequest();
    void parseRegistrationAnswer(std::vector<uint8_t> answer);
    void writeAuthenticationKeyToFile(const std::string &path);

    void sendGeneralResponseToPlatform(uint16_t serialNum, uint16_t messageID);

    void sendAuthenticationRequest();
    void sendHeartBeatRequest();
    void sendTerminalParametersRequest();

    void sendAlarmAttachmentMessageToStorage(const std::vector<uint8_t> &alarmID, const std::vector<uint8_t> &alarmNumber, int attachmentsNumber);

    void startPlatformAnswerHandler();
    void handlePlatformAnswer(const std::vector<uint8_t> &answer);

    bool parseGeneralResponse(const std::vector<uint8_t> &response);
    bool parseRealTimeVideoRequest(const std::vector<uint8_t> &request);
    bool parseRealTimeVideoControlRequest(const std::vector<uint8_t> &request);
    bool parseRealTimeVideoStatusRequest(const std::vector<uint8_t> &request);

    bool parseArchiveListRequest(const std::vector<uint8_t> &request);
    bool parseVideoPlaybackRequest(const std::vector<uint8_t> &request);
    bool parseVideoPlaybackControlRequest(const std::vector<uint8_t> &request);
    bool parseAlarmAttachmentUploadRequest(const std::vector<uint8_t> &request);

    void streamVideo(const streamer::VideoServerRequisites &vsRequisites, const std::vector<uint8_t> &request);

    bool isIPAddress(const std::string &socketAddr);

private:
    TerminalInfo terminalInfo;
    platform::PlatformInfo platformInfo;
    TerminalParameters terminalParams;

    int socketFd;
    bool isConnected = false;

    int storageSocketId;

    bool isFileUploadingInProgress = false;
    uint32_t multimediaID = 0x000011001;

    bool isAuthenticationKeyExists = false;
    std::vector<uint8_t> authenticationKey;

    std::vector<uint8_t> currentAlarmBody;
    std::vector<uint8_t> currentAddInfo;

    std::thread heartBeatThread;
    std::thread platformAnswerHandlerThread;

    std::mutex sendingMessageMutex;

    std::map<int, std::unique_ptr<streamer::RealTimeVideoStreamer>> videoStreamers;

};

#endif // JT808CONNECTIONHANDLER_H
