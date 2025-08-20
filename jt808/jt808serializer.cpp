#include "jt808serializer.h"
#include "alarmtypes.h"
#include "easylogging++.h"

#include "tools.h"
#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>

JT808EventSerializer::JT808EventSerializer()
{

}

JT808EventSerializer::~JT808EventSerializer()
{

}

void JT808EventSerializer::setTerminalPhoneNumber(const std::string &phone)
{
    terminalPhoneNumber = phone;
}

void JT808EventSerializer::setTerminalID(const std::string &id)
{
    terminalID = id;
}

std::vector<uint8_t> JT808EventSerializer::serializeToBitStream(const std::string &message, uint8_t alarmSerNum)
{
    alarmSerialNum = alarmSerNum;
    messageStream.clear();
    json data = json::parse(message);

    return serializeToBitStream(data);

}

std::vector<uint8_t> JT808EventSerializer::serializeToBitStream(const json &j)
{
    eventJson = j;

    messageStream.clear();

    try {
        setEventData();
        setTerminalStatus();
        setAlarmFlag();
        setStateFlag();
    } catch(const nlohmann::detail::out_of_range &exc) {
        LOG(ERROR) << "Ошибка обработки события: " << exc.what();
        return messageStream;
    }

    fillAlarmFlag();
    fillStateFlag();
    fillEventDada();

    addAdditionalInformation();

    setHeader();

    messageStream.insert(messageStream.end(), headerStream.begin(), headerStream.end());
    messageStream.insert(messageStream.end(), bodyStream.begin(), bodyStream.end());
    tools::replaceByteInVectorWithTwo(messageStream, 0x7d, 0x7d, 0x01);
    tools::replaceByteInVectorWithTwo(messageStream, 0x7e, 0x7d, 0x02);
    setCheckSum();

    addStartByte();
    addStopByte();

    return messageStream;
}

const std::vector<uint8_t> JT808EventSerializer::getBodyStream() const
{
    return bodyStream;
}

const std::vector<uint8_t> JT808EventSerializer::getAddInfoStream() const
{
    return addInfoStream;
}

void JT808EventSerializer::parseEventMessage(const std::string &message)
{

}

void JT808EventSerializer::addStartByte()
{
    messageStream.insert(messageStream.begin(), startStopByte);
}

void JT808EventSerializer::addStopByte()
{
    messageStream.insert(messageStream.end(), startStopByte);
}

void JT808EventSerializer::  setAlarmFlag()
{
        alarmFlag = 0;

        const int eventID = eventJson.at("event_type");
        alarmType = alarms::dsmAlarmsMap[eventID];

        switch(eventID) {
            case 1 :
                tools::setBit(alarmFlag, 3);
                alarmTypeID = 0x65;
                break;
            case 2 :
                tools::setBit(alarmFlag, 14);
                alarmTypeID = 0x65;
                break;
            case 3 :
                tools::setBit(alarmFlag, 3);
                alarmTypeID = 0x65;
                break;
            case 4 :
                tools::setBit(alarmFlag, 3);
                alarmTypeID = 0x65;
                break;
            case 5 :
                tools::setBit(alarmFlag, 2);
                alarmTypeID = 0x65;
                break;
            case 6 :
                tools::setBit(alarmFlag, 3);
                alarmTypeID = 0x65;
                break;
            case 7 :
                tools::setBit(alarmFlag, 3);
                alarmTypeID = 0x65;
                break;
            case 8 :
                tools::setBit(alarmFlag, 3);
                alarmTypeID = 0x65;
                break;
            case 9 :
                tools::setBit(alarmFlag, 3);
                alarmTypeID = 0x65;
                break;
            case 10 :
                tools::setBit(alarmFlag, 3);
                tools::setBit(alarmFlag, 2);
                tools::setBit(alarmFlag, 14);
                alarmTypeID = 0x65;
                break;
            case 14 :
                tools::setBit(alarmFlag, 14);
                alarmTypeID = 0x65;
                break;
            case 15 :
                tools::setBit(alarmFlag, 14);
                alarmTypeID = 0x65;
                break;
            case 16 :
                tools::setBit(alarmFlag, 14);
                alarmTypeID = 0x65;
                break;
            case 17 :
                tools::setBit(alarmFlag, 3);
                tools::setBit(alarmFlag, 2);
                tools::setBit(alarmFlag, 29);
                tools::setBit(alarmFlag, 30);
                alarmTypeID = 0x65;
                break;
            case 21 :
                tools::setBit(alarmFlag, 3);
                tools::setBit(alarmFlag, 29);
                alarmTypeID = 0x64;
                break;
            case 22 :
                tools::setBit(alarmFlag, 3);
                tools::setBit(alarmFlag, 29);
                alarmTypeID = 0x64;
                break;
            case 23 :
                tools::setBit(alarmFlag, 3);
                tools::setBit(alarmFlag, 29);
                alarmTypeID = 0x64;
                break;
            case 24 :
                tools::setBit(alarmFlag, 3);
                alarmTypeID = 0x64;
                break;
            case 30 :
                tools::setBit(alarmFlag, 11);
                alarmTypeID = 0x65;
                break;
            case 31 :
                tools::setBit(alarmFlag, 11);
                alarmTypeID = 0x64;
                break;
            case 95 :
                tools::setBit(alarmFlag, 29);
                alarmTypeID = 0x64;
                break;
        }
}

void JT808EventSerializer::fillAlarmFlag()
{
    tools::addToStdVector(bodyStream, alarmFlag);
}

void JT808EventSerializer::setTerminalStatus()
{
    if(latitude < 0)
        terminalStatus.isSouthLatitude = true;
    else
        terminalStatus.isSouthLatitude = false;

    if(longitude < 0)
        terminalStatus.isWestLongitude = true;
    else
        terminalStatus.isWestLongitude = false;

    terminalStatus.isPositioned = true;
    terminalStatus.isGPSUsing = true;
}

void JT808EventSerializer::setStateFlag()
{
    stateFlag = 0;

    if(terminalStatus.isACCOn)
        tools::setBit(stateFlag, 0);
    if(terminalStatus.isPositioned)
        tools::setBit(stateFlag, 1);
    if(terminalStatus.isSouthLatitude)
        tools::setBit(stateFlag, 2);
    if(terminalStatus.isWestLongitude)
        tools::setBit(stateFlag, 3);
    if(!terminalStatus.isRunningStatus)
        tools::setBit(stateFlag, 4);
    if(terminalStatus.isCoordinatesEncrypted)
        tools::setBit(stateFlag, 5);


    if(terminalStatus.loadLevel == 1) {
        tools::setBit(stateFlag, 9);
    } else if(terminalStatus.loadLevel == 2) {
        tools::setBit(stateFlag, 8);
    } else if(terminalStatus.loadLevel == 3) {
        tools::setBit(stateFlag, 8);
        tools::setBit(stateFlag, 9);
    }

    if(!terminalStatus.vehicleOilCurcuit)
        tools::setBit(stateFlag, 10);
    if(!terminalStatus.vehicleCurcuit)
        tools::setBit(stateFlag, 11);
    if(terminalStatus.isDoorLocked)
        tools::setBit(stateFlag, 12);
    if(terminalStatus.isDoor1Opened)
        tools::setBit(stateFlag, 13);
    if(terminalStatus.isDoor2Opened)
        tools::setBit(stateFlag, 14);
    if(terminalStatus.isDoor3Opened)
        tools::setBit(stateFlag, 15);
    if(terminalStatus.isDoor4Opened)
        tools::setBit(stateFlag, 16);
    if(terminalStatus.isDoor5Opened)
        tools::setBit(stateFlag, 17);
    if(terminalStatus.isGPSUsing)
        tools::setBit(stateFlag, 18);
    if(terminalStatus.isBeidouUsing)
        tools::setBit(stateFlag, 19);
    if(terminalStatus.isGlonasUsing)
        tools::setBit(stateFlag, 20);
    if(terminalStatus.isGalileoUsing)
        tools::setBit(stateFlag, 21);

}

void JT808EventSerializer::fillStateFlag()
{
    tools::addToStdVector(bodyStream, stateFlag);
}

void JT808EventSerializer::setEventData()
{
    //Coordinates
    std::string gps = "";
    if(eventJson.contains("gps")) {
        gps = eventJson.at("gps");
    } else {
        gps = "55.760626, 37.703999";
    }
    std::vector<std::string> coordinates = tools::split(gps, ',');
    try {
        latitude = static_cast<int32_t>(std::stod(coordinates.at(0))*1000000);
        longitude = static_cast<int32_t>(std::stod(coordinates.at(1))*1000000);
    } catch(const std::invalid_argument &e) {
        std::cerr << "Ошибка аргумента метода stod при преобразовании координат из строки: " << e.what();
    } catch(const std::out_of_range &e) {
        std::cerr << "Ошибка аргумента метода stod при преобразовании координат из строки: " << e.what();
    }

    elevation = 10;
    direction = 100;

    //speed
    const int8_t s = 0;
    if(eventJson.contains("speed")) {
        const int8_t s = eventJson.at("speed");
        if(s >= 0) {
            speed = static_cast<uint16_t>(s);
        }
    } else {
        speed = 50;
    }

    speed = 50;

    //Time
    std::string timestamp = "";
    if(eventJson.contains("timestamp")) {
        timestamp = eventJson.at("timestamp");
    } else {
        timestamp = "2025-08-15 16:33:10";
    }

    std::vector<std::string> splittedTimestamp = tools::split(timestamp, ' ');
    const std::string dateStr = splittedTimestamp.at(0);
    const std::string timeStr = splittedTimestamp.at(1);
    std::vector<std::string> splittedDate = tools::split(dateStr, '-');
    std::vector<std::string> splittedTime = tools::split(timeStr, ':');
    const std::string yearStr = splittedDate.at(0);
    const std::string monthStr = splittedDate.at(1);
    const std::string dayStr = splittedDate.at(2);
    const std::string hourStr = splittedTime.at(0);
    const std::string minuteStr = splittedTime.at(1);
    const std::string secondStr = splittedTime.at(2);
    time.setDateAndTimeFromOrdinary(std::stoi(yearStr.substr(yearStr.size() - 2)),
                                    std::stoi(monthStr),
                                    std::stoi(dayStr),
                                    std::stoi(hourStr),
                                    std::stoi(minuteStr),
                                    std::stoi(secondStr));

}

void JT808EventSerializer::fillEventDada()
{
    tools::addToStdVector(bodyStream, latitude);
    tools::addToStdVector(bodyStream, longitude);
    tools::addToStdVector(bodyStream, elevation);
    tools::addToStdVector(bodyStream, speed);
    tools::addToStdVector(bodyStream, direction);
    bodyStream.push_back(time.year);
    bodyStream.push_back(time.month);
    bodyStream.push_back(time.day);
    bodyStream.push_back(time.hour);
    bodyStream.push_back(time.minute);
    bodyStream.push_back(time.second);
}

void JT808EventSerializer::addAdditionalInformation()
{
    addInfoStream.clear();

    const uint32_t policeID = 0x00000000;
    tools::addToStdVector(addInfoStream, policeID);

//    addInfoStream.push_back(0x00);
//    addInfoStream.push_back(alarmType);
//    addInfoStream.push_back(0x01);
//    addInfoStream.push_back(0x05);
//    addInfoStream.push_back(0x00);
//    addInfoStream.push_back(0x00);
//    addInfoStream.push_back(0x00);
//    addInfoStream.push_back(0x00);
//    addInfoStream.push_back(static_cast<uint8_t>(speed));
//    tools::addToStdVector(addInfoStream, elevation);
//    tools::addToStdVector(addInfoStream, latitude);
//    tools::addToStdVector(addInfoStream, longitude);
//    addInfoStream.push_back(time.year);
//    addInfoStream.push_back(time.month);
//    addInfoStream.push_back(time.day);
//    addInfoStream.push_back(time.hour);
//    addInfoStream.push_back(time.minute);
//    addInfoStream.push_back(time.second);

    addInfoStream.push_back(0x00);
    addInfoStream.push_back(alarmType);
    addInfoStream.push_back(0x01);
    addInfoStream.push_back(0x01);
    addInfoStream.push_back(0x00);
    addInfoStream.push_back(0x00);
    addInfoStream.push_back(0x00);
    addInfoStream.push_back(0x00);
    addInfoStream.push_back(static_cast<uint8_t>(speed));
    tools::addToStdVector(addInfoStream, elevation);
    tools::addToStdVector(addInfoStream, latitude);
    tools::addToStdVector(addInfoStream, longitude);
    addInfoStream.push_back(time.year);
    addInfoStream.push_back(time.month);
    addInfoStream.push_back(time.day);
    addInfoStream.push_back(time.hour);
    addInfoStream.push_back(time.minute);
    addInfoStream.push_back(time.second);


    const uint16_t vehicleStatus = getVehicleStateStatus();
    tools::addToStdVector(addInfoStream, vehicleStatus);

    const std::vector<uint8_t> alarmID = getAlarmID();
    addInfoStream.insert(addInfoStream.end(), alarmID.begin(), alarmID.end());

    bodyStream.push_back(alarmTypeID);

    const uint8_t addInfoLength = addInfoStream.size();
    bodyStream.push_back(addInfoLength);
    bodyStream.insert(bodyStream.end(), addInfoStream.begin(), addInfoStream.end());

}

const uint16_t JT808EventSerializer::getVehicleStateStatus()
{
    uint16_t status = 0x0000;
    if(terminalStatus.isACCOn)
        tools::setBit(status, 0);

    tools::setBit(status, 10);

    return status;

}

const std::vector<uint8_t> JT808EventSerializer::getAlarmID()
{
    std::vector<uint8_t> id;

    std::vector<uint8_t> terminalIDBytes = tools::getUint8VectorFromString(terminalID);
    id.insert(id.begin(), terminalIDBytes.begin(), terminalIDBytes.end());

    id.push_back(time.year);
    id.push_back(time.month);
    id.push_back(time.day);
    id.push_back(time.hour);
    id.push_back(time.minute);
    id.push_back(time.second);

    id.push_back(alarmType);
    id.push_back(0x01);
    id.push_back(0x00);

    return id;
}

void JT808EventSerializer::setHeader()
{
    headerStream.clear();

    //MessageID
    tools::addToStdVector(headerStream, alarmMessageID);

    //Information of body
    uint16_t bodyInfo = 0;
    setBodyInfo(bodyInfo);
    tools::addToStdVector(headerStream, bodyInfo);

    //PhoneNumber
    std::vector<std::string> numbers = tools::split(terminalPhoneNumber, '-');
    if(numbers.size() == 6) {
        for(const auto &numStr : numbers) {
            const uint8_t num = static_cast<uint8_t>(std::stoi(numStr));
            uint8_t bcdNum = tools::to_bcd(num);
            headerStream.push_back(bcdNum);
        }
    }

    //Serial number
    messageSerialNum = tools::random_hex_uint16();
    tools::addToStdVector(headerStream, messageSerialNum);

    //Packets incapsulation if there is
    if(isPacketsIncapsulated) {

    }

}

void JT808EventSerializer::setBodyInfo(uint16_t &info)
{
    //body length
    const uint16_t bodySize = static_cast<uint16_t>(bodyStream.size());
    if(tools::getBit(bodySize,0)) tools::setBit(info, 0);
    if(tools::getBit(bodySize,1)) tools::setBit(info, 1);
    if(tools::getBit(bodySize,2)) tools::setBit(info, 2);
    if(tools::getBit(bodySize,3)) tools::setBit(info, 3);
    if(tools::getBit(bodySize,4)) tools::setBit(info, 4);
    if(tools::getBit(bodySize,5)) tools::setBit(info, 5);
    if(tools::getBit(bodySize,6)) tools::setBit(info, 6);
    if(tools::getBit(bodySize,7)) tools::setBit(info, 7);
    if(tools::getBit(bodySize,8)) tools::setBit(info, 8);

    //EncryptionInfo

    //subcontract
//    isPacketsIncapsulated = true;
    //reserve

}

void JT808EventSerializer::setCheckSum()
{
    checkSum = 0x00;

    for(uint8_t elem : messageStream) {
        checkSum ^= elem;
    }

    messageStream.push_back(checkSum);

}

void BCDTime::setDateAndTimeFromOrdinary(uint8_t y, uint8_t mth, uint8_t d, uint8_t h, uint8_t min, uint8_t s)
{
    year = tools::to_bcd(y);
    month = tools::to_bcd(mth);
    day = tools::to_bcd(d);
    hour = tools::to_bcd(h);
    minute = tools::to_bcd(min);
    second = tools::to_bcd(s);
}
