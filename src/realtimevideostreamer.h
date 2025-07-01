#ifndef REALTIMEVIDEOSTREAMER_H
#define REALTIMEVIDEOSTREAMER_H

#include <vector>
#include <inttypes.h>
#include <iostream>

struct VideoServerRequisites
{
    std::string host = "";
    uint16_t tcpPort = 0;
    uint16_t udpPort = 0;
    uint8_t channel = 0;
    uint8_t dataType = 0;
    uint8_t streamType = 0;

    void printInfo() {
        std::cout << "Реквизиты видеосервера: " << std::endl;
        std::cout << "IP-адрес: " << host << std::endl;
        std::cout << "TCP-порт: " << std::dec << tcpPort << std::endl;
        std::cout << "UDP-порт: " << static_cast<int>(udpPort) << std::endl;
        std::cout << "Номер логического канала: " << static_cast<int>(channel) << std::endl;
        std::cout << "Тип данных: " << static_cast<int>(dataType) << std::endl;
        std::cout << "Тип потока: " << static_cast<int>(streamType) << std::endl;
    }
};

class RealTimeVideoStreamer
{
public:
    RealTimeVideoStreamer(const std::vector<uint8_t> &hex, const std::string &rtsp);
    ~RealTimeVideoStreamer();

private:
    void parseHex(const std::vector<uint8_t> &hex);

private:
    std::string rtspLink = "";
    VideoServerRequisites videoServer;
};

#endif // REALTIMEVIDEOSTREAMER_H
