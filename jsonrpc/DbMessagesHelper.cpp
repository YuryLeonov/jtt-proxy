#include "DbMessagesHelper.h"
#include "uuid.h"

#include <iostream>

JSON DbMessagesHelper::buildSaveRequest(const std::string &declaration, const std::map<std::string, JSON> &entityMap, const SaveMode &mode) const
{
    if(entityMap.empty()) {
        std::cerr << "Ошибка формирования запроса: пустой entity !";
        return JSON{};
    }

    JSON j;
    j["jsonrpc"] = "2.0";
    const std::string id = UuId::generate_uuid_v4();
    j["id"]      = id;
    j["method"]  = "save";

    JSON params;
    params["auth"]        = m_auth;
    params["declaration"] = declaration;
    params["entity"]      = entityMap;

    JSON modeJs = JSON::object_t{} ;
    for(auto em: entityMap) {
        modeJs[em.first] = mode;
    }

    params["mode"] = modeJs;
    //if (!sequence_.empty()) params["sequence"] = sequence_;

    j["params"] = params;
    recordRequestId(id);
    return j;
}

JSON DbMessagesHelper::buildDeleteRequest(const std::string &declaration, const std::vector<std::string> &entityListId) const
{
    JSON j;
    j["jsonrpc"] = "2.0";
    const std::string id = UuId::generate_uuid_v4();
    j["id"]      = id;
    j["method"]  = "delete";

    JSON params;
    params["auth"]        = m_auth;
    params["declaration"] = declaration;
    params["entity"]      = entityListId;

    //if (!sequence_.empty()) params["sequence"] = sequence_;

    j["params"] = params;
    recordRequestId(id);
    return j;
}

JSON DbMessagesHelper::buildGetRequest(const std::string &declaration, const std::optional<std::vector<std::string> > &entityListId, const std::optional<int> &offset, const std::optional<int> &limit, const std::optional<std::string> &attribute, const std::optional<JSON> &minValue, const std::optional<JSON> &maxValue, const std::optional<JSON> &value, const std::optional<std::string> &order, const std::optional<JSON> &schema) const
{
    JSON j;
    j["jsonrpc"] = "2.0";
    std::string id = UuId::generate_uuid_v4();
    j["id"]      = id;
    j["method"]  = "query";

    JSON params;
    params["auth"]        = m_auth;
    params["declaration"] = declaration;

    if (entityListId) {
        params["entity"] = *entityListId;
    } else {
        if (offset)    params["offset"]    = *offset;
        if (limit)     params["limit"]     = *limit;
        if (attribute) params["attribute"] = *attribute;
        if (minValue)  params["min"]       = *minValue;
        if (maxValue)  params["max"]       = *maxValue;
        if (value)     params["value"]     = *value;
        if (order)     params["order"]     = *order;
    }
    if (schema) {
        params["schema"] = *schema;
    }

    j["params"] = params;
    recordRequestId(id);
    return j;
}

bool DbMessagesHelper::isSuccessResponse(const JSON &response, const std::optional<std::string> &expectedId) const
{
    // Проверка наличия ошибки в ответе
    if (auto err = extractErrorMessage(response)) {
        std::cerr << "errText: " << err.value() << std::endl;
        return false;
    }

    // Проверка id
    if (!response.contains("id") || !response["id"].is_string())
        return false;

    std::string respId = response["id"].get<std::string>();
    if (expectedId) {
        if (respId != *expectedId) return false;
    } else {
        if (m_lastRequestIds.empty() || respId != m_lastRequestIds.back())
            return false;
    }
    // Проверка результата
    return response.contains("result") && response["result"].is_object();
}

std::optional<std::string> DbMessagesHelper::extractErrorMessage(const JSON &response) const {
    if (response.contains("error") && response["error"].contains("message")) {
        return response["error"]["message"].get<std::string>();
    }
    return std::nullopt;
}

std::optional<std::map<std::string, JSON> > DbMessagesHelper::parseQueryResponse(const JSON &response, const std::optional<std::string> &expectedId) const
{
    if (!isSuccessResponse(response, expectedId))
        return std::nullopt;

    const auto& resultObj = response["result"];
    if (!resultObj.contains("entity") || !resultObj["entity"].is_object())
        return std::nullopt;

    std::map<std::string, JSON> entities;
    for (auto it = resultObj["entity"].begin(); it != resultObj["entity"].end(); ++it) {
        entities[it.key()] = it.value();
    }
    return entities;
}

void DbMessagesHelper::recordRequestId(const std::string &id) const {
    m_lastRequestIds.push_back(id);
    if (m_lastRequestIds.size() > 100) {
        m_lastRequestIds.pop_front();
    }
}
