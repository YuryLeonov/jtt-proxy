#ifndef IDBMESSAGESHELPER_H
#define IDBMESSAGESHELPER_H

#include <string>
#include <variant>
#include <optional>

#include "json.hpp"
using JSON = nlohmann::json;

// --- Варианты авторизации ---
struct UserPasswordAuth {
    std::string user;
    std::string password;
};

struct TokenAuth {
    std::string token;
};

struct SessionAuth {
    std::string session;
};

using Auth = std::variant<UserPasswordAuth, TokenAuth, SessionAuth>;

enum class SaveMode {
    Patch,  // перезаписывает все указанные атрибуты, оставляет остальные как есть.
    Set,    // перезаписывает все указанные атрибуты, остальные удаляет.
    Create, // гарантирует, что будет создана новая сущность, иначе выдаёт ошибку
    Extend  // при “расширении” строки и массивы конкатенируются, ключи объектов перезаписываются.
};

class IDbMessagesHelper
{
public:
    IDbMessagesHelper(Auth auth)
        : m_auth(std::move(auth)) {}

    virtual ~IDbMessagesHelper() {}

    // Считывает токен из переменной окружения MTP_ES_TOKEN
    static std::optional<std::string> getSavedToken() {
        const char* raw = std::getenv("MTP_ES_TOKEN");
        if (!raw) {
            return std::nullopt;
        }
        std::string token(raw);
        if (token.empty()) {
            return std::nullopt;
        }
        return token;
    }

    virtual JSON buildSaveRequest(const std::string& declaration,
                          const std::map<std::string, JSON> &entityMap,
                          const SaveMode& mode = SaveMode::Patch) const = 0;

    virtual JSON buildDeleteRequest(const std::string& declaration,
                          const std::vector<std::string> &entityListId) const = 0;

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
        ) const = 0;

    virtual bool isSuccessResponse(const JSON& response,
                          const std::optional<std::string>& expectedId = std::nullopt) const = 0;

    virtual std::optional<std::string> extractErrorMessage(const JSON& response) const = 0;

    virtual std::optional<std::map<std::string, JSON>> parseQueryResponse(
        const JSON& response,
        const std::optional<std::string>& expectedId = std::nullopt) const = 0;

    bool wasAuthed() const {
        return std::visit([](auto&& auth){
            using T = std::decay_t<decltype(auth)>;
            if constexpr (std::is_same_v<T, UserPasswordAuth>) {
                return !auth.user.empty() && !auth.password.empty();
            } else if constexpr (std::is_same_v<T, TokenAuth>) {
                return !auth.token.empty();
            } else if constexpr (std::is_same_v<T, SessionAuth>) {
                return !auth.session.empty();
            }
            return false;
        }, m_auth);
    }

protected:
    Auth m_auth;

};

#endif // IDBMESSAGESHELPER_H
