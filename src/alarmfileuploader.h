#ifndef ALARMFILEUPLOADER_H
#define ALARMFILEUPLOADER_H

#include "TerminalInfo.h"
#include <iostream>
#include <vector>

class AlarmFileUploader
{
public:
    AlarmFileUploader(const std::string &host, int port, const TerminalInfo &info);
    ~AlarmFileUploader();

    void setJTAlarmTyoe(uint8_t type);
    void setPathToVideo(const std::string &path);
    void setAlarmID(const std::vector<uint8_t> &id);
    void setAlarmNumber(const std::vector<uint8_t> &number);
    void setAttachments(int ats);
    void setAlarmType(uint8_t type);

    bool connectToStorage();
    bool uploadFile();

private:
    bool sendAlarmAttachmentMessageToStorage();
    bool initUploading();
    bool upload();

    bool parseGeneralResponse(const std::vector<uint8_t> &response);
    void parse9212Answer(const std::vector<uint8_t> &answer);

private:
    std::string storageHost = "";
    int storagePort = 0;

    TerminalInfo terminalInfo;

    int socketId = -1;
    bool isConnected = false;

    uint8_t jtAlarmType = 0x10;
    std::string pathToVideo = "";
    std::vector<uint8_t> alarmID;
    std::vector<uint8_t> alarmNumber;
    int attachments = 0;
    uint8_t alarmType = 0x65;
};

#endif // ALARMFILEUPLOADER_H
