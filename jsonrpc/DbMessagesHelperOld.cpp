#include "DbMessagesHelperOld.h"
#include "uuid.h"

#include <iostream>

JSON DbMessagesHelperOld::buildSaveRequest(const std::string &uuid, const std::string &declaration, const std::map<std::string, JSON> &entityMap, const SaveMode &mode) const
{
    if(entityMap.empty()) {
        std::cerr << "Ошибка формирования запроса: пустой JSON для toWrite !";
        return JSON{};
    }

    JSON j;
    j["mtp"]     = UuId::generate_uuid_v4();
    j["request"] = "addNewRecord";
    j["srcId"]   = declaration;

    for(const auto &it : entityMap)
    {
        auto mayBeUuid = it.first;
        if(!it.second.empty()) {
            j["toWrite"] = it.second;
            if(!mayBeUuid.empty())
                j["toWrite"]["event_uuid"] = mayBeUuid;
            break;
        }
    }
    return j;
}

JSON DbMessagesHelperOld::buildDeleteRequest(const std::string &declaration, const std::vector<std::string> &entityListId) const
{

    return JSON {};
}

std::string DbMessagesHelperOld::buildGetRequest(const std::string &uuid,  const std::string &declaration, const std::optional<std::vector<std::string> > &entityListId, const std::optional<int> &offset, const std::optional<int> &limit, const std::optional<std::string> &attribute, const std::optional<JSON> &minValue, const std::optional<JSON> &maxValue, const std::optional<JSON> &value, const std::optional<std::string> &order, const std::optional<JSON> &schema) const
{
    return "";
}

bool DbMessagesHelperOld::isSuccessResponse(const JSON &response, const std::optional<std::string> &expectedId) const
{
    return false;
}

std::optional<std::string> DbMessagesHelperOld::extractErrorMessage(const JSON &response) const
{
    return std::nullopt;
}

std::optional<std::map<std::string, JSON> > DbMessagesHelperOld::parseQueryResponse(const JSON &response, const std::optional<std::string> &expectedId) const
{
    return std::nullopt;
}
