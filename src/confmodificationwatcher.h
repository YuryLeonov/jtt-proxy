#ifndef CONFMODIFICATIONWATCHER_H
#define CONFMODIFICATIONWATCHER_H

#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

class ConfModificationWatcher
{
public:
    ConfModificationWatcher(const std::string &path);
    ~ConfModificationWatcher() noexcept;

    bool checkIfFileWasModified();

private:
    std::string filePath = "";
    fs::file_time_type lastTimeModification;
};

#endif // CONFMODIFICATIONWATCHER_H
