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

    void addToStdVector(std::vector<uint8_t> &vec, uint64_t num)
    {
        for (int i = 7; i >= 0; --i) {  // от старшего к младшему
            vec.push_back(static_cast<uint8_t>((num >> (8 * i)) & 0xFF));
        }
    }

    bool isByteInStream(const std::vector<uint8_t> &vec, const uint8_t &byte)
    {
        for(const auto &el : vec) {
            if(el == byte)
                return true;
        }

        return false;
    }

    std::string hex_bytes_to_string(const std::vector<uint8_t>& hex_bytes)
    {
        return std::string(hex_bytes.begin(), hex_bytes.end());
    }

    std::vector<uint8_t> getUint8VectorFromString(const std::string &str) {
        std::vector<uint8_t> vec(str.begin(), str.end());

        return vec;
    }

    std::string getStringFromBitStream(const std::vector<uint8_t> &hex_bytes)
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');

        for (uint8_t byte : hex_bytes) {
            oss << std::setw(2) << static_cast<int>(byte) << " ";
        }

        return oss.str();
    }

    uint16_t make_uint16(uint8_t high_byte, uint8_t low_byte)
    {
        return (static_cast<uint16_t>(high_byte) << 8) | low_byte;
    }

    uint32_t make_uint32(const std::vector<uint8_t> &bytes)
    {
        return (static_cast<uint32_t>(bytes[0]) << 24) |
               (static_cast<uint32_t>(bytes[1]) << 16) |
               (static_cast<uint32_t>(bytes[2]) << 8) |
               static_cast<uint32_t>(bytes[3]);
    }

    uint64_t make_uint64(const std::vector<uint8_t> &bytes)
    {
        return (static_cast<uint64_t>(bytes[0]) << 56) |
               (static_cast<uint64_t>(bytes[1]) << 48) |
               (static_cast<uint64_t>(bytes[2]) << 40) |
               (static_cast<uint64_t>(bytes[3]) << 32) |
               (static_cast<uint64_t>(bytes[4]) << 24) |
               (static_cast<uint64_t>(bytes[5]) << 16) |
               (static_cast<uint64_t>(bytes[6]) << 8) |
                static_cast<uint64_t>(bytes[7]);
    }

    const std::string addSecondsToTime(const std::string &timeStr, int secs)
    {
        std::tm tm = {};
        std::istringstream ss(timeStr);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (ss.fail()) {
            throw std::runtime_error("Failed to parse time string");
        }

        // Преобразование std::tm в time_t, затем в time_point
        std::time_t time = std::mktime(&tm);
        auto point = std::chrono::system_clock::from_time_t(time) + std::chrono::seconds(secs);

        std::time_t newTime = std::chrono::system_clock::to_time_t(point);
        std::stringstream new_ss;
        new_ss << std::put_time(std::localtime(&newTime), "%Y-%m-%d %H:%M:%S.%M");

        return new_ss.str();
    }

    const std::string add10MillisecondsToTime(const std::string &time)
    {
        std::vector<std::string> v = tools::split(time, '.');
        int msecs = std::stoi(v.at(1));
        msecs += 10;
        if(msecs < 1000) {
            return v.at(0) + "." + std::to_string(msecs);
        } else {
            std::string newTime = tools::addSecondsToTime(v.at(0), 1);
            return tools::split(newTime, '.').at(0) + ".000";
        }
    }

}
