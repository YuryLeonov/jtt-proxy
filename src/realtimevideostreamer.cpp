#include "realtimevideostreamer.h"

RealTimeVideoStreamer::RealTimeVideoStreamer(const std::vector<uint8_t> &hex, const std::string &rtsp) :
    rtspLink(rtsp)
{
    parseHex(hex);
}

RealTimeVideoStreamer::~RealTimeVideoStreamer()
{

}

void RealTimeVideoStreamer::parseHex(const std::vector<uint8_t> &hex)
{
    parseHex(hex);
}
