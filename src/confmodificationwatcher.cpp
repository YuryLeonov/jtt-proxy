#include "confmodificationwatcher.h"

#include <iostream>

ConfModificationWatcher::ConfModificationWatcher(const std::string &path) :
    filePath(path)
{
    const fs::path pathToConf = path;
    lastTimeModification = fs::last_write_time(pathToConf);
}

ConfModificationWatcher::~ConfModificationWatcher() noexcept
{

}

bool ConfModificationWatcher::checkIfFileWasModified()
{
    const fs::path pathToConf = filePath;
    const auto modTime = fs::last_write_time(pathToConf);
    if(modTime != lastTimeModification) {
        std::cout << "Конфигурационный файл " << filePath << " изменен!!!" << std::endl;
        lastTimeModification = modTime;
        return true;
    }

    return false;
}
