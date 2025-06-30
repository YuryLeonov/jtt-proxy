#ifndef REALTIMEVIDEOSTREAMER_H
#define REALTIMEVIDEOSTREAMER_H

#include <vector>
#include <inttypes.h>
#include <iostream>

class RealTimeVideoStreamer
{
public:
    RealTimeVideoStreamer(const std::vector<uint8_t> &hex, const std::string &rtsp);
    ~RealTimeVideoStreamer();

private:
    void parseHex(const std::vector<uint8_t> &hex);

private:
    std::string host = "";
    int tcpPort = 0;
    int udpPort = 0;
    std::string rtspLink = "";

};

#endif // REALTIMEVIDEOSTREAMER_H
