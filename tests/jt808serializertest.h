#ifndef JT808SERIALIZERTEST_H
#define JT808SERIALIZERTEST_H

#include "jt808serializer.h"

void jt808SerializerTest(const std::string &terminalPhoneNumber) {
    JT808EventSerializer serializer;
    serializer.setTerminalPhoneNumber(terminalPhoneNumber);
    std::ifstream stream("../tests/events.json", std::ifstream::in);

    if(!stream.is_open()) {
        std::cout << "Ошибка открытия файла события" << std::endl;

        return;
    }

    json eventJson;
    stream >> eventJson;

    std::vector<uint8_t> vec = serializer.serializeToBitStream(eventJson);

    for(const auto &num : vec) {
        std::cout << std::hex << static_cast<int>(num) << " ";
    }
    std::cout << std::endl;

}


#endif // JT808SERIALIZERTEST_H
