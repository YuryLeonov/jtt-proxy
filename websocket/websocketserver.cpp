#include "websocketserver.h"
#include "easylogging++.h"

WebSocketServer::WebSocketServer(int port) :
    serverPort(port)
{
    setLoggingSettings();
    setHandlers();
}

WebSocketServer::~WebSocketServer()
{
    LOG(DEBUG) << "WebSocketServer destructor";
    stopListen();
}

void WebSocketServer::setExternalMessageHandler(std::function<void (const std::string &)> f)
{
    externalMessageHandler = f;
}

void WebSocketServer::startListen()
{
    listenThread = std::thread([this](){
        server.init_asio();
        server.listen(serverPort);
        server.start_accept();
        LOG(INFO) << "Local websocket server starts to listen on port " << serverPort;
        server.run();
    });
}

void WebSocketServer::setLoggingSettings()
{
    server.set_access_channels(websocketpp::log::alevel::all);
    server.clear_access_channels(websocketpp::log::alevel::frame_payload);
}

void WebSocketServer::getMessageHandler(websocketpp::connection_hdl handler, message_ptr message)
{
    externalMessageHandler(message->get_payload());
}

void WebSocketServer::connectionOpenedHandler(websocketpp::connection_hdl handler)
{
    LOG(INFO) << "New connection with websocket server established";
    websocketpp::server<websocketpp::config::asio>::connection_ptr con = server.get_con_from_hdl(handler);
    websockets.push_back(con);
}

void WebSocketServer::connectionClosedHandler(websocketpp::connection_hdl handler)
{
    LOG(INFO) << "Connection with websocket server closed";
    websocketpp::server<websocketpp::config::asio>::connection_ptr con = server.get_con_from_hdl(handler);
    websockets.remove(con);
}

void WebSocketServer::setHandlers()
{
    server.set_open_handler(bind(&WebSocketServer::connectionOpenedHandler, this, ::_1));
    server.set_close_handler(bind(&WebSocketServer::connectionClosedHandler, this, ::_1));
    server.set_message_handler(bind(&WebSocketServer::getMessageHandler, this, ::_1, ::_2));
}

void WebSocketServer::stopListen()
{
    websocketpp::lib::error_code ec;
    server.stop_listening(ec);
    if (ec) {
        return;
    }

    websockets.clear();

    server.stop();
}

