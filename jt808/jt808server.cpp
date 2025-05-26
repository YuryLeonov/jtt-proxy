#include "jt808server.h"

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <vector>

JT808Server::JT808Server(const std::string &h, int p, int c) :
    host(h),
    port(p),
    connectionsAllowed(c)
{

}

JT808Server::~JT808Server()
{
    stop();
}

bool JT808Server::start()
{
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFD < 0) {
        std::cerr << "Ошибка создания сокета для сервера" << std::endl;
        return false;
    }

    sockaddr_in serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(socketFD, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
             std::cerr << "Ошибка привязки серверного сокета" << std::endl;
             close(socketFD);
             return false;
    }

    int flags = fcntl(socketFD, F_GETFL, 0);
    fcntl(socketFD, F_SETFL, flags | O_NONBLOCK);

    if (listen(socketFD, connectionsAllowed) < 0) {
        std::cerr << "Ошибка прослушивания сервера" << std::endl;
        close(socketFD);
        return false;
    }

    isRunning = true;
    std::cout << "Сервер запущен на порту " << port << std::endl;
    return true;
}

void JT808Server::stop()
{
    if(isRunning) {
        isRunning = false;
        close(socketFD);
        std::cout << "Сервер остановлен" << std::endl;
    }
}

void JT808Server::run()
{
    sockaddr_in clientAddr;
    socklen_t clientLen  = sizeof(clientAddr);

    while(isRunning) {
        int clientSocketFD = accept(socketFD, (sockaddr *)&clientAddr, &clientLen);
        if(clientSocketFD < 0) {
            continue;
        } else {
            std::cout << "Установлено соединение с клиентом: " << clientAddr.sin_addr.s_addr << std::endl;
        }

        std::thread clientMessageHandlerThread(&JT808Server::handleClientMessage, this, clientSocketFD);
        clientMessageHandlerThread.detach();

    }

}

void JT808Server::setHost(const std::string &h)
{
    host = h;
}

void JT808Server::setPort(int p)
{
    port = p;
}

void JT808Server::setPlatformRequestHandler(const std::function<void (const std::vector<uint8_t> &)> handler)
{
    platformRequestHandler = handler;
}

void JT808Server::handleClientMessage(int socketID)
{
    char buffer[1024] = {0};

    // Чтение данных от клиента
    ssize_t bytes_read = read(socketID, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        std::cerr << "Ошибка чтения из сокета" << std::endl;
        return;
    }

    std::vector<uint8_t> answer(bytes_read);
    std::copy(buffer, buffer + bytes_read, answer.begin());
    platformRequestHandler(answer);

}
