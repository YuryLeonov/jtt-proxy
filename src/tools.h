#ifndef TOOLS_H
#define TOOLS_H

#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <iomanip>
#include <chrono>
#include <fstream>

namespace tools {

    std::vector<std::string> split(const std::string& s, char delimiter);

    uint8_t to_bcd(uint8_t value);

    uint8_t from_bcd(uint8_t bcd);

    void addToStdVector(std::vector<uint8_t> &vec, uint16_t num); //Записывает в порядке big-endian двухбайтовое число num в буфер vec

    void addToStdVector(std::vector<uint8_t> &vec, uint32_t num); //Записывает в порядке big-endian четырехбайтовое число num в буфер vec

    void addToStdVector(std::vector<uint8_t> &vec, int32_t num);

    void addToStdVector(std::vector<uint8_t> &vec, uint64_t num);

    //Установка битов
    void setBit(uint16_t &num, uint8_t bitPos);

    void setBit(uint32_t &num, uint8_t bitPos);

    void setBit(uint8_t &num, int pos);

    void clearBit(uint16_t &num, uint8_t bitPos);

    void clearBit(uint32_t &num, uint8_t bitPos);

    bool getBit(uint16_t num, uint8_t bitPos);

    void replaceByteInVectorWithTwo(std::vector<uint8_t> &vec, uint8_t oldValue, uint8_t firstReplacer, uint8_t secondReplacer);

    void replaceTwoBytesInVectorWithOne(std::vector<uint8_t> &vec, uint8_t firstByte, uint8_t secondByte, uint8_t replacer);

    bool isByteInStream(const std::vector<uint8_t> &vec, const uint8_t &byte);

    uint16_t random_hex_uint16();

    std::string getStringFromBitStream(const std::vector<uint8_t>& hex_bytes); //выводит строкой последовательность байт

    std::string hex_bytes_to_string(const std::vector<uint8_t>& hex_bytes); //переводит последовательность байт в строку по unicode

    std::vector<uint8_t> getUint8VectorFromString(const std::string &str);

    const std::string getPastTime(int seconds);
    const std::string addSecondsToTime(const std::string &time, int secs);
    const std::string add10MillisecondsToTime(const std::string &time);

    uint8_t calculateChecksum(const std::vector<uint8_t>& data);

    void printHexBitStream(const std::vector<uint8_t> &vec);

    std::vector<std::vector<uint8_t>> splitFileIntoChunks(const std::string& filePath, size_t chunkSize);

   void parseJT808Request(const std::vector<uint8_t> &vec);

   uint16_t make_uint16(uint8_t high_byte, uint8_t low_byte);
   uint32_t make_uint32(const std::vector<uint8_t> &bytes);
   uint64_t make_uint64(const std::vector<uint8_t> &bytes);

}


#endif // TOOLS_H
