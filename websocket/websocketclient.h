#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <functional>

#include "websocketpp/config/asio_no_tls_client.hpp"
#include "websocketpp/client.hpp"
#include "eventrequests.h"

using wsclient = websocketpp::client<websocketpp::config::asio_client>;
using message_ptr = websocketpp::config::asio_client::message_type::ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


class WebSocketClient
{
public:
    WebSocketClient(const std::string &hostIP, int port, const std::string &eventsTable);
    ~WebSocketClient() noexcept;

    void setReconnectTimeout(int timeout);
    void setSurveyInterval(int interval);
    void setExternalMessageAlarmHandler(const std::function<void(const std::string &message)> &f);
    void setExternalMessageMediaInfoHandler(const std::function<void(const std::string &message)> &f);

    void connect();

    void sendMessage(const std::string &message);

private:
    void formServerURI();

    void registerHandlers();

    void setLoggingLevel();

    void setConnection();

    void connectionEstablishedHandler(websocketpp::connection_hdl handler);
    void connectionFailedHandler(websocketpp::connection_hdl handler);
    void connectionClosedHandler(websocketpp::connection_hdl handler);
    void messageHandler(websocketpp::connection_hdl handler, message_ptr message);

    void runConnectionThread();

    void closeConnection();

    void startPeriodicSurvey();

    EventAnswerInfo parseEventServerAnswer(const std::string &message);

private:
    wsclient client;
    std::string serverHostIP = "127.0.0.1";
    std::string eventsTableName = "";
    int serverPort = 8088;
    int reconnectTimeout = 3000; //milliseconds
    int surveyInterval = 5000;
    std::string serverURI = "";

    std::function<void(const std::string &message)> externalMessageAlarmHandler;
    std::function<void(const std::string &message)> externalMessageMediaInfoHandler;

    std::thread connectionLoopThread;
    websocketpp::connection_hdl currentConnectionHandler;

    std::shared_ptr<websocketpp::lib::asio::steady_timer> surveyTimer;

    std::string eventInfoMTP = "b53e1c02-3f8e-3708-995c-6db8c457c356";
    std::string eventMediaInfoMTP = "b53e1c02-3f8e-3708-995c-6db8c457c312";
};

#endif // WEBSOCKETCLIENT_H
