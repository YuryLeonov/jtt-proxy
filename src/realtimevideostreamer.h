#ifndef REALTIMEVIDEOSTREAMER_H
#define REALTIMEVIDEOSTREAMER_H

#include <vector>
#include <inttypes.h>
#include <iostream>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/avfilter.h>
    #include <libavutil/timestamp.h>
    #include <libavutil/pixdesc.h>
    #include <libavutil/opt.h>
}


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

    bool establishConnection();
    void startStreaming();

private:
    void parseHex(const std::vector<uint8_t> &hex);

    bool initDecoder();
    bool fillDecoderVideoStreamInfo();
    bool fillStreamInfo(AVStream *stream, AVCodec **codec, AVCodecContext **codecContext);

    void startPacketsReading();

private:
    std::string rtspLink = "";
    VideoServerRequisites videoServer;

    int socketFd;
    bool isConnected = false;

    //FFMPEG
    AVFormatContext *decoderFormatContext = nullptr;
    AVCodec *decoderVideoCodec = nullptr;
    AVCodecContext *decoderVideoCodecContext = nullptr;
    AVStream *decoderVideoStream = nullptr;
    int decoderVideoIndex = 0;

};

#endif // REALTIMEVIDEOSTREAMER_H
