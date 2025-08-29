#ifndef JT808SERIALIZER_H
#define JT808SERIALIZER_H

/*
 * Класс предназначен для конвертации сообщений от регистратора событий в битовый поток по формату JT/T 808
 */

#include <vector>
#include <cstdint>

#include "nlohmann/json.hpp"
#include "TerminalInfo.h"

using json = nlohmann::json;

struct BCDTime {
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    void setDateAndTimeFromOrdinary(uint8_t y, uint8_t mth, uint8_t d, uint8_t h, uint8_t min, uint8_t s);
};

class JT808EventSerializer
{
public:

    enum LocationInfoStatus {
        Basic = 0,
        Alarm
    };

    JT808EventSerializer();
    ~JT808EventSerializer();

    void setTerminalInfo(const TerminalInfo &info);
    void setLocationInfoStatus(LocationInfoStatus s);
    const std::vector<uint8_t> getAlarmID() const;
    const std::string getAlarmTime() const;
    uint8_t getAlarmType() const;

    std::vector<uint8_t> serializeToBitStream(const std::string &message, uint8_t alarmSerNum);
    std::vector<uint8_t> serializeToBitStream(const json &j);

    const std::vector<uint8_t> getBodyStream() const;
    const std::vector<uint8_t> getAddInfoStream() const;

private:
    void parseEventMessage(const std::string &message);

    void addStartByte();
    void addStopByte();
    
    void setAlarmFlag();
    void fillAlarmFlag();
    
    void setTerminalStatus();
    void setStatusFlag();
    void fillStateFlag();
    
    void setLocationData();
    void fillEventDada();

    void addAdditionalInformation();
    void addSatellitesCountInfo();

    void composeAlarmID();
    const uint16_t getVehicleStateStatus();

    void setHeader();
    void setBodyInfo(uint16_t &info);
    void setCheckSum();

private:
    std::string eventMessage;
    json eventJson;

    std::vector<uint8_t> messageStream;
    std::vector<uint8_t> headerStream;
    std::vector<uint8_t> bodyStream;
    std::vector<uint8_t> addInfoStream;
    uint8_t checkSum;
    uint16_t messageSerialNum = 0;

    uint32_t alarmFlag = 0x00000000;
    uint32_t stateFlag = 0x00000000;

    TerminalInfo terminalInfo;
    TerminalStatus terminalStatus;

    int32_t latitude = 0;
    int32_t longitude = 0;
    uint16_t elevation = 0;
    uint16_t speed = 0;
    uint16_t direction = 0;
    BCDTime time;
    std::string timestamp = "";

    const uint8_t startStopByte = 0x7E;
    const uint16_t alarmMessageID = 0x0200;

    uint8_t alarmTypeID = 0x65;
    uint8_t alarmType = 0x05;
    uint8_t alarmSerialNum = 0;
    std::vector<uint8_t> alarmID;

    const std::vector<uint8_t> replacers7E = {0x7d, 0x02};
    const std::vector<uint8_t> replacers7D = {0x7d, 0x01};

    bool isPacketsIncapsulated = false;

    LocationInfoStatus locationInfoStatus = LocationInfoStatus::Basic;
};

#endif // JT808SERIALIZER_H
