#include "alarmfileuploader.h"

#include "easylogging++.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include "netdb.h"
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <filesystem>

#include "jt808alarmattachmentrequest.h"
#include "jt808fileuploadinforequest.h"
#include "jt808fileuploadstoprequest.h"
#include "jt808headerparser.h"


AlarmFileUploader::AlarmFileUploader(const std::string &host, int port, const TerminalInfo &info) :
    storageHost(host),
    storagePort(port),
    terminalInfo(info)
{

}

AlarmFileUploader::~AlarmFileUploader()
{
    close(socketId);
}

void AlarmFileUploader::setJTAlarmTyoe(uint8_t type)
{
    jtAlarmType = type;
}

void AlarmFileUploader::setPathToVideo(const std::string &path)
{
    pathToVideo = path;
}

void AlarmFileUploader::setAlarmID(const std::vector<uint8_t> &id)
{
    alarmID = std::move(id);
}

void AlarmFileUploader::setAlarmNumber(const std::vector<uint8_t> &number)
{
    alarmNumber = std::move(number);
}

void AlarmFileUploader::setAttachments(int ats)
{
    attachments = ats;
}

void AlarmFileUploader::setUploadChannel(int ch)
{
    uploadChannel = ch;
}

bool AlarmFileUploader::connectToStorage()
{
    if(isConnected) {
        LOG(INFO) << "Уже есть соединение с сервером storage" << std::endl;
        return true;
    }

    socketId = socket(AF_INET, SOCK_STREAM, 0);

    if (socketId == -1) {
        LOG(ERROR) << "Ошибка подключения к storage(ошибка создания сокета)" << std::endl;
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if(setsockopt(socketId, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) {
         LOG(ERROR) << "Ошибка установки таймаута переподключения к серверу storage" << std::endl;
        return false;
    }

    if (setsockopt(socketId, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
        LOG(ERROR) << "Ошибка установки таймаута на чтение из сокета сервера storage" << std::endl;
        return false;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(storagePort);
    inet_pton(AF_INET, storageHost.c_str(), &server_addr.sin_addr);
    if(connect(socketId, (sockaddr*)&server_addr, sizeof(server_addr))) {
        close(socketId);
        LOG(ERROR) << "Ошибка подключения к storage(проверьте реквизиты сервера)" << std::endl;
        return false;
    } else {
         isConnected = true;
    }

    return true;
}

bool AlarmFileUploader::uploadFile()
{
    if(sendAlarmAttachmentMessageToStorage()) {
        if(initUploading()) {
            if(upload()) {
                return true;
            }
        }
    }


    return false;
}

bool AlarmFileUploader::sendAlarmAttachmentMessageToStorage()
{
    JT808AlarmAttachmentRequest request(jtAlarmType, pathToVideo, alarmID, alarmNumber, uploadChannel, attachments, terminalInfo);
    std::vector<uint8_t> requestBuffer = std::move(request.getRequest());

    unsigned char *message = requestBuffer.data();
    ssize_t bytes_sent = send(socketId, message, requestBuffer.size(), 0);
    if (bytes_sent == -1) {
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            while(bytes_sent == -1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                LOG(INFO) << "Повторная отправка 0x1210" << std::endl;
                bytes_sent = send(socketId, message, requestBuffer.size(), MSG_NOSIGNAL);
            }
        }
    }

    int bytes_read = -1;
    char buffer[1024] = {0};

    bytes_read = read(socketId, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        LOG(ERROR) << "Ошибка при чтении ответа на запрос 0x1210: " << errno << std::endl;

        while(bytes_read <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            bytes_read = read(socketId, buffer, 1024);
        }

    }

    std::vector<uint8_t> vec(bytes_read);
    std::copy(buffer, buffer + bytes_read, vec.begin());

    tools::replaceTwoBytesInVectorWithOne(vec, 0x7d, 0x02, 0x7e);
    tools::replaceTwoBytesInVectorWithOne(vec, 0x7d, 0x01, 0x7d);

    if(parseGeneralResponse(vec)) {
        return true;
    } else {
        LOG(ERROR) << "Сервер Storage запретил начало выгрузки роликов: " << tools::getStringFromBitStream(vec);
        return false;
    }
}

bool AlarmFileUploader::initUploading()
{

    while(true) {
        try {
            JT808FileUploadInfoRequest request(pathToVideo, terminalInfo);
            std::vector<uint8_t> requestBuffer = std::move(request.getRequest());

            unsigned char *message = requestBuffer.data();
            ssize_t bytes_sent = send(socketId, message, requestBuffer.size(), 0);
            if (bytes_sent == -1) {
                if(errno == EAGAIN || errno == EWOULDBLOCK) {
                    while(bytes_sent == -1) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                        LOG(INFO) << "Повторная отправка 0x1211" << std::endl;
                        bytes_sent = send(socketId, message, requestBuffer.size(), MSG_NOSIGNAL);
                    }
                }
            }

        } catch(const std::runtime_error &err) {
            LOG(ERROR) << err.what();
            return false;
        }

        int bytes_read = -1;
        char buffer[1024] = {0};

        bytes_read = read(socketId, buffer, 1024);
        if (bytes_read <= 0) {
            LOG(ERROR) << "Ошибка при чтении ответа на запрос 0x1211: " << errno << std::endl;
            return false;
        } else {
            std::vector<uint8_t> vec(bytes_read);
            std::copy(buffer, buffer + bytes_read, vec.begin());
            if(parseGeneralResponse(std::move(vec))) {
                return true;
            } else {
//                LOG(ERROR) << "Сервер Storage не принял информацию о файле.Повторяем запрос! ";
            }
        }
    }

}

bool AlarmFileUploader::upload()
{
    std::filesystem::path fsPath = pathToVideo;

    uint32_t fileSize = std::filesystem::file_size(pathToVideo);

    const size_t chunkSize = 64000;

    std::ifstream file(pathToVideo, std::ios::binary);
    if (!file) {
        LOG(ERROR) << "Ошибка открытия файла " << pathToVideo;
        return false;
    }

    std::vector<uint8_t> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);

    std::vector<std::vector<uint8_t>> chunks = std::move(tools::splitFileIntoChunks(pathToVideo, chunkSize));
    int size = 0;
    ssize_t bytes_sent = -1;
    const uint32_t header = 0x30316364;
    std::filesystem::path filePath = pathToVideo;
    std::string fileName = filePath.filename().string();
    if(fileName.length() > 50) {
        fileName = tools::split(fileName, 'T').at(1) + tools::split(fileName, 'T').at(2);
        if(fileName.length() > 50) {
            LOG(ERROR) << "Длина имени выгружаемого файла слишком большая!!!";
            return false;
        }
    }
    std::vector<uint8_t> fileNameBytes = tools::getUint8VectorFromString(fileName);

    fileNameBytes.insert(fileNameBytes.end(), 50 - fileName.length(), 0x00);
    uint32_t offset = 0;
    uint32_t dataLength = 0;
    std::vector<uint8_t> dataBody;
    for(int i = 0; i < chunks.size(); ++i) {
        dataBody.clear();
        tools::addToStdVector(dataBody, header);
        dataBody.insert(dataBody.end(), fileNameBytes.begin(), fileNameBytes.end());
        tools::addToStdVector(dataBody, offset);
        dataLength = chunks.at(i).size();
        tools::addToStdVector(dataBody, dataLength);
        dataBody.insert(dataBody.end(), chunks.at(i).begin(), chunks.at(i).end());
        unsigned char *message = dataBody.data();
        bytes_sent = send(socketId, message, dataBody.size(), MSG_NOSIGNAL);
        if (bytes_sent == -1) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                while(bytes_sent == -1) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    bytes_sent = send(socketId, message, dataBody.size(), MSG_NOSIGNAL);
                }
            }
            continue;
        }
        offset += bytes_sent;
        size += bytes_sent;

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    JT808FileUploadStopRequest request(pathToVideo, terminalInfo);
    std::vector<uint8_t> requestBuffer = std::move(request.getRequest());

    unsigned char *message = requestBuffer.data();
    bytes_sent = send(socketId, message, requestBuffer.size(), 0);
    if (bytes_sent == -1) {
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            while(bytes_sent == -1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                LOG(INFO) << "Повторная отправка 0x1212" << std::endl;
                bytes_sent = send(socketId, message, requestBuffer.size(), MSG_NOSIGNAL);
            }
        }
    }

    int bytes_read = -1;
    char buffer[1024] = {0};

    bytes_read = read(socketId, buffer, 1024);
    if (bytes_read <= 0) {
        LOG(ERROR) << "Ошибка при чтении ответа на запрос 0x1212: " << errno << std::endl;
    } else {
        std::vector<uint8_t> vec(bytes_read);
        std::copy(buffer, buffer + bytes_read, vec.begin());
        parse9212Answer(vec);
    }

    return true;
}

bool AlarmFileUploader::parseGeneralResponse(const std::vector<uint8_t> &response)
{
    const int size = response.size();
    if(size == 0) {
        return false;
    }

    if((response[0] != 0x7e) || (response[size - 1] != 0x7e)) {
        LOG(ERROR) << "Неверный формат ответа General response" << std::endl;
        return false;
    }

    const int result = static_cast<int>(response[response.size() - 3]);

    if(result == 1) {
        LOG(ERROR) << "General response failure";
    } else if(result == 2) {
        LOG(ERROR) << "General response message";
    } else if(result == 3) {
        LOG(ERROR) << "General response not supported error";
    } else if(result == 4) {
        LOG(ERROR) << "General response alarm";
    }

    return !result;
}

void AlarmFileUploader::parse9212Answer(const std::vector<uint8_t> &answer)
{
    //7e 92 12 00 0c 19 11 11 78 13 17 00 0c 08 74 65 73 74 2e 6d 70 34 02 00 00 fe 7e

    JT808HeaderParser headerParser;
    JT808Header header = headerParser.getHeader(answer);

    std::vector<uint8_t> body(answer.begin() + 13, answer.end() - 2);
    uint8_t offset = 0;

    const int fileNameLength = static_cast<int>(body[offset++]);
    offset += fileNameLength;
    std::vector<uint8_t> fileNameBuffer(body.begin() + 1, body.begin() + offset);
    const std::string fileName = tools::hex_bytes_to_string(fileNameBuffer);

    const uint8_t fileType = body[offset++];
    const uint8_t result = body[offset++];
    const uint8_t numberOfTransmissionPackets = body[offset++];

    if(!result) {
        LOG(INFO) << "Выгрузка файла " << fileName << " успешна";
    } else {
        LOG(ERROR) << "Необходима дополнительная выгрузка файла " << fileName;

    }
}
