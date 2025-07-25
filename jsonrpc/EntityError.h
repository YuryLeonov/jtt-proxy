#ifndef ENTITYERROR_H
#define ENTITYERROR_H

#include <string>
class EntityError {
public:
    enum class Code {
        // Конфликт при изменении
        EventConflict = 11000429,
        ConditionFailed = 11000412,
        AlreadyExists = 11000409,

        // Неправильное содержимое сущности
        InvalidAttributes = 11000422,

        // Проблемы с доступом
        NotAllowed = 11000403,

        // Аутентификация
        InvalidPassword = 11000401,
        UnknownUser = 11001401,

        // Токен
        TokenError1 = 11002401,
        TokenError2 = 11003401,
        TokenError3 = 11004401,

        // Формат токена
        TokenFormatError1 = 11005401,
        TokenFormatError2 = 11006401
    };

    static std::string toString(Code code) {
        switch (code) {
        case Code::EventConflict:
            return "Event conflict (11000429): следует повторить запрос";
        case Code::ConditionFailed:
            return "Condition failed (11000412): неактуальное sequence, перезапросите состояние сущности";
        case Code::AlreadyExists:
            return "Already exists (11000409): сущность уже существует";
        case Code::InvalidAttributes:
            return "Invalid attributes (11000422): сущность не соответствует схеме";
        case Code::NotAllowed:
            return "Not allowed (11000403): нет доступа или неверные данные";
        case Code::InvalidPassword:
            return "Invalid password (11000401): неправильный пароль";
        case Code::UnknownUser:
            return "Unknown user (11001401): неизвестный пользователь или пароль не задан";
        case Code::TokenError1:
        case Code::TokenError2:
        case Code::TokenError3:
            return "Token error (11x00401): проблемы с токеном";
        case Code::TokenFormatError1:
        case Code::TokenFormatError2:
            return "Token format error (11x00401): неверный формат токена";
        default:
            return "Unknown error";
        }
    }
};

#endif // ENTITYERROR_H
