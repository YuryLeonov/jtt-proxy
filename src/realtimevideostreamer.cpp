#include "realtimevideostreamer.h"

#include "tools.h"

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
    std::vector<uint8_t> body(hex.begin() + 13, hex.end() - 2);
    uint8_t offset = 0;

    const int ipLength = static_cast<int>(body[offset++]);
    offset += ipLength;
    std::vector<uint8_t> ipBuffer(body.begin() + 1, body.begin() + offset);
    videoServer.host = tools::hex_bytes_to_string(ipBuffer);

    videoServer.tcpPort = tools::make_uint16(body[offset], body[offset+1]);
    offset+=2;
    videoServer.udpPort = tools::make_uint16(body[offset], body[offset+1]);
    offset+=2;
    videoServer.channel = body[offset++];
    videoServer.dataType = body[offset++];
    videoServer.streamType = body[offset++];

    videoServer.printInfo();
}
