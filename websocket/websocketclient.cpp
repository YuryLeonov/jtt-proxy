#include "websocketclient.h"
#include "easylogging++.h"

#include "tools.h"

#include <thread>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

WebSocketClient::WebSocketClient(const std::string &hostIP, int port, const std::string &eventsTable) :
    serverHostIP(hostIP),
    serverPort(port),
    eventsTableName(eventsTable)
{
    formServerURI();
    setLoggingLevel();
    registerHandlers();
}

WebSocketClient::~WebSocketClient()
{
    LOG(DEBUG) << "WebSocketClient destructor";
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

void WebSocketClient::setExternalMessageRecievedHandler(const std::function<void (const std::string &)> &f)
{
    externalMessageRecievedHandler = f;
}

void WebSocketClient::formServerURI()
{
    serverURI = "ws://" + serverHostIP + ":" + std::to_string(serverPort);
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
    LOG(INFO) << "Соединение вебсокет с сервером закрыто: " << serverURI;
    surveyTimer->cancel();
}

void WebSocketClient::messageHandler(websocketpp::connection_hdl handler, message_ptr message)
{
    EventAnswerInfo answerInfo = parseEventServerAnswer(message->get_payload());
    if(!answerInfo.status) {
        std::cerr << "Ошибка запроса к клиенту БД: " << answerInfo.error << std::endl;
        return;
    }

    if(answerInfo.answer == "getAll" && answerInfo.isData/* && answerInfo.mtp == eventInfoMTP*/) {
//        const std::string time = tools::getPastTime(10);
//        const std::string getEventMediaInfoRequest = EventRequests::createGetAllRequest(eventMediaInfoMTP, time, "archivewriter_events");
//        std::cout << "Отправка запроса на информацию о видео события...  " << std::endl;
//        client.send(currentConnectionHandler, getEventMediaInfoRequest, websocketpp::frame::opcode::text);
        externalMessageRecievedHandler(message->get_payload());
    } else if(answerInfo.answer == "getAll" &&  answerInfo.mtp == eventMediaInfoMTP) {
//        externalMessageRecievedHandler(message->get_payload());
    }
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
                std::cout << "Timer stopped." << std::endl;
            } else {
                std::cerr << "Timer error: " << ec.message() << std::endl;
            }
            return;
        }

        if (auto con = currentConnectionHandler.lock()) {
            const std::string time = tools::getPastTime(10);
            const std::string getEventInfoRequest = EventRequests::createGetAllRequest(eventInfoMTP, time, eventsTableName);
//            std::cout << "Отправка запроса на события после " << time << std::endl;
            client.send(currentConnectionHandler, getEventInfoRequest, websocketpp::frame::opcode::text);
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
