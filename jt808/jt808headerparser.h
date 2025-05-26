#ifndef JT808HEADERPARSER_H
#define JT808HEADERPARSER_H

#include "jt808header.h"
#include <vector>

class JT808HeaderParser
{
public:
    JT808HeaderParser();
    ~JT808HeaderParser();

    static JT808Header getHeader(const std::vector<uint8_t> &message);

private:

};

#endif // JT808HEADERPARSER_H
