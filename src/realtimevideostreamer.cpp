#include "realtimevideostreamer.h"
#include "tools.h"
#include "jt1078streamtransmitrequest.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include "netdb.h"
#include "unistd.h"
#include <thread>

RealTimeVideoStreamer::RealTimeVideoStreamer(const std::vector<uint8_t> &hex, const std::string &rtsp, const TerminalInfo &tInfo) :
    rtspLink(rtsp),
    terminalInfo(tInfo)
{
    parseHex(hex);
}

RealTimeVideoStreamer::~RealTimeVideoStreamer()
{
    isConnected = false;
    close(socketFd);
}

void RealTimeVideoStreamer::startStreaming()
{
    //Инициализируем декодер
    if(initDecoder()) {
        std::cout << "ДЕМУЛЬТИПЛЕКСОР ИНИЦИАЛИЗИРОВАН" << std::endl;
    }

    //Стрим пакетов платформе
    startPacketsReading();

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

bool RealTimeVideoStreamer::initDecoder()
{
    decoderFormatContext = avformat_alloc_context();
    if(!decoderFormatContext) {
        std::cerr << "Ошибка выделения памяти под контекста формата для декодера" << std::endl;
        return false;
    }

    AVDictionary *options = NULL;
    av_dict_set(&options, "buffer_size", "1000", 0);
    av_dict_set(&options, "max_packet_size", "1000", 0);

    int ret = 0;
    ret = avformat_open_input(&decoderFormatContext, rtspLink.c_str(), nullptr, &options);
    if(ret != 0) {
        std::cerr << "Ошибка открытия входного файла" << std::endl;
        return false;
    } else {
        std::cout << "Демультиплексор открыт..." << std::endl;
    }

    ret = avformat_find_stream_info(decoderFormatContext, nullptr);
    if(ret < 0 ) {
        std::cerr << "Ошибка получения потоков из входного файла" << std::endl;
        return false;
    }

    for(int i = 0; i < decoderFormatContext->nb_streams; ++i) {
        if(decoderFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            decoderVideoStream = decoderFormatContext->streams[i];

            AVRational avg_frame_rate = decoderVideoStream->avg_frame_rate;
            double fps = av_q2d(avg_frame_rate);
            std::cout << "Обнаружен видеопоток с частотой кадров: " << fps << std::endl;
            decoderVideoIndex = i;

            const bool ret = fillDecoderVideoStreamInfo();
            if(!ret) {
                std::cerr << "Ошибка парсинга параметров видеокодека декодером";
                return false;
            }

        }
    }

    return true;
}

bool RealTimeVideoStreamer::fillDecoderVideoStreamInfo()
{
    return fillStreamInfo(decoderVideoStream, &decoderVideoCodec, &decoderVideoCodecContext);
}

bool RealTimeVideoStreamer::fillStreamInfo(AVStream *stream, AVCodec **codec, AVCodecContext **codecContext)
{
    *codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if(!codec) {
        std::cerr << "Ошибка поиска кодека по id" << std::endl;
        return false;
    }

    std::cout << "Для потока " << stream->index << " обнаружен кодек " << (*codec)->name << std::endl;

    *codecContext = avcodec_alloc_context3(*codec);
    if(!codecContext) {
        std::cerr << "Ошибка выделения памяти по контекст кодека" << std::endl;
        return false;
    }
    int ret = 0;

    ret = avcodec_parameters_to_context(*codecContext, stream->codecpar);
    if(ret < 0) {
        std::cerr << "Ошибка копирования параметров кодека в контекст" << std::endl;
        return false;
    }

    ret = avcodec_open2(*codecContext, *codec, nullptr);

    if(ret < 0) {
        std::cerr << "Ошибка открытия кодека" << std::endl;
        return false;
    }

    return true;
}

void RealTimeVideoStreamer::startPacketsReading()
{
    AVPacket *input_packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    if (!input_packet) {
        std::cerr << "Ошибка выделения памяти под объект пакета" << std::endl;
        return;
    }

    int i = 0;
    std::chrono::system_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::system_clock::time_point startIFrame = std::chrono::high_resolution_clock::now();

    while(av_read_frame(decoderFormatContext, input_packet) >= 0) {

        if(input_packet->buf->size > 950) {
            av_packet_unref(input_packet);
            continue;
        }

        std::vector<uint8_t> vec(input_packet->buf->data, input_packet->buf->data + input_packet->buf->size);

        RTPParams params;
        params.logicalNumber = videoServer.channel;
        params.serialNumber = i;

        auto stop = std::chrono::high_resolution_clock::now();
        params.lastFrameInterval = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        params.lastIFrameInterval = std::chrono::duration_cast<std::chrono::milliseconds>(stop - startIFrame).count();
        if(input_packet->flags & AV_PKT_FLAG_KEY) {
            startIFrame = std::chrono::high_resolution_clock::now();
        }
        start = std::chrono::high_resolution_clock::now();

        av_packet_unref(input_packet);
        ++i;


        JT1078StreamTransmitRequest request(terminalInfo, params, vec);
        std::vector<uint8_t> requestBuffer = std::move(request.getRequest());
        unsigned char *message = requestBuffer.data();
        ssize_t bytes_sent = send(socketFd, message, requestBuffer.size(), 0);
        if (bytes_sent == -1) {
            std::cerr << "Ошибка отправки видеопакета" << std::endl;
        } else {
            std::cout << "Отправлен" << std::endl;
        }
    }

    av_packet_free(&input_packet);
    input_packet = nullptr;
}

bool RealTimeVideoStreamer::establishConnection()
{
    socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd == -1) {
        std::cerr << "Ошибка подключения к видео-серверу(ошибка создания сокета)" << std::endl;
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if(setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) {
         std::cerr << "Ошибка установки таймаута переподключения к видео-серверу" << std::endl;
        return false;
    }

    if (setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
        std::cerr << "Ошибка установки таймаута на чтение из сокета видео-сервера" << std::endl;
        return false;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(videoServer.tcpPort);
    inet_pton(AF_INET, videoServer.host.c_str(), &server_addr.sin_addr);
    std::cout << "Попытка подключения к видео-серверу:" << videoServer.host << ":" << std::to_string(videoServer.tcpPort) << std::endl;
    if(connect(socketFd, (sockaddr*)&server_addr, sizeof(server_addr))) {
        close(socketFd);
        std::cerr << "Ошибка подключения к видео-серверу(проверьте реквизиты сервера)" << std::endl;
        return false;
    } else {
         std::cout << "Соединение с видео-сервером установлено" << std::endl << std::endl;
         isConnected = true;
    }

    return true;
}
