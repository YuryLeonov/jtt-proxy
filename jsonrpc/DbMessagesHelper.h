#ifndef DBMESSAGESHELPER_H
#define DBMESSAGESHELPER_H

#include "EntityError.h"
#include "IDbMessagesHelper.h"
#include <deque>

inline std::string to_string(SaveMode mode) {
    switch (mode) {
    case SaveMode::Patch:   return "patch";
    case SaveMode::Set:     return "set";
    case SaveMode::Create:  return "create";
    case SaveMode::Extend:  return "extend";
    }
    return "patch";
}

inline void to_json(nlohmann::json& j, const SaveMode& mode) {
    j = to_string(mode);
}

// --- Сериализация авторизации ---
inline void to_json(JSON& j, const UserPasswordAuth& a) {
    j = JSON{{"user", a.user}, {"password", a.password}};
}
inline void to_json(JSON& j, const TokenAuth& a) {
    j = JSON{{"token", a.token}};
}
inline void to_json(JSON& j, const SessionAuth& a) {
    j = JSON{{"session", a.session}};
}
inline void to_json(JSON& j, const Auth& a) {
    std::visit([&j](auto&& arg) { to_json(j, arg); }, a);
}

class DbMessagesHelper : public IDbMessagesHelper
{
public:
    explicit DbMessagesHelper(Auth auth)
        : IDbMessagesHelper(auth) {}

    ~DbMessagesHelper() {}

    //.buildSaveRequest("archive", {{"video", data1}, {"audio", data2}}, SaveMode::Patch);

    virtual JSON buildSaveRequest(const std::string &uuid, const std::string& declaration,
                          const std::map<std::string, JSON> &entityMap,
                          const SaveMode& mode = SaveMode::Patch) const;

    virtual JSON buildDeleteRequest(const std::string& declaration,
                            const std::vector<std::string> &entityListId) const;

    /**
     * Получение сущностей:
     * - либо по списку entityListId,
     * - либо через offset/limit/attribute/min/max/order,
     *   с опциональным schema-фильтром.
     */
    virtual std::string buildGetRequest(
        const std::string &uuid,
        const std::string& declaration,
        const std::optional<std::vector<std::string>>& entityListId       = std::nullopt,
        const std::optional<int>& offset                                  = std::nullopt,
        const std::optional<int>& limit                                   = std::nullopt,
        const std::optional<std::string>& attribute                       = std::nullopt,
        const std::optional<JSON>& minValue                               = std::nullopt,
        const std::optional<JSON>& maxValue                               = std::nullopt,
        const std::optional<JSON>& value                                  = std::nullopt,
        const std::optional<std::string>& order                           = std::nullopt,
        const std::optional<JSON>& schema                                 = std::nullopt
        ) const;

    /**
     * Проверка ответа на save/delete и query: одновременно проверяет совпадение id (с последним или ожидаемым)
     * и наличие корректного поля "result".
     * @param response JSON-ответ с полем "id" и "result".
     * @param expectedId (необязательно) конкретный id запроса для проверки.
     *                  Если не указан, сравнивается с последним зарегистрированным.
     */
    virtual bool isSuccessResponse(const JSON& response,
                           const std::optional<std::string>& expectedId = std::nullopt) const;

    virtual std::optional<std::string> extractErrorMessage(const JSON& response) const;

    /**
     * Разбор ответа на query, с проверкой id.
     * @param response JSON-ответ со "id" и "result.entity".
     * @param expectedId (необязательно) id запроса для сверки.
     */
    virtual std::optional<std::map<std::string, JSON>> parseQueryResponse(
        const JSON& response,
        const std::optional<std::string>& expectedId = std::nullopt) const;

private:
    //std::map<std::string, int> sequence_;
    mutable std::deque<std::string> m_lastRequestIds;

    void recordRequestId(const std::string& id) const;
};

#endif // DBMESSAGESHELPER_H
