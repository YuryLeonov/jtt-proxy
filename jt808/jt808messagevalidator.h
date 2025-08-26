#ifndef JT808MESSAGEVALIDATOR_H
#define JT808MESSAGEVALIDATOR_H

#include <vector>
#include <inttypes.h>

class JT808MessageValidator
{
public:
    JT808MessageValidator();
    ~JT808MessageValidator();

    static bool validateMessage(const std::vector<uint8_t> &message);

};

#endif // JT808MESSAGEVALIDATOR_H
