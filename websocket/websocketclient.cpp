#include "websocketclient.h"
#include "easylogging++.h"

#include "tools.h"
#include "uuid.h"

#include <thread>
#include <memory>
#include <string>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

WebSocketClient::WebSocketClient(const std::string &hostIP, int port, const std::string &eventsTable) :
    serverHostIP(hostIP),
    serverPort(port),
    eventsTableName(eventsTable)
{
    auto token = IDbMessagesHelper::getSavedToken();
    if(token) {
        dbMessageHelper = std::make_unique<DbMessagesHelper>(TokenAuth{token.value()});
    }
    else {
        std::cout << "Не задан токен MTP_ES_TOKEN! " << std::endl;
        exit(1);
    }

    formServerURI();
    setLoggingLevel();
    registerHandlers();
}

WebSocketClient::~WebSocketClient()
{
    closeConnection();
}

void WebSocketClient::setReconnectTimeout(int timeout)
{
    reconnectTimeout = timeout;
}

void WebSocketClient::setSurveyInterval(int interval)
{
    surveyInterval = interval;
}

void WebSocketClient::setExternalMessageAlarmHandler(const std::function<void (const std::string &)> &f)
{
    externalMessageAlarmHandler = f;
}

void WebSocketClient::setExternalMessageMediaInfoHandler(const std::function<void (const std::string &)> &f)
{
    externalMessageMediaInfoHandler = f;
}

void WebSocketClient::formServerURI()
{
    serverURI = "ws://" + serverHostIP + ":" + std::to_string(serverPort) + "/jsonrpc";
}

void WebSocketClient::registerHandlers()
{
    client.set_open_handler(bind(&WebSocketClient::connectionEstablishedHandler, this, ::_1));
    client.set_fail_handler(bind(&WebSocketClient::connectionFailedHandler, this, ::_1));
    client.set_close_handler(bind(&WebSocketClient::connectionClosedHandler, this, ::_1));
    client.set_message_handler(bind(&WebSocketClient::messageHandler, this, ::_1, ::_2));
}

void WebSocketClient::setLoggingLevel()
{
    client.clear_access_channels(websocketpp::log::alevel::frame_header);
    client.clear_access_channels(websocketpp::log::alevel::frame_payload);
    client.set_error_channels(websocketpp::log::elevel::none);
}

void WebSocketClient::setConnection()
{
    websocketpp::lib::error_code ec;
    const wsclient::connection_ptr con = client.get_connection(serverURI, ec);
    if(ec) {
        LOG(INFO) << "Couldn't create connection: " << ec.message();
        return;
    }
    client.connect(con);
}

void WebSocketClient::connectionEstablishedHandler(websocketpp::connection_hdl handler)
{
    LOG(INFO) << "Установлено соединение вебсокет с сервером: " << serverURI;
    currentConnectionHandler = handler;

    lastEventTime = tools::getPastTime(2);

    //Запускаем периодический опрос сервера
    startPeriodicSurvey();
}

void WebSocketClient::connectionFailedHandler(websocketpp::connection_hdl handler)
{
    LOG(ERROR) << "Ошибка установки соединения вебсокет с сервером: " << serverURI;
    surveyTimer->cancel();
}

void WebSocketClient::connectionClosedHandler(websocketpp::connection_hdl handler)
{
    LOG(INFO) << "Закрыто соединение вебсокет с сервером " << serverURI;
    surveyTimer->cancel();
}

void WebSocketClient::messageHandler(websocketpp::connection_hdl handler, message_ptr message)
{
    const std::string answerMessage = message->get_payload();
    json data = json::parse(answerMessage);

    std::optional<std::map<std::string, json> > eventEntities = dbMessageHelper->parseQueryResponse(data,eventInfoMTP);
    std::optional<std::map<std::string, json> > videoEntities = dbMessageHelper->parseQueryResponse(data,eventMediaInfoMTP);

    if(eventEntities != std::nullopt) {
        for(const auto &pair : eventEntities.value()) {
            const json eventJson = pair.second;
            LOG(INFO) << "Получено событие: " << eventJson.at("info") << " c ID = " << eventJson.at("event_type") << std::endl;

            lastEventTime = tools::addSecondsToTime(eventJson.at("timestamp"), 1);

//            sendRequestForMediaInfo(pair.first);

            externalMessageAlarmHandler(eventJson.dump());
        }
    }

    if(videoEntities != std::nullopt) {
        for(const auto &pair : videoEntities.value()) {
            const json eventVideoJson = pair.second;
            externalMessageMediaInfoHandler(eventVideoJson.dump());
        }
    }
}

void WebSocketClient::sendRequestForEvents()
{
    eventInfoMTP = UuId::generate_uuid_v4();
    const std::string getEventInfoRequest = dbMessageHelper->buildGetRequest(eventInfoMTP,
                                                                            "event",
                                                                             std::nullopt,
                                                                             std::nullopt,
                                                                             20,
                                                                             "timestamp",
                                                                             lastEventTime,
                                                                             std::nullopt,
                                                                             std::nullopt,
                                                                             "asc",
                                                                             std::nullopt);

    client.send(currentConnectionHandler, getEventInfoRequest, websocketpp::frame::opcode::text);
}

void WebSocketClient::sendRequestForMediaInfo(const std::string &eventUUID)
{
    eventMediaInfoMTP = UuId::generate_uuid_v4();
    const std::string event_uuid_start = std::string("event@").append(eventUUID + "@");
    const std::string event_uuid_stop = std::string("event@").append(eventUUID + "A");
    const std::string getEventMediaInfoRequest = dbMessageHelper->buildGetRequest(eventMediaInfoMTP,
                                                                                  "video",
                                                                                  std::nullopt,
                                                                                  std::nullopt,
                                                                                  20,
                                                                                  std::nullopt,
                                                                                  event_uuid_start,
                                                                                  event_uuid_stop,
                                                                                  std::nullopt,
                                                                                  "asc",
                                                                                  std::nullopt);

    std::cout << getEventMediaInfoRequest << std::endl;

    client.send(currentConnectionHandler, getEventMediaInfoRequest, websocketpp::frame::opcode::text);
}

void WebSocketClient::runConnectionThread()
{
    while(true) {
        LOG(INFO) << "Trying to connect to server " << serverURI;
        setConnection();
        client.run();
        client.reset();
        std::this_thread::sleep_for(std::chrono::milliseconds(reconnectTimeout));
    }
}

void WebSocketClient::closeConnection()
{
    client.close(currentConnectionHandler, websocketpp::close::status::normal,"");
}

void WebSocketClient::startPeriodicSurvey()
{
    surveyTimer->expires_from_now(std::chrono::milliseconds(surveyInterval));
    surveyTimer->async_wait([this](const auto& ec) {
        if (ec) {
            if (ec == websocketpp::lib::asio::error::operation_aborted) {
                LOG(INFO) << "Остановлен таймер опроса клиента БД" << std::endl;
            } else {
                LOG(ERROR) << "Timer error: " << ec.message() << std::endl;
            }
            return;
        }

        if (auto con = currentConnectionHandler.lock()) {
            sendRequestForEvents();
        }

        startPeriodicSurvey(); // Перезапуск таймера
    });
}

EventAnswerInfo WebSocketClient::parseEventServerAnswer(const std::string &message)
{
    EventAnswerInfo info;
    json data = json::parse(message);
    info.status = data["status"];

    if(!info.status) {
        info.error = data["error"];
    } else {
        info.answer = data["answer"];
        info.mtp = data["mtp"];
        if(!data["data"].empty())
            info.isData = true;
        else
            info.isData = false;
    }

    return info;
}

void WebSocketClient::connect()
{
    try {
        client.init_asio();
        surveyTimer = std::make_shared<websocketpp::lib::asio::steady_timer>(client.get_io_service());
        connectionLoopThread = std::thread([this](){
            runConnectionThread();
        });
    } catch(const std::exception &e) {
        LOG(ERROR) << e.what();
    } catch(websocketpp::lib::error_code e) {
        LOG(ERROR) << e.message();
    } catch(...) {
        LOG(ERROR) << "Unusual exception while opening connection with webocket server";
    }
}

void WebSocketClient::sendMessage(const std::string &message)
{
    client.send(currentConnectionHandler, message, websocketpp::frame::opcode::text);
}
