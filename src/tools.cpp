#include "tools.h"

#include <iostream>

namespace tools {

    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::istringstream iss(s);
        std::string token;

        while (std::getline(iss, token, delimiter)) {
            if(!token.empty()) {
                tokens.push_back(token);
            }
        }

        return tokens;
    }

    uint8_t to_bcd(uint8_t value) {
        return ((value / 10) << 4) | (value % 10);
    }

    uint8_t from_bcd(uint8_t bcd) {
        return ((bcd >> 4) * 10) + (bcd & 0x0F);
    }

    void setBit(uint16_t &num, uint8_t bitPos) {
        num |= (1 << bitPos);
    }

    void setBit(uint32_t &num, uint8_t bitPos) {
        num |= (1U << bitPos);
    }

    void clearBit(uint16_t &num, uint8_t bitPos) {
        num &= ~(1 << bitPos);
    }

    void clearBit(uint32_t &num, uint8_t bitPos) {
        num &= ~(1U << bitPos);
    }

    bool getBit(uint16_t num, uint8_t bitPos) {
        if (bitPos > 15) return false;

        return (num & (1 << bitPos)) != 0;
    }

    void replaceByteInVectorWithTwo(std::vector<uint8_t> &vec, uint8_t oldValue, uint8_t firstReplacer, uint8_t secondReplacer)
    {
        std::vector<uint8_t> newVec;

        for(const auto &el : vec) {
            if(el == oldValue) {
                newVec.push_back(firstReplacer);
                newVec.push_back(secondReplacer);
            } else {
                newVec.push_back(el);
            }
        }

        vec.clear();
        vec = std::move(newVec);
    }

    uint16_t random_hex_uint16() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint16_t> dist(0, 0xFFFF);

        uint16_t num = dist(gen);

        return  num;
    }

    std::vector<uint8_t> getUint8VectorFromString(const std::string &str) {
        std::vector<uint8_t> vec(str.begin(), str.end());

        return vec;
    }

    const std::string getPastTime(int seconds)
    {
        auto now = std::chrono::system_clock::now();
        auto newTime = now - std::chrono::seconds(seconds);
        std::time_t time = std::chrono::system_clock::to_time_t(newTime);
        std::stringstream ss;
         ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%M");

         return ss.str();
    }

    uint8_t calculateChecksum(const std::vector<uint8_t>& data) {
        uint8_t checksum = 0;
        for (uint8_t byte : data) {
            checksum ^= byte;
        }
        return checksum;
    }

    void printHexBitStream(const std::vector<uint8_t> &vec) {
        for(int i = 0; i < vec.size(); ++i) {
            std::cout << std::hex << static_cast<int>(vec[i]) << " ";
        }
        std::cout << std::endl;
    }

    const std::string getStringFromBitStream(const std::vector<uint8_t> &vec) {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');

        for (uint8_t byte : vec) {
            oss << std::setw(2) << static_cast<int>(byte) << " ";
        }

        return oss.str();
    }

    std::vector<std::vector<uint8_t>> splitFileIntoChunks(const std::string& filePath, size_t chunkSize) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + filePath);
        }

        std::vector<std::vector<uint8_t>> chunks;
        std::vector<uint8_t> buffer(chunkSize);

        while (file) {
            file.read(reinterpret_cast<char*>(buffer.data()), chunkSize);
            size_t bytesRead = file.gcount();

            if (bytesRead > 0) {
                buffer.resize(bytesRead);  // Обрезаем буфер до фактически прочитанных данных
                chunks.push_back(buffer);
            }
        }

        return chunks;
    }

    void parseJT808Request(const std::vector<uint8_t> &vec)
    {
        std::vector<uint8_t> request = std::move(vec);
        //1)Избавляемся от граничных битов
        request.erase(request.begin());
        request.pop_back();
        //2)Читаем заголовок

        printHexBitStream(request);
    }

    void setBit(uint8_t &num, int pos)
    {
        num |= (1 << pos);
    }

    void addToStdVector(std::vector<uint8_t> &vec, uint16_t num) {
        vec.push_back(static_cast<uint8_t>((num >> 8) & 0xFF));
        vec.push_back(static_cast<uint8_t>(num & 0xFF));
    }

    void addToStdVector(std::vector<uint8_t> &vec, uint32_t num) {
        vec.push_back(static_cast<uint8_t>((num >> 24) & 0xFF));
        vec.push_back(static_cast<uint8_t>((num >> 16) & 0xFF));
        vec.push_back(static_cast<uint8_t>((num >> 8) & 0xFF));
        vec.push_back(static_cast<uint8_t>(num & 0xFF));
    }


    void addToStdVector(std::vector<uint8_t> &vec, int32_t num)
    {
        vec.push_back(static_cast<uint8_t>((num >> 24) & 0xFF));  // Старший байт (биты 24-31)
        vec.push_back(static_cast<uint8_t>((num >> 16) & 0xFF));  // Биты 16-23
        vec.push_back(static_cast<uint8_t>((num >> 8)  & 0xFF));  // Биты 8-15
        vec.push_back(static_cast<uint8_t>(num & 0xFF));
    }

    bool isByteInStream(const std::vector<uint8_t> &vec, const uint8_t &byte)
    {
        for(const auto &el : vec) {
            if(el == byte)
                return true;
        }

        return false;
    }

    uint16_t make_uint16(uint8_t high_byte, uint8_t low_byte)
    {
        return (static_cast<uint16_t>(high_byte) << 8) | low_byte;
    }

}
