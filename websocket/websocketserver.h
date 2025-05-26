#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include "websocketpp/server.hpp"
#include "websocketpp/config/asio_no_tls.hpp"

#include <functional>
#include <thread>
#include <list>

using wsserver = websocketpp::server<websocketpp::config::asio>;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using message_ptr = wsserver::message_ptr;

class WebSocketServer
{
public:
    WebSocketServer(int port);
    ~WebSocketServer() noexcept;

    void setExternalMessageHandler(std::function<void(const std::string &message)> f);

    void startListen();
    void stopListen();

private:
    void setLoggingSettings();

    void getMessageHandler(websocketpp::connection_hdl handler,  message_ptr message);
    void connectionOpenedHandler(websocketpp::connection_hdl handler);
    void connectionClosedHandler(websocketpp::connection_hdl handler);
    void setHandlers();

private:
    wsserver server;
    int serverPort = 8090;
    std::thread listenThread;

    std::function<void(const std::string &message)> externalMessageHandler;

    std::list<wsserver::connection_ptr> websockets;
};

#endif // WEBSOCKETSERVER_H
