#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <functional>

#include "websocketpp/config/asio_no_tls_client.hpp"
#include "websocketpp/client.hpp"
#include "eventrequests.h"
#include "DbMessagesHelper.h"
#include "alarmtypes.h"


using wsclient = websocketpp::client<websocketpp::config::asio_client>;
using message_ptr = websocketpp::config::asio_client::message_type::ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


class WebSocketClient
{
public:

    struct Event
    {
        std::string id;
        std::time_t time;
    };

    WebSocketClient(const std::string &hostIP, int port);
    ~WebSocketClient() noexcept;

    void setReconnectTimeout(int timeout);
    void setSurveyInterval(int interval);
    void setExternalMessageAlarmHandler(const std::function<void(const alarms::AlarmType &type, const std::string &message)> &f);
    void setExternalMessageMediaInfoHandler(const std::function<void(const std::string &eventID, const std::string &message)> &f);

    void connect();

    void setAlarmVideosCount(int count);

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

    void sendRequestForEvents();
    void sendRequestForMediaInfo(const std::string &eventUUID);
    void removeOldUnuploadedEvents();

    void runConnectionThread();

    void closeConnection();

    void startPeriodicSurvey();

    EventAnswerInfo parseEventServerAnswer(const std::string &message);

private:
    wsclient client;
    std::string serverHostIP = "127.0.0.1";
    int serverPort = 8088;
    int reconnectTimeout = 3000;
    int surveyInterval = 5000;
    std::string serverURI = "";
    int alarmVideosCount = 1;

    std::function<void(const alarms::AlarmType &type, const std::string &message)> externalMessageAlarmHandler;
    std::function<void(const std::string &eventID, const std::string &message)> externalMessageMediaInfoHandler;

    std::thread connectionLoopThread;
    websocketpp::connection_hdl currentConnectionHandler;

    std::shared_ptr<websocketpp::lib::asio::steady_timer> surveyTimer;

    std::string eventInfoMTP = "";
    std::string lastEventTime = "";
    std::string eventMediaInfoMTP = "";
    std::string currentEventUUID = "";

    std::unique_ptr<IDbMessagesHelper> dbMessageHelper;

    std::queue<Event> unuploadedEvents;
    std::map<std::string, int> receivedVideosForEvent;
    std::vector<std::string> uploadedVideoFiles;

};

#endif // WEBSOCKETCLIENT_H
