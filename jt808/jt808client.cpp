#include "jt808client.h"
#include "jt808registrationrequest.h"
#include "jt808authenticationrequest.h"
#include "jt808heartbeatrequest.h"
#include "jt808authenticationkeyfinder.h"
#include "jt808mediauploadrequest.h"
#include "jt808mediauploadeventinforequest.h"
#include "jt808terminalparametersrequest.h"

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

JT808Client::JT808Client(const TerminalInfo &tInfo, const PlatformInfo &pInfo) :
    terminalInfo(tInfo),platformInfo(pInfo)
{
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
    JT808RegistrationRequest request(terminalInfo);
    std::vector<uint8_t> requestBuffer = std::move(request.getRequest());

    unsigned char *message = requestBuffer.data();
    ssize_t bytes_sent = send(socketFd, message, requestBuffer.size(), 0);
    if (bytes_sent == -1) {
        std::cerr << "Ошибка отправки данных" << std::endl;
        return;
    }

    std::cout << std::endl << "Запрос на регистрацию: " << std::endl;
    tools::printHexBitStream(requestBuffer);

    char buffer[1024] = {0};
    int bytes_read = read(socketFd, buffer, 1024);
    if (bytes_read < 0) {
        std::cerr << "Ошибка при чтении ответа на запрос на регистрацию терминала" << std::endl;
    }

    std::vector<uint8_t> vec(bytes_read);
    std::copy(buffer, buffer + bytes_read, vec.begin());

    parseRegistrationAnswer(std::move(vec));

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    sendAuthenticationRequest();

}

void JT808Client::parseRegistrationAnswer(std::vector<uint8_t> answer)
{
    const uint8_t registrationResult = answer[11];
    if(registrationResult) {
        std::cerr << "Ошибка регистрации: " << std::dec << registrationResult << std::endl;
        return;
    } else {
        std::cout << "РЕГИСТРАЦИЯ УСПЕШНА" << std::endl << std::endl;
    }

    std::cout << std::endl << "Запрос на регистрацию(ответ): " << std::endl;
    tools::printHexBitStream(answer);

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
    ssize_t bytes_sent = send(socketFd, message, requestBuffer.size(), 0);
    if (bytes_sent == -1) {
        std::cerr << "Ошибка отправки данных" << std::endl;
        return;
    }

    char buffer[1024] = {0};
    int bytes_read = read(socketFd, buffer, 1024);
    if (bytes_read < 0) {
        std::cerr << "Ошибка при чтении ответа" << std::endl;
    }

    std::vector<uint8_t> vec(bytes_read);
    std::copy(buffer, buffer + bytes_read, vec.begin());

    if(parseGeneralResponse(std::move(vec))) {
        std::cout << "АВТОРИЗАЦИЯ УСПЕШНА" << std::endl << std::endl;
        isConnected = true;
        heartBeatThread = std::thread([this](){
            while(true) {
                sendHeartBeatRequest();
                std::this_thread::sleep_for(std::chrono::milliseconds(platformInfo.heartBeatTimeout));
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
        std::this_thread::sleep_for(std::chrono::microseconds(500));
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
    }
}

void JT808Client::sendTerminalParametersRequest()
{
    JT808TerminalParametersRequest request(terminalInfo, terminalParams, 4);
    std::vector<uint8_t> requestBuffer = std::move(request.getRequest());

    unsigned char *message = requestBuffer.data();
    std::cout << "Отправка параметров терминала на платформу" << std::endl;
    ssize_t bytes_sent = send(socketFd, message, requestBuffer.size(), 0);
    if (bytes_sent == -1) {
        std::cerr << "Ошибка отправки данных" << std::endl;
        return;
    } else
        std::cout << "Параметры терминала отправлены" << std::endl << std::endl;

}

void JT808Client::startPlatformAnswerHandler()
{
    char buffer[1024];
    while (isConnected) {
            int bytes_recieved = recv(socketFd, buffer, sizeof(buffer), 0);
            if(bytes_recieved <= 0) {
                std::cout << "Сервер отключился" << std::endl;
                isConnected = false;
                break;
            }

            std::vector<uint8_t> answer(bytes_recieved);
            std::copy(buffer, buffer + bytes_recieved, answer.begin());
            handlePlatformAnswer(answer);

    }
}

void JT808Client::handlePlatformAnswer(const std::vector<uint8_t> &answer)
{
    const uint16_t answerID = (answer[1] << 8) | answer[2];
    if(answerID == 0x8001) {
        parseGeneralResponse(answer);
    }
}

bool JT808Client::parseGeneralResponse(const std::vector<uint8_t> &response)
{
    const uint16_t replyID = (response[13] << 8) | response[14];
    const uint16_t requestID = (response[15] << 8) | response[16];
    const int result = static_cast<int>(response[17]);

    return !result;
}


void JT808Client::sendAlarmMessage(const std::vector<uint8_t> &request, const std::vector<uint8_t> &alarmBody)
{
    unsigned char *message = const_cast<unsigned char *>(request.data());

    ssize_t bytes_sent = send(socketFd, message, request.size(), 0);

    if (bytes_sent == -1) {
        std::cerr << "Ошибка отправки данных alarm" << std::endl;
        return;
    } else {
        std::cout << "Аларм отправлен на платформу!" << std::endl << std::endl;
    }

//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
//    if(!isFileUploadingInProgress) {
//        isFileUploadingInProgress = true;
//        sendVideoFile("../tests/test.mp4", alarmBody);
//    } else {
//        std::cout << "Файл выгружается..." << std::endl;
//    }
}

void JT808Client::connectToPlatform()
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

void JT808Client::setConfiguration(const TerminalInfo &tInfo, const PlatformInfo &pInfo)
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

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(platformInfo.port);
    inet_pton(AF_INET, platformInfo.ipAddress.c_str(), &server_addr.sin_addr);
    std::cout << "Попытка подключения к " << platformInfo.ipAddress << std::endl;
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

void JT808Client::setPlatformInfo(PlatformInfo info)
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

    JT808MediaUploadEventInfoRequest mediaEventInfoRequest(terminalInfo);
    std::vector<uint8_t> requestBuffer = std::move(mediaEventInfoRequest.getRequest());
    unsigned char *message = requestBuffer.data();
    ssize_t bytes_sent = send(socketFd, message, requestBuffer.size(), 0);
    if (bytes_sent == -1) {
        std::cout << "Ошибка запроса на передачу информации о медиафайле" << std::endl;
        return;
    } else {
        std::cout << "Запрос JT808MediaUploadEventInfo отправлен" << std::endl;

    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "Выгрузка медиафайла..." << std::endl;
    std::vector<std::vector<uint8_t>> chunks = std::move(tools::splitFileIntoChunks(filePath, 470));
    for(int i = 0; i < chunks.size(); ++i) {
        JT808MediaUploadRequest request(terminalInfo, chunks.at(i), alarmBody);
        requestBuffer = std::move(request.getRequest());
        message = requestBuffer.data();
        bytes_sent = send(socketFd, message, requestBuffer.size(), 0);
        if (bytes_sent == -1) {
            std::cout << "error sending chank" << std::endl;
            continue;
        }

//        std::cout << "Выгружено: " << std::dec << bytes_sent << " байт" << std::endl;
    }

    std::cout << "Файл выгружен!" << std::endl;
    isFileUploadingInProgress = false;
}

bool JT808Client::checkIfAuthenticationKeyExists()
{
    FileJT808AuthenticationKeyFinderCreator creator("../config/authKey.bin"); //В случае с поиском ключа в базе просто добавить тут нужный Creator

    try {
        authenticationKey = creator.getAuthenticationKey();
    } catch(const AuthenticationKeyNotFoundException &excep) {
        std::cout << "Ключ авторизации не найден" << std::endl;
        return false;
    }

    std::cout << "Ключ авторизации найден: ";
    tools::printHexBitStream(authenticationKey);
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
