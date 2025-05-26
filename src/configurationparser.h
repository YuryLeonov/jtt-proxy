#ifndef CONFIGURATIONPARSER_H
#define CONFIGURATIONPARSER_H

/*
 * Класс отвечает за парсинг конфигурационного файла сервиса
 */
#include <stdexcept>
#include "json.hpp"
#include "configuration.h"

class ConfFileOpenErrorException : public std::runtime_error {
    public:
        ConfFileOpenErrorException(const char *message) : std::runtime_error(message) {}

};

using json = nlohmann::json;

class ConfigurationParser
{
public:
    ConfigurationParser(const std::string &path);

    void initConfJson();
    Configuration parseConfiguration();

private:
    std::string filePath;
    json confJson;
};

#endif // CONFIGURATIONPARSER_H
