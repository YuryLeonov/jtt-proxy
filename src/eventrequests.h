#ifndef EVENTREQUESTS_H
#define EVENTREQUESTS_H

#include "nlohmann/json.hpp"
#include <string>

struct EventAnswerInfo
{
    bool status = false;
    std::string answer = "";
    std::string mtp = "";
    bool isData = false;
    std::string error = "";
};

namespace EventRequests
{
    const std::string createGetAllRequest(const std::string &mtp, const std::string &datetime, const std::string &srcId);
}

#endif // EVENTREQUESTS_H
