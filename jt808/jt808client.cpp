#include "jt808client.h"
#include "jt808registrationrequest.h"
#include "jt808generalresponserequest.h"
#include "jt808authenticationrequest.h"
#include "jt808heartbeatrequest.h"
#include "jt808authenticationkeyfinder.h"
#include "jt808mediauploadrequest.h"
#include "jt808mediauploadeventinforequest.h"
#include "jt808terminalparametersrequest.h"
#include "jt1078uploadedresourceslistrequest.h"
#include "jt808headerparser.h"

#include "tools.h"
#include "easylogging++.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include "netdb.h"
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <iostream>
#include <filesystem>
#include <regex>

JT808Client::JT808Client()
{

}

JT808Client::JT808Client(const TerminalInfo &tInfo, const platform::PlatformInfo &pInfo) :
    terminalInfo(tInfo),platformInfo(pInfo)
{
//    videoStreamer = std::make_shared<streamer::RealTimeVideoStreamer>();

    connectToPlatform();
}

JT808Client::~JT808Client()
{
    if(!close(socketFd))
        std::cout << "Соединение с платформой закрыто" << std::endl;
    else
        std::cerr << "Ошибка закрытия соединения с платформой" << std::endl;
}

void JT808Client::sendRegistrationRequest()
{
    int bytes_read = -1;

    while(bytes_read <= 0) {
        JT808RegistrationRequest request(terminalInfo);
        std::vector<uint8_t> requestBuffer = std::move(request.getRequest());

        unsigned char *message = requestBuffer.data();

        if(socketFd >= 0) {
            ssize_t bytes_sent = send(socketFd, message, requestBuffer.size(), MSG_NOSIGNAL);
            if (bytes_sent == -1 && (errno == EPIPE || errno == ECONNRESET)) {
                std::cerr << "Ошибка отправки данных(сервер закрыл соединение)." << std::endl;
                while(!connectToHost()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(platformInfo.reconnectTimeout));
                }
                continue;
            } else {
                LOG(DEBUG) << "Отправлен запрос на регистрацию: " << tools::getStringFromBitStream(requestBuffer) << std::endl;
            }
        } else {
            std::cerr << "Сокет закрыт" << std::endl;
        }

        char buffer[1024] = {0};
        bytes_read = read(socketFd, buffer, 1024);
        if (bytes_read < 0) {
            std::cerr << "Ошибка при чтении ответа на запрос на регистрацию терминала" << std::endl;
            continue;
        } else {
            if(bytes_read == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10000));
                continue;
            }
        }

        std::vector<uint8_t> vec(bytes_read);
        std::copy(buffer, buffer + bytes_read, vec.begin());

        parseRegistrationAnswer(std::move(vec));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sendAuthenticationRequest();
    }
}

void JT808Client::parseRegistrationAnswer(std::vector<uint8_t> answer)
{
    LOG(DEBUG) << "Ответ на запрос регистрации: " << tools::getStringFromBitStream(answer) << std::endl;
    const uint8_t registrationResult = answer[11];
    if(registrationResult) {
        std::cerr << "Ошибка регистрации: " << std::dec << registrationResult << std::endl;
        return;
    } else {
        std::cout << "РЕГИСТРАЦИЯ УСПЕШНА" << std::endl << std::endl;
    }

    authenticationKey.clear();
    //Get authentication key
    const int bodyLength = (answer[3] << 8) | answer[4];
    const int authenticationKeyLength = (int)(bodyLength - 3);

    for(int i = 16; i < 16 + authenticationKeyLength; ++i) {
        authenticationKey.push_back(answer[i]);
    }

    std::cout << "Получен ключ авторизации: ";
    for(int i = 0; i < authenticationKey.size(); ++i) {
        std::cout << std::hex << static_cast<int>(authenticationKey[i]) << " ";
    }

    std::cout << std::endl;

    writeAuthenticationKeyToFile("../config/authKey.bin");

}

void JT808Client::writeAuthenticationKeyToFile(const std::string &path)
{
    std::ofstream file(path, std::ios::binary | std::ios::trunc);

    if(!file) {
        std::cerr << "Ошибка открытия файла для записи ключа авторизации на платформе" << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char *>(authenticationKey.data()), authenticationKey.size());

    if(file.good()) {
        std::cout << "Ключ авторизации сохранен в файл" << std::endl;
    } else {
        std::cerr << "Ошибка сохранения ключа вторизации в файл" << std::endl;
    }

}

void JT808Client::sendAuthenticationRequest()
{
    JT808AuthenticationRequest request(authenticationKey, terminalInfo);
    std::vector<uint8_t> requestBuffer = std::move(request.getRequest());

    unsigned char *message = requestBuffer.data();

    ssize_t bytes_sent = -1;
    int bytes_read = -1;
    char buffer[1024] = {0};

    uint8_t errors_count = 0;
    while(true) {
        bytes_sent = send(socketFd, message, requestBuffer.size(), MSG_NOSIGNAL);
        if (bytes_sent == -1) {
            std::cerr << "Ошибка отправки запроса на авторизацию." << std::endl;
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                while(bytes_sent == -1) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    std::cout << "Повторная отправка запроса на авторизацию..." << std::endl;
                    bytes_sent = send(socketFd, message, requestBuffer.size(), MSG_NOSIGNAL);
                }
            }
        }

        std::cout << "Отправлен запрос на авторизацию: " << tools::getStringFromBitStream(requestBuffer) << std::endl;

        bytes_read = read(socketFd, buffer, 1024);
        if (bytes_read <= 0) {
            std::cerr << "Ошибка при чтении ответа на запрос авторизации: " << errno << std::endl;
            errors_count++;
            std::this_thread::sleep_for(std::chrono::milliseconds(platformInfo.reconnectTimeout));
            if(errors_count == 3) {
                errors_count = 0;
                reconnectToHost();
                break;
            }
            continue;
        } else {
            break;
        }
    }

    std::vector<uint8_t> vec(bytes_read);
    std::copy(buffer, buffer + bytes_read, vec.begin());

    if(parseGeneralResponse(std::move(vec))) {
        std::cout << "АВТОРИЗАЦИЯ УСПЕШНА" << std::endl << std::endl;
        isConnected = true;
        heartBeatThread = std::thread([this](){
            while(true) {
                if(isConnected) {
                    sendHeartBeatRequest();
                    std::this_thread::sleep_for(std::chrono::milliseconds(platformInfo.heartBeatTimeout));
                }
            }
        });
        heartBeatThread.detach();

        //Запускаем поток обработки ответов от платформы
        platformAnswerHandlerThread = std::thread([this](){
            startPlatformAnswerHandler();
        });
        platformAnswerHandlerThread.detach();

        //Отправляем запрос на установку параметров терминала
        sendTerminalParametersRequest();

    } else {
        std::cerr << "Ошибка авторизации"<< std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(30000));
        sendAuthenticationRequest();
    }
}

void JT808Client::sendHeartBeatRequest()
{
    JT808HeartbeatRequest request(terminalInfo);
    std::vector<uint8_t> requestBuffer = std::move(request.getRequest());

    unsigned char *message = requestBuffer.data();
    ssize_t bytes_sent = send(socketFd, message, requestBuffer.size(), 0);
    if (bytes_sent == -1) {
        LOG(DEBUG) << "Ошибка запроса heartbeat" << std::endl;
        return;
    } else {
        LOG(DEBUG) << "Отправлен heartbeat: " << tools::getStringFromBitStream(requestBuffer) << std::endl;
    }
}

void JT808Client::sendTerminalParametersRequest()
{
    JT808TerminalParametersRequest request(terminalInfo, terminalParams, 1);
    std::vector<uint8_t> requestBuffer = std::move(request.getRequest());

    unsigned char *message = requestBuffer.data();
    ssize_t bytes_sent = send(socketFd, message, requestBuffer.size(), 0);
    if (bytes_sent == -1) {
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            while(bytes_sent == -1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::cout << "Повторная отправка параметров терминала..." << std::endl;
                bytes_sent = send(socketFd, message, requestBuffer.size(), MSG_NOSIGNAL);
            }
        }
        return;
    }

    LOG(DEBUG) << "Параметры терминала отправлены: " << tools::getStringFromBitStream(requestBuffer) << std::endl;

}

void JT808Client::startPlatformAnswerHandler()
{
    while (isConnected) {
            char buffer[1024];
            int bytes_recieved = recv(socketFd, buffer, sizeof(buffer), 0);

            if(bytes_recieved == 0) {
                std::cout << "Сервер отключился" << std::endl;
                close(socketFd);
                isConnected = false;
                connectToPlatform();
            } else if(bytes_recieved > 0) {
                std::vector<uint8_t> answer(bytes_recieved);
                std::copy(buffer, buffer + bytes_recieved, answer.begin());
                LOG(DEBUG) << "Получен запрос от сервера: " << tools::getStringFromBitStream(answer) << std::endl;
                handlePlatformAnswer(answer);
            } else {
                if(errno != EAGAIN) {
                    LOG(ERROR) << "bytes_recieved == -1, errno = " << errno << std::endl;
                }
            }
    }
}

void JT808Client::handlePlatformAnswer(const std::vector<uint8_t> &answer)
{
    JT808Header header = JT808HeaderParser::getHeader(answer);
    if(header.messageID == 0x8001) {
        parseGeneralResponse(std::move(answer));
    } else if(header.messageID == 0x9101){
        parseRealTimeVideoRequest(std::move(answer));
    } else if(header.messageID == 0x9102){
        parseRealTimeVideoControlRequest(std::move(answer));
    } else if(header.messageID == 0x9105) {
        parseRealTimeVideoStatusRequest(std::move(answer));
    } else if(header.messageID == 0x9205) {
        parseArchiveListRequest(std::move(answer));
    } else if(header.messageID == 0x8802) {
        std::cout << "Got stored multimedia data retrival" << std::endl;
    } else if(header.messageID == 0x8800) {
        std::cout << tools::getStringFromBitStream(answer) << std::endl;
    }
    else {
        std::cout << "Неизвестный запрос: " << tools::getStringFromBitStream(answer) << std::endl;
    }
}

bool JT808Client::parseGeneralResponse(const std::vector<uint8_t> &response)
{
    const int size = response.size();
    if(size == 0) {
        return false;
    }

    if((response[0] != 0x7e) || (response[size - 1] != 0x7e)) {
        std::cerr << "Неверный формат ответа General response" << std::endl;
        return false;
    }

    const uint16_t replyID = (response[13] << 8) | response[14];
    const uint16_t requestID = (response[15] << 8) | response[16];
    const int result = static_cast<int>(response[17]);

    return !result;
}

bool JT808Client::parseRealTimeVideoRequest(const std::vector<uint8_t> &request)
{
    streamer::VideoServerRequisites vsRequisites;

    std::vector<uint8_t> body(request.begin() + 13, request.end() - 2);
    uint8_t offset = 0;

    const int ipLength = static_cast<int>(body[offset++]);
    offset += ipLength;
    std::vector<uint8_t> ipBuffer(body.begin() + 1, body.begin() + offset);

    vsRequisites.host = tools::hex_bytes_to_string(ipBuffer);
    vsRequisites.tcpPort = tools::make_uint16(body[offset], body[offset+1]);

    offset+=2;
    vsRequisites.udpPort = tools::make_uint16(body[offset], body[offset+1]);
    offset+=2;
    vsRequisites.channel = body[offset++];
    vsRequisites.dataType = body[offset++];
    vsRequisites.streamType = body[offset++];

    if(videoStreamers.count(vsRequisites.channel) > 0) {
        std::cout << "По этому каналу уже ведется стриминг, игнорируем запрос" << std::endl;
        return false;
    }

    if(vsRequisites.channel == 1 || vsRequisites.channel == 2) {
        vsRequisites.printInfo();
        streamVideo(vsRequisites, request);
    } else
        return false;


    return true;
}

bool JT808Client::parseRealTimeVideoControlRequest(const std::vector<uint8_t> &request)
{
    std::cout << tools::getStringFromBitStream(request) << std::endl;

    const uint8_t channelNumber = static_cast<int>(request[13]);
    const uint8_t controlInstruction = static_cast<int>(request[14]);
    const uint8_t closeType = static_cast<int>(request[15]);
    const uint8_t switchStreamType = static_cast<int>(request[16]);

    std::cout << "Channel: " << static_cast<int>(channelNumber) << std::endl;
    std::cout << "ControlInstruction: " << static_cast<int>(controlInstruction) << std::endl;
    std::cout << "CloseType: " << static_cast<int>(closeType) << std::endl;
    std::cout << "SwitchStreamType: " << static_cast<int>(switchStreamType) << std::endl;
    std::cout << std::endl;

    if(controlInstruction == 0) {
        if(videoStreamers.count(channelNumber) > 0) {
            videoStreamers.at(channelNumber)->stopStreaming();
            videoStreamers.erase(channelNumber);
        }
    }

    return true;

}

bool JT808Client::parseRealTimeVideoStatusRequest(const std::vector<uint8_t> &request)
{
    JT808HeaderParser headerParser;
    JT808Header header = headerParser.getHeader(request);

    const uint8_t channelNumber = static_cast<int>(request[13]);
    const uint8_t packetLossRate = static_cast<int>(request[14])*100;

    if(packetLossRate > 0) {
        std::cout << "Потеряно " << packetLossRate << " пакетов" << std::endl;
    }

//    JT808GeneralResponseRequest generalResponse(terminalInfo, header.messageSerialNumber, header.messageID, JT808GeneralResponseRequest::Result::Success);
//    std::vector<uint8_t> requestBuffer = std::move(generalResponse.getRequest());
//    unsigned char *message = requestBuffer.data();
//    ssize_t bytes_sent = send(socketFd, message, requestBuffer.size(), 0);
//    if (bytes_sent == -1) {
//        std::cerr << "Ошибка отправки video general response" << std::endl;
//        return false;
//    } else {
//        std::cout << "Отправлен general response в ответ на статус передачи видео." << std::endl;
//    }

    return true;
}

bool JT808Client::parseArchiveListRequest(const std::vector<uint8_t> &request)
{
    std::cout << "Запрос на список архивных данных: " << tools::getStringFromBitStream(request) << std::endl;

    JT808HeaderParser headerParser;
    JT808Header header = headerParser.getHeader(request);

    UploadedResource resource;

    resource.channelNumber = static_cast<int>(request[13]);
    resource.startYear = tools::from_bcd(static_cast<int>(request[14]));
    resource.startMonth = tools::from_bcd(static_cast<int>(request[15]));
    resource.startDay = tools::from_bcd(static_cast<int>(request[16]));
    resource.startHour = tools::from_bcd(static_cast<int>(request[17]));
    resource.startMin = tools::from_bcd(static_cast<int>(request[18]));
    resource.startSec = tools::from_bcd(static_cast<int>(request[19]));

    resource.stopYear = tools::from_bcd(static_cast<int>(request[20]));
    resource.stopMonth = tools::from_bcd(static_cast<int>(request[21]));
    resource.stopDay = tools::from_bcd(static_cast<int>(request[22]));
    resource.stopHour = tools::from_bcd(static_cast<int>(request[23]));
    resource.stopMin = tools::from_bcd(static_cast<int>(request[24]));
    resource.stopSec = tools::from_bcd(static_cast<int>(request[25]));

    std::vector<uint8_t> alarmBytes{request[26], request[27], request[28], request[29]};
    resource.alarmSign = tools::make_uint64(alarmBytes);

    resource.resourceType = static_cast<int>(request[30]);
    resource.streamType = static_cast<int>(request[31]);
    resource.memoryType = static_cast<int>(request[32]);

    const std::string filePath = "/opt/lms/mtp-808-proxy/tests/test.mp4";
    resource.fileSize = std::filesystem::file_size(filePath);

    try {
        resource.fileSize = std::filesystem::file_size(filePath);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    JT1078UploadedResourcesListRequest answer(header.messageSerialNumber, resource, terminalInfo);
    std::vector<uint8_t> answerBuffer = std::move(answer.getRequest());

    unsigned char *message = answerBuffer.data();
    ssize_t bytes_sent = send(socketFd, message, answerBuffer.size(), MSG_NOSIGNAL);
    if (bytes_sent == -1) {
        std::cout << "Ошибка отправки данных о видеофайлах" << std::endl;
        return false;
    } else {
        std::cout << "Запрос JT1078UploadedResourcesList отправлен" << std::endl;

    }

    return true;
}

void JT808Client::streamVideo(const streamer::VideoServerRequisites &vsRequisites, const std::vector<uint8_t> &request)
{   
    std::unique_ptr<streamer::RealTimeVideoStreamer> videoStreamer = std::make_unique<streamer::RealTimeVideoStreamer>();

    videoStreamer->setVideoServerParams(vsRequisites);
//    videoStreamer->setRtsp(platformInfo.videoServer.rtspLinks.at(videoStreamers.size()));
    videoStreamer->setRtsp("rtsp://admin:a1234567@10.2.0.16:554/Streaming/Channels/101");
    videoStreamer->setTerminalInfo(terminalInfo);
    if(platformInfo.videoServer.connType == platform::ConnectionType::TCP)
        videoStreamer->setConnectionType(streamer::ConnectionType::TCP);
    else
        videoStreamer->setConnectionType(streamer::ConnectionType::UDP);

    videoStreamers[vsRequisites.channel] = std::move(videoStreamer);

    std::cout << "Начинаем стриминг по каналу " << static_cast<int>(vsRequisites.channel) << std::endl;
    if(videoStreamers[vsRequisites.channel]->establishTCPConnection()) {
        std::thread streamThread([this, vsRequisites](){
            videoStreamers[vsRequisites.channel]->startStreaming();
        });
        streamThread.detach();
    }
}

bool JT808Client::sendAlarmMessage(const std::vector<uint8_t> &request)
{
    if(socketFd <= 0)
        return false;

    unsigned char *message = const_cast<unsigned char *>(request.data());
    ssize_t bytes_sent = send(socketFd, message, request.size(), 0);
    if (bytes_sent == -1) {
        std::cerr << "Ошибка отправки данных alarm" << std::endl;
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            while(bytes_sent == -1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::cout << "Повторная отправка аларма..." << std::endl;
                bytes_sent = send(socketFd, message, request.size(), MSG_NOSIGNAL);
            }
        }
    }

    std::cout << std::endl << "Аларм отправлен на платформу!" << std::endl;
    LOG(TRACE) << "Аларм: ";
    LOG(TRACE) << tools::getStringFromBitStream(request) << std::endl;
    LOG(TRACE) << "**********************";
    return true;
}

void JT808Client::sendAlarmVideoFile(const std::string &filePath, const std::vector<uint8_t> &alarmBody)
{
    std::thread uploadVideoThread(&JT808Client::sendVideoFile, this,  filePath, std::move(alarmBody));
    uploadVideoThread.detach();
}

void JT808Client::  connectToPlatform()
{
    isAuthenticationKeyExists = checkIfAuthenticationKeyExists();

    while(!connectToHost()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(platformInfo.reconnectTimeout));
    }

    if(!isAuthenticationKeyExists) {
        sendRegistrationRequest();
    } else {
        sendAuthenticationRequest();
    }
}

void JT808Client::setConfiguration(const TerminalInfo &tInfo, const platform::PlatformInfo &pInfo)
{
    setTerminalInfo(tInfo);
    setPlatformInfo(pInfo);
    setTerminalParameters();
}

bool JT808Client::connectDomain()
{
    struct addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    const std::string portName = std::to_string(platformInfo.port);

    struct addrinfo* server_info;
    int status = getaddrinfo(platformInfo.ipAddress.c_str(), portName.c_str(), &hints, &server_info);
    if (status != 0) {
        std::cerr << "Ошибка подключения к платформе мониторинга(ошибка getaddrinfo)" << std::endl;
        return false;
    }

    socketFd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (socketFd == -1) {
        freeaddrinfo(server_info);
        std::cerr << "Ошибка подключения к платформе мониторинга(ошибка socket)" << std::endl;
        return false;
    }

    if (connect(socketFd, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        close(socketFd);
        freeaddrinfo(server_info);
        std::cerr << "Ошибка подключения к платформе мониторинга(ошибка подключения)" << std::endl;
    }

    std::cout << "Соединение  с платформой установлено" << std::endl << std::endl;

    return true;
}

bool JT808Client::connectIp()
{
    socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd == -1) {
        std::cerr << "Ошибка подключения к платформе мониторинга(ошибка создания сокета)" << std::endl;
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if(setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) {
         std::cerr << "Ошибка установки таймаута переподключения к серверу" << std::endl;
        return false;
    }

    if (setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
        std::cerr << "Ошибка установки таймаута на чтение из сокета сервеа" << std::endl;
        return false;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(platformInfo.port);
    inet_pton(AF_INET, platformInfo.ipAddress.c_str(), &server_addr.sin_addr);
    std::cout << "Попытка подключения к платформе:" << platformInfo.ipAddress << ":" << std::to_string(platformInfo.port) << std::endl;
    if(connect(socketFd, (sockaddr*)&server_addr, sizeof(server_addr))) {
        close(socketFd);
        std::cerr << "Ошибка подключения к платформе мониторинга(проверьте реквизиты сервера)" << std::endl;
        return false;
    } else {
         std::cout << "Соединение с платформой установлено" << std::endl << std::endl;
    }

    return true;
}

void JT808Client::setTerminalInfo(TerminalInfo info)
{
    terminalInfo = info;
    setTerminalParameters();
}

void JT808Client::setPlatformInfo(platform::PlatformInfo info)
{
    platformInfo = info;
}

void JT808Client::setTerminalParameters()
{
    terminalParams.terminalHeartbeatTimeout = static_cast<uint32_t>(platformInfo.heartBeatTimeout / 1000);
    terminalParams.primaryServerIpAddress = terminalInfo.localServerInfo.host;
    terminalParams.serverTCPPort = terminalInfo.localServerInfo.port;
    terminalParams.serverUDPPort = terminalInfo.localServerInfo.port;
}

void JT808Client::sendVideoFile(const std::string &filePath, const std::vector<uint8_t> &alarmBody)
{
    std::lock_guard<std::mutex> lock(sendingMessageMutex);

    std::filesystem::path fsPath = filePath;
    std::string fileName = fsPath.filename().string();
    uint32_t fileSize = std::filesystem::file_size(filePath);

    const size_t chunkSize = 500;
    size_t offset = 0;

    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open videofile!\n";
        return;
    }

    std::vector<uint8_t> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);

    JT808MediaUploadEventInfoRequest mediaEventInfoRequest(multimediaID, terminalInfo);
    std::vector<uint8_t> requestBuffer = std::move(mediaEventInfoRequest.getRequest());
    unsigned char *message = requestBuffer.data();
    ssize_t bytes_sent = send(socketFd, message, requestBuffer.size(), MSG_NOSIGNAL);
    if (bytes_sent == -1) {
        std::cout << "Ошибка запроса на передачу информации о медиафайле" << std::endl;
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            while(bytes_sent == -1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::cout << "Повторная отправка запроса на передачу информации о медиафайле..." << std::endl;
                bytes_sent = send(socketFd, message, requestBuffer.size(), MSG_NOSIGNAL);
            }
        }
    }

    std::cout << "Запрос JT808MediaUploadEventInfo отправлен" << std::endl;
    LOG(TRACE) << "MediaInfo:";
    LOG(TRACE) << tools::getStringFromBitStream(requestBuffer);
    LOG(TRACE) << "************************";


    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::vector<std::vector<uint8_t>> chunks = std::move(tools::splitFileIntoChunks(filePath, 1000));
    std::cout << "Выгрузка медиафайла(" << static_cast<int>(multimediaID) << ")..." << std::endl;
    std::cout << "Количество чанков: " << chunks.size() << std::endl;
    int size = 0;
    for(int i = 0; i < chunks.size(); ++i) {
        JT808MediaUploadRequest request(multimediaID, terminalInfo, chunks.at(i), alarmBody);
        requestBuffer = std::move(request.getRequest());
        message = requestBuffer.data();
        bytes_sent = send(socketFd, message, requestBuffer.size(), MSG_NOSIGNAL);
        if (bytes_sent == -1) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                while(bytes_sent == -1) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    bytes_sent = send(socketFd, message, requestBuffer.size(), MSG_NOSIGNAL);
                }
            }
            continue;
        }

        if(i == 0 || i == 20) {
            LOG(TRACE) << "Chunk:";
            LOG(TRACE) << tools::getStringFromBitStream(requestBuffer);
        }
        size += bytes_sent;

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    std::cout << "Файл выгружен!(отправлено " << size << " байт)" << std::endl;
    isFileUploadingInProgress = false;
    multimediaID++;

}

bool JT808Client::checkIfAuthenticationKeyExists()
{
    FileJT808AuthenticationKeyFinderCreator creator("../config/authKey.bin"); //В случае с поиском ключа в базе просто добавить тут нужный Creator

    try {
        authenticationKey = creator.getAuthenticationKey();
    } catch(const AuthenticationKeyNotFoundException &excep) {
        return false;
    }

    std::cout << std::endl;

    return true;
}

bool JT808Client::connectToHost()
{
    if(isIPAddress(platformInfo.ipAddress)) {
        return connectIp();
    } else {
        return connectDomain();
    }
}

void JT808Client::reconnectToHost()
{
    close(socketFd);
    connectToHost();
}

bool JT808Client::isIPAddress(const std::string &socketAddr)
{
    std::regex ipv4_regex(
        R"(^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)"
    );
    return std::regex_match(socketAddr, ipv4_regex);
}

JT808ConnectionErrorException::JT808ConnectionErrorException(const std::string errMessage) : runtime_error(errMessage)
{

}
