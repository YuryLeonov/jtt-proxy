#ifndef JT808REQUESTSTEST_H
#define JT808REQUESTSTEST_H

#include <string>
#include <inttypes.h>
#include "tools.h"
#include "TerminalInfo.h"
#include "jt808generalresponserequest.h"

void requestTest()
{
    TerminalInfo terminalInfo;
    terminalInfo.phoneNumber = "19-11-11-78-13-17";
    terminalInfo.provinceID = 108;
    terminalInfo.cityID = 23;
    terminalInfo.manufacturerID = "13579";
    terminalInfo.terminalModel = "QWERVDUIOPASDFGHJKLZ";
    terminalInfo.terminalID = "QAZWSBV";
    terminalInfo.licencePlateColor = 0;
    terminalInfo.vin = "WVGZZZCAZJC559100";

    uint16_t rsn = 0x0505;
    uint16_t rID = 0x0707;
    JT808GeneralResponseRequest::Result res = JT808GeneralResponseRequest::Result::Failure;

    JT808GeneralResponseRequest request(terminalInfo, rsn, rID, res);
    std::vector<uint8_t> requestBuffer = std::move(request.getRequest());
    tools::printHexBitStream(requestBuffer);
}

#endif // JT808REQUESTSTEST_H
