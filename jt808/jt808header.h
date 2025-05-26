#ifndef JT808HEADER_H
#define JT808HEADER_H

#include <iostream>

struct JT808Header
{
    uint16_t messageID;
    uint16_t messageSerialNumber;

    void printInfo() {
        std::cout << "Message ID: " << std::hex << messageID << std::endl;
        std::cout << "Message SerialNumber: " << std::hex << messageSerialNumber << std::endl;
        std::cout << std::endl;
    }

};

#endif // JT808HEADER_H
