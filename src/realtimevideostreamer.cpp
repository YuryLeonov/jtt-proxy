#include "realtimevideostreamer.h"
#include "tools.h"
#include "jt1078streamtransmitrequest.h"
#include "easylogging++.h"

#include <thread>
#include <csignal>

namespace streamer
{
RealTimeVideoStreamer::~RealTimeVideoStreamer()
{
    isConnected = false;
    close(socketFd);
}

void RealTimeVideoStreamer::setVideoServerParams(const streamer::VideoServerRequisites &r)
{
    videoServer = r;
}

void RealTimeVideoStreamer::setRtsp(const std::string &rtsp)
{
    rtspLink = rtsp;
    LOG(INFO) << "Установлена ссылка: " << rtspLink;
}

void RealTimeVideoStreamer::setTerminalInfo(const TerminalInfo &tInfo)
{
    terminalInfo = tInfo;
}

bool RealTimeVideoStreamer::startStreaming()
{
    //Инициализируем декодер
    if(initDecoder()) {
        LOG(INFO) << "ДЕМУЛЬТИПЛЕКСОР ИНИЦИАЛИЗИРОВАН";
    } else {
        LOG(ERROR) << "Ошибка инициализации демультиплексора";
        return false;
    }

    //Стрим пакетов платформе
    startPacketsReading();

    return true;
}

void RealTimeVideoStreamer::stopStreaming()
{
    LOG(INFO) << "Прекращаем стриминг по каналу " << videoServer.channel;
    isStreamingInProgress = false;
    close(socketFd);
}

void RealTimeVideoStreamer::pauseStreaming()
{
    isStreamingInProgress = false;
}

bool RealTimeVideoStreamer::isStreaming()
{
    return isStreamingInProgress;
}

bool RealTimeVideoStreamer::isConnectionValid()
{
    if(connType == ConnectionType::TCP)
        return isConnected;
    else
        return true;
}

bool RealTimeVideoStreamer::initDecoder()
{
    av_log_set_level(AV_LOG_QUIET);

    decoderFormatContext = avformat_alloc_context();
    if(!decoderFormatContext) {
        LOG(ERROR) << "Ошибка выделения памяти под контекста формата для декодера";
        return false;
    }

    AVDictionary *options = NULL;
//    av_dict_set(&options, "buffer_size", "1000", 0);
//    av_dict_set(&options, "max_packet_size", "1000", 0);

    int ret = 0;
    ret = avformat_open_input(&decoderFormatContext, rtspLink.c_str(), nullptr, &options);
    if(ret != 0) {
        LOG(ERROR) << "Ошибка открытия входного файла";
        return false;
    } else {
        LOG(INFO) << "Демультиплексор открыт...";
    }

    ret = avformat_find_stream_info(decoderFormatContext, nullptr);
    if(ret < 0 ) {
        LOG(ERROR) << "Ошибка получения потоков из входного файла";
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
                LOG(ERROR) << "Ошибка парсинга параметров видеокодека декодером";
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
        LOG(ERROR) << "Ошибка поиска кодека по id";
        return false;
    }

    std::cout << "Для потока " << stream->index << " обнаружен кодек " << (*codec)->name << std::endl;

    *codecContext = avcodec_alloc_context3(*codec);
    if(!codecContext) {
        LOG(ERROR) << "Ошибка выделения памяти по контекст кодека";
        return false;
    }
    int ret = 0;

    ret = avcodec_parameters_to_context(*codecContext, stream->codecpar);
    if(ret < 0) {
        LOG(ERROR) << "Ошибка копирования параметров кодека в контекст";
        return false;
    }

    ret = avcodec_open2(*codecContext, *codec, nullptr);

    if(ret < 0) {
        LOG(ERROR) << "Ошибка открытия кодека";
        return false;
    }

    return true;
}

void RealTimeVideoStreamer::startPacketsReading()
{
    AVPacket *input_packet = av_packet_alloc();
    AVFrame *input_frame = av_frame_alloc();

    if (!input_packet) {
        LOG(ERROR) << "Ошибка выделения памяти под объект пакета" << std::endl;
        return;
    }

    long int packageNumber = 0;
    std::chrono::system_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::system_clock::time_point startIFrame = std::chrono::high_resolution_clock::now();

    std::chrono::system_clock::time_point firstPackageTime = start;
    isStreamingInProgress = true;

    std::vector<std::vector<uint8_t>> packets;

    while((av_read_frame(decoderFormatContext, input_packet) >= 0) && isStreamingInProgress) {

        if(decoderFormatContext->streams[input_packet->stream_index]->codecpar->codec_type != AVMEDIA_TYPE_VIDEO) {
            continue;
        }

        if(packageNumber && (packageNumber % 100 == 0)) {
            LOG(DEBUG) << "Стриминг по каналу " << static_cast<int>(videoServer.channel) << std::endl;
        }

        avcodec_send_packet(decoderVideoCodecContext, input_packet);
        avcodec_receive_frame(decoderVideoCodecContext, input_frame);

        rtp::RTPParams params;
        params.logicalNumber = videoServer.channel;

        if (input_frame->pict_type == AV_PICTURE_TYPE_I) {
            params.frameType = rtp::FrameType::IFrame;
        } else if(input_frame->pict_type == AV_PICTURE_TYPE_P) {
            params.frameType = rtp::FrameType::PFrame;
        } else if(input_frame->pict_type == AV_PICTURE_TYPE_B) {
            params.frameType = rtp::FrameType::BFrame;
        } else {
            params.frameType = rtp::FrameType::Undefined;
        }

        int segments = 0;
        int lastSegmentSize = 0;
        int packetSize = 1400;
        if(input_packet->buf->size > packetSize) {
            segments = (input_packet->buf->size / packetSize) + 1;
            lastSegmentSize = input_packet->buf->size % packetSize;
        }

        auto stop = std::chrono::high_resolution_clock::now();
        params.lastFrameInterval = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        params.lastIFrameInterval = std::chrono::duration_cast<std::chrono::milliseconds>(stop - startIFrame).count();
        if(input_packet->flags & AV_PKT_FLAG_KEY) {
            startIFrame = std::chrono::high_resolution_clock::now();
        }
        start = std::chrono::high_resolution_clock::now();

        int offset = 0;
        for(int i = 0; i < segments; ++i) {

            std::vector<uint8_t> vec(input_packet->buf->data + offset, input_packet->buf->data + offset + packetSize);
            packets.push_back(vec);
            offset+= packetSize;

        }

        av_packet_unref(input_packet);

        for(int i = 0; i < packets.size(); ++i) {

            if(i == (segments - 1)) {
                packetSize = lastSegmentSize;
                params.mMarker = true;
                if(segments == 1) {
                    params.subcontractType = rtp::SubcontractType::Atomic;
                } else  {
                    params.subcontractType = rtp::SubcontractType::Last;
                }
            } else {
                if(i == 0) {
                    params.mMarker = true;
                    params.subcontractType = rtp::SubcontractType::First;
                } else {
                    params.mMarker = false;
                    params.subcontractType = rtp::SubcontractType::Intermediate;
                }
            }
            params.serialNumber = packageNumber++;

            std::chrono::system_clock::time_point curentNTPFrameTime = std::chrono::high_resolution_clock::now();
            params.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(curentNTPFrameTime - firstPackageTime).count();

            JT1078StreamTransmitRequest request(terminalInfo, params, packets[i]);
            std::vector<uint8_t> requestBuffer = std::move(request.getRequest());
            if(!sendMessage(requestBuffer)) {
                LOG(ERROR) << "Ошибка отправки RTP-пакета" << std::endl;
            } else {
                LOG(TRACE) << "Отправлен пакет: ";
                LOG(TRACE) << tools::getStringFromBitStream(requestBuffer);
                LOG(TRACE) << "---------------------------------------------------";
            }

        }
        packets.clear();
    }

    LOG(INFO) << "Стриминг по каналу " << static_cast<int>(videoServer.channel) << " остановлен..." << std::endl;

    av_packet_free(&input_packet);
    input_packet = nullptr;

    av_frame_free(&input_frame);
    input_frame = nullptr;

    isStreamingInProgress = false;
}

int RealTimeVideoStreamer::sendMessage(const std::vector<uint8_t> &requestBuffer)
{
    const unsigned char *message = requestBuffer.data();
    ssize_t bytes_sent = 0;

    if(connType == streamer::ConnectionType::TCP) {

        bytes_sent = send(socketFd, message, requestBuffer.size(), MSG_NOSIGNAL);

        if (bytes_sent == -1) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                while(bytes_sent == -1) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    bytes_sent = send(socketFd, message, requestBuffer.size(), MSG_NOSIGNAL);
                }
            }
        }

    } else {
        bytes_sent = sendto(socketFd, message, requestBuffer.size(),
              MSG_CONFIRM, (const struct sockaddr *)&server_addr,
              sizeof(server_addr));
    }

    return bytes_sent;

}

//bool RealTimeVideoStreamer::establishUDPConnection()
//{
//    socketFd = socket(AF_INET, SOCK_DGRAM, 0);

//    if (socketFd == -1) {
//        std::cerr << "Ошибка подключения к видео-серверу(ошибка создания сокета)" << std::endl;
//        return false;
//    }

//    server_addr.sin_family = AF_INET;
//    server_addr.sin_port = htons(videoServer.udpPort);

//    return true;
//}

void streamer::RealTimeVideoStreamer::setConnectionType(streamer::ConnectionType type)
{
    connType = type;
}

bool RealTimeVideoStreamer::establishTCPConnection()
{
    socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd == -1) {
        LOG(ERROR) << "Ошибка подключения к видео-серверу(ошибка создания сокета)" << std::endl;
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if(setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) {
         LOG(ERROR) << "Ошибка установки таймаута переподключения к видео-серверу" << std::endl;
        return false;
    }

    if (setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
        LOG(ERROR) << "Ошибка установки таймаута на чтение из сокета видео-сервера" << std::endl;
        return false;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(videoServer.tcpPort);
    inet_pton(AF_INET, videoServer.host.c_str(), &server_addr.sin_addr);
    if(connect(socketFd, (sockaddr*)&server_addr, sizeof(server_addr))) {
        close(socketFd);
        LOG(ERROR) << "Ошибка подключения к видео-серверу(проверьте реквизиты сервера)" << std::endl;
        return false;
    } else {
         LOG(INFO) << "Соединение с видео-сервером установлено(" << socketFd << ")";
         isConnected = true;

    }

    return true;
}

}
