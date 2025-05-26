#ifndef JT808SERVER_H
#define JT808SERVER_H

#include <string>
#include <functional>

class JT808Server
{
public:
    JT808Server(const std::string &h, int p, int c);
    ~JT808Server();

    bool start();
    void stop();
    void run();

    void setHost(const std::string &h);
    void setPort(int p);

    void setPlatformRequestHandler(const std::function<void(const std::vector<uint8_t> &buffer)> handler);

private:
    void handleClientMessage(int socketID);

private:
    std::string host;
    int port;
    int connectionsAllowed = 1;

    int socketFD;
    bool isRunning = false;

    std::function<void(const std::vector<uint8_t> &buffer)> platformRequestHandler;

};

#endif // JT808SERVER_H
