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

    void addToStdVector(std::vector<uint8_t> &vec, uint16_t num);

    void addToStdVector(std::vector<uint8_t> &vec, uint32_t num);

    void addToStdVector(std::vector<uint8_t> &vec, int32_t num);

    void setBit(uint16_t &num, uint8_t bitPos);

    void setBit(uint32_t &num, uint8_t bitPos);

    void setBit(uint8_t &num, int pos);

    void clearBit(uint16_t &num, uint8_t bitPos);

    void clearBit(uint32_t &num, uint8_t bitPos);

    bool getBit(uint16_t num, uint8_t bitPos);

    void replaceByteInVectorWithTwo(std::vector<uint8_t> &vec, uint8_t oldValue, uint8_t firstReplacer, uint8_t secondReplacer);

    uint16_t random_hex_uint16();

    std::vector<uint8_t> getUint8VectorFromString(const std::string &str);

    const std::string getPastTime(int seconds);

    uint8_t calculateChecksum(const std::vector<uint8_t>& data);

    void printHexBitStream(const std::vector<uint8_t> &vec);

    const std::string getStringFromBitStream(const std::vector<uint8_t> &vec);

    std::vector<std::vector<uint8_t>> splitFileIntoChunks(const std::string& filePath, size_t chunkSize);

   void parseJT808Request(const std::vector<uint8_t> &vec);
}


#endif // TOOLS_H
