#ifndef JT808SERIALIZER_H
#define JT808SERIALIZER_H

/*
 * Класс предназначен для конвертации сообщений от регистратора событий в битовый поток по формату JT/T 808
 */

#include <vector>
#include <cstdint>

#include "nlohmann/json.hpp"

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

struct TerminalStatus
{
    bool isACCOn = true;
    bool isPositioned = true;
    bool isSouthLatitude = false;
    bool isWestLongitude = false;
    bool isRunningStatus = false;
    bool isCoordinatesEncrypted = false;
    int loadLevel = 0;
    bool vehicleOilCurcuit = false;
    bool vehicleCurcuit = false;
    bool isDoorLocked = false;
    bool isDoor1Opened = false;
    bool isDoor2Opened = false;
    bool isDoor3Opened = false;
    bool isDoor4Opened = false;
    bool isDoor5Opened = false;
    bool isGPSUsing = true;
    bool isBeidouUsing = false;
    bool isGlonasUsing = false;
    bool isGalileoUsing = false;
};

class JT808EventSerializer
{
public:
    JT808EventSerializer();
    ~JT808EventSerializer();

    void setTerminalPhoneNumber(const std::string &phone);
    void setTerminalID(const std::string &id);

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
    void setStateFlag();
    void fillStateFlag();
    
    void setEventData();
    void fillEventDada();

    void addAdditionalInformation();
    const uint16_t getVehicleStateStatus();
    const std::vector<uint8_t> getAlarmID();

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

    TerminalStatus terminalStatus;
    uint32_t stateFlag = 0x00000000;

    int32_t latitude = 0;
    int32_t longitude = 0;
    uint16_t elevation = 0;
    uint16_t speed = 0;
    uint16_t direction = 0;
    BCDTime time;

    const uint8_t startStopByte = 0x7E;
    const uint16_t alarmMessageID = 0x0200;

    const std::unordered_map<uint8_t, uint32_t> alarmMaps = {
      {0, 0x00000000}, {1, 0x00000000}, {2, 0x00000011}, {3, 0x00000000},
      {4, 0x00000000}, {5, 0x00000000}, {6, 0x00000000}, {7, 0x00000000},
      {8, 0x00000000}, {9, 0x0F030020}, {10, 0x00000000}, {11, 0x00000000},
      {12, 0x00000000}, {13, 0x00000000}, {14, 0x00000000}, {15, 0x00000000},
      {16, 0x00000000}, {17, 0x000000FF}, {18, 0x00000000}, {19, 0x00000000},
      {20, 0x00000000}, {21, 0x00000000}, {22, 0x00000000}, {23, 0x00000000},
      {24, 0x00000000}, {25, 0x00000000}, {26, 0x00000000}, {27, 0x00000000},
      {28, 0x00000000}, {29, 0x00000000}, {30, 0x00000000}, {31, 0x00000000}
    };

    uint8_t alarmTypeID = 0x65;
    uint8_t alarmType = 0x05;
    uint8_t alarmSerialNum = 0;

    const std::vector<uint8_t> replacers7E = {0x7d, 0x02};
    const std::vector<uint8_t> replacers7D = {0x7d, 0x01};

    std::string terminalPhoneNumber;
    std::string terminalID = "";

    bool isPacketsIncapsulated = false;
};

#endif // JT808SERIALIZER_H
