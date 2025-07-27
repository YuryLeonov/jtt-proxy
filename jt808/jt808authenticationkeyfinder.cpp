#include "jt808authenticationkeyfinder.h"

#include <filesystem>
#include <fstream>
#include <thread>

#include "easylogging++.h"

JT808AuthenticationKeyFinder::JT808AuthenticationKeyFinder()
{

}

JT808AuthenticationKeyFinder::~JT808AuthenticationKeyFinder()
{

}

const std::vector<uint8_t> JT808AuthenticationKeyFinder::getKey() const
{
    return key;
}

bool JT808AuthenticationKeyFinder::isKeyFound()
{
    keyFound = false;
    findKey();
    return keyFound;
}

FileJT808AuthenticationKeyFinder::FileJT808AuthenticationKeyFinder(const std::string &path) : filePath(path)
{

}

FileJT808AuthenticationKeyFinder::~FileJT808AuthenticationKeyFinder()
{

}

void FileJT808AuthenticationKeyFinder::findKey()
{

    if(!std::filesystem::exists(filePath)) {
        return;
    }

    std::ifstream file(filePath, std::ios::binary);
    if(!file) {
        LOG(ERROR) << "Ошибка открытия файла ключа авторизации на платформе" << std::endl;
        return;
    }

    const size_t fileSize = std::filesystem::file_size(filePath);

    key.clear();

    if(fileSize > 0) {
        key.reserve(fileSize);

        std::vector<uint8_t> buffer(fileSize);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
            throw std::runtime_error("Ошибка чтения файла: " + filePath);
        }

        key = std::move(buffer);
    }

    keyFound = true;

}

DataBaseJT808AuthenticationKeyFinder::DataBaseJT808AuthenticationKeyFinder()
{

}

DataBaseJT808AuthenticationKeyFinder::~DataBaseJT808AuthenticationKeyFinder()
{

}

void DataBaseJT808AuthenticationKeyFinder::findKey()
{

}

JT808AuthenticationKeyFinderCreator::~JT808AuthenticationKeyFinderCreator()
{

}

const std::vector<uint8_t> JT808AuthenticationKeyFinderCreator::getAuthenticationKey()
{
    auto product = this->factoryMethod();
    if(!product->isKeyFound()) {
        throw AuthenticationKeyNotFoundException("Ключ авторизации на платформе не найден");
    }

    return product->getKey();
}

FileJT808AuthenticationKeyFinderCreator::FileJT808AuthenticationKeyFinderCreator(const std::string &path) :
    filePath(path)
{

}

FileJT808AuthenticationKeyFinderCreator::~FileJT808AuthenticationKeyFinderCreator()
{

}

std::unique_ptr<JT808AuthenticationKeyFinder> FileJT808AuthenticationKeyFinderCreator::factoryMethod()
{
    return std::make_unique<FileJT808AuthenticationKeyFinder>(filePath);
}

DataBaseJT808AuthenticationKeyFinderCreator::~DataBaseJT808AuthenticationKeyFinderCreator()
{

}

std::unique_ptr<JT808AuthenticationKeyFinder> DataBaseJT808AuthenticationKeyFinderCreator::factoryMethod()
{
    return std::make_unique<DataBaseJT808AuthenticationKeyFinder>();
}

AuthenticationKeyNotFoundException::AuthenticationKeyNotFoundException(const std::string errMessage) : std::runtime_error(errMessage)
{

}
