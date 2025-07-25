#include <iostream>

#include "DbMessagesHelper.h"

#include "json.hpp"

int main()
{

    IDbMessagesHelper   *m_pDbMsgHelper = nullptr;

    auto token = IDbMessagesHelper::getSavedToken();
    if(token) {
        std::cout << "Задан token" << std::endl;
        m_pDbMsgHelper = new DbMessagesHelper(TokenAuth{token.value()});
    }
    else {
        std::cout << "Не задан токен MTP_ES_TOKEN! "
                      "Будет использована обертка для старого клинта БД!" << std::endl;
    }

    JSON getJson = m_pDbMsgHelper->buildGetRequest("event", std::nullopt, std::nullopt, 20, "timestamp", "2025-07-25 13:00:00.000",
                                                   std::nullopt, "asc", std::nullopt);

    std::string request = getJson.dump();

    std::cout << request << std::endl;


}
