#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <sstream>
#include <iomanip>
#include <string>
#include "easylogging++.h"

namespace logger
{
    const std::string CONF_LOGGER_PATH = "../logger/logger.conf";

    std::string getDailyFilename(const std::string &logType) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        const std::string fileName = "../logs/" + logType + "_%Y-%m-%d.log";

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), fileName.c_str());
        return ss.str();
    }

    void setLogger()
    {
        el::Configurations config(CONF_LOGGER_PATH);
        if(!config.parseFromFile(CONF_LOGGER_PATH)) {
            std::cerr << "Ошибка парсинга файла логгера" << std::endl;
        }

        config.setToDefault();

        config.set(el::Level::Info,
                 el::ConfigurationType::Filename,
                 getDailyFilename("info"));

        config.set(el::Level::Debug,
                 el::ConfigurationType::Filename,
                 getDailyFilename("debug"));

        config.set(el::Level::Error,
                 el::ConfigurationType::Filename,
                 getDailyFilename("error"));

        config.set(el::Level::Trace,
                 el::ConfigurationType::Filename,
                 getDailyFilename("trace"));

        el::Loggers::reconfigureLogger("default", config);
        el::Loggers::flushAll();
    }

    void checkAndUpdateLogFile() {
        static std::string currentEtalonFilename = getDailyFilename("info");

        std::string newEtalonFilename = getDailyFilename("info");
        if (newEtalonFilename != currentEtalonFilename) {
            setLogger();
            currentEtalonFilename = newEtalonFilename;
            LOG(INFO) << "Созданы логи для нового дня!!!";
        }
    }

}


#endif // LOGGER_H
