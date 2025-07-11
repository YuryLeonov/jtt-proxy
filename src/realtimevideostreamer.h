#ifndef REALTIMEVIDEOSTREAMER_H
#define REALTIMEVIDEOSTREAMER_H

#include <vector>
#include <inttypes.h>
#include <iostream>

#include "TerminalInfo.h"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/avfilter.h>
    #include <libavutil/timestamp.h>
    #include <libavutil/pixdesc.h>
    #include <libavutil/opt.h>
}

#include <sys/socket.h>
#include <arpa/inet.h>
#include "netdb.h"
#include "unistd.h"

namespace streamer
{
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

    enum class ConnectionType
    {
        TCP = 0,
        UDP
    };

    class RealTimeVideoStreamer
    {
    public:

        RealTimeVideoStreamer() = default;
        ~RealTimeVideoStreamer();

        void setVideoServerParams(const std::vector<uint8_t> &hex);
        void setRtsp(const std::string &rtsp);
        void setTerminalInfo(const TerminalInfo &tInfo);
        void setConnectionType(ConnectionType type);

        bool establishTCPConnection();
        bool establishUDPConnection();
        bool establishConnection();
        void startServerAnswerHandler();
        bool startStreaming();
        void stopStreaming();
        void pauseStreaming();
        bool isStreaming();

    private:
        void parseHex(const std::vector<uint8_t> &hex);

        bool initDecoder();
        bool fillDecoderVideoStreamInfo();
        bool fillStreamInfo(AVStream *stream, AVCodec **codec, AVCodecContext **codecContext);

        void startPacketsReading();
        int sendMessage(const std::vector<uint8_t> &message);

    private:
        std::string rtspLink = "";
        VideoServerRequisites videoServer;

        bool isStreamingInProgress = false;

        int socketFd;
        struct sockaddr_in server_addr;
        bool isConnected = false;

        TerminalInfo terminalInfo;

        ConnectionType connType = ConnectionType::TCP;

        //FFMPEG
        AVFormatContext *decoderFormatContext = nullptr;
        AVCodec *decoderVideoCodec = nullptr;
        AVCodecContext *decoderVideoCodecContext = nullptr;
        AVStream *decoderVideoStream = nullptr;
        int decoderVideoIndex = 0;

    };
}

#endif // REALTIMEVIDEOSTREAMER_H
