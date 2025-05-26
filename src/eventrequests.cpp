#include "eventrequests.h"

using json = nlohmann::json;

namespace EventRequests
{

const std::string createGetAllRequest(const std::string &mtp, const std::string &datetime, const std::string &srcID)
{
    const std::string condition = std::string("> ") + std::string("\'") + datetime + std::string("\'");

    std::string paramName = "";
    if(srcID == "archivewriter_events") {
        paramName = "timestamp-begin";
    } else
        paramName = "timestamp";

    json request = {
        {"mtp",mtp},
        {"request","getAll"},
        {"srcId",srcID},
        {"order","desc"},
        {"limit",5},
        {"param-name",paramName},
        {"param-condition", condition}
    };

    return request.dump();

}

} //end of namespace EventRequests
