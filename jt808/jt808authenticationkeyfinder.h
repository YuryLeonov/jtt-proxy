#ifndef JT808AYTHENTICATIONKEYFINDER_H
#define JT808AYTHENTICATIONKEYFINDER_H

#include <vector>
#include <inttypes.h>
#include <iostream>
#include <stdexcept>
#include <memory>

/*
 * Осовано на паттерне "Фабричный метод"
 */


class AuthenticationKeyNotFoundException : public std::runtime_error
{
public:
    AuthenticationKeyNotFoundException(const std::string errMessage);
};

//----------Products---------------------

class JT808AuthenticationKeyFinder
{
public:
    JT808AuthenticationKeyFinder();
    virtual ~JT808AuthenticationKeyFinder();

    const std::vector<uint8_t> getKey() const;
    bool isKeyFound();

protected:
    virtual void findKey() = 0;

protected:

    std::vector<uint8_t> key;
    bool keyFound = false;

};

class FileJT808AuthenticationKeyFinder : public JT808AuthenticationKeyFinder
{
public:
    FileJT808AuthenticationKeyFinder(const std::string &path);
    ~FileJT808AuthenticationKeyFinder();

private:
    void findKey() override;

private:
    std::string filePath = "";

};

class DataBaseJT808AuthenticationKeyFinder : public JT808AuthenticationKeyFinder
{
public:
    DataBaseJT808AuthenticationKeyFinder();
    ~DataBaseJT808AuthenticationKeyFinder();

private:
    void findKey() override;

};

//--------Creators-----------------------

class JT808AuthenticationKeyFinderCreator
{
public:
    JT808AuthenticationKeyFinderCreator() = default;
    virtual ~JT808AuthenticationKeyFinderCreator();

    virtual std::unique_ptr<JT808AuthenticationKeyFinder> factoryMethod() = 0;
    const std::vector<uint8_t> getAuthenticationKey();
};

class FileJT808AuthenticationKeyFinderCreator : public JT808AuthenticationKeyFinderCreator
{
public:
    FileJT808AuthenticationKeyFinderCreator(const std::string &path);
    ~FileJT808AuthenticationKeyFinderCreator();

    std::unique_ptr<JT808AuthenticationKeyFinder> factoryMethod() override;

private:
    std::string filePath;
};

class DataBaseJT808AuthenticationKeyFinderCreator : public JT808AuthenticationKeyFinderCreator
{
public:
    DataBaseJT808AuthenticationKeyFinderCreator() = default;
    ~DataBaseJT808AuthenticationKeyFinderCreator();

    std::unique_ptr<JT808AuthenticationKeyFinder> factoryMethod() override;

};

#endif // JT808AYTHENTICATIONKEYFINDER_H
