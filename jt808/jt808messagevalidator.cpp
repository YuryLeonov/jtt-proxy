#include "jt808messagevalidator.h"
#include "jt808header.h"
#include <tools.h>
#include "easylogging++.h"

JT808MessageValidator::JT808MessageValidator()
{

}

bool JT808MessageValidator::validateMessage(const std::vector<uint8_t> &message)
{
    const uint8_t startByte = message[0];
    const uint8_t stopByte = message[message.size() - 1];

    //Проверка стартового и последнего битов
    if(startByte != 0x7e || stopByte != 0x7e) {
        LOG(ERROR) << "Ошибка стартового или закрывающего байтов" << std::endl;
        return false;
    }

    std::vector<uint8_t> main(message.begin() + 1, message.end() - 1);

    //Проверка контрольной суммы
    const uint8_t checkCode = main[main.size() - 1];

    main.pop_back();

    uint8_t calculatedCheckCode = 0x00;

    for(uint8_t elem : main) {
        calculatedCheckCode ^= elem;
    }

    if(checkCode != calculatedCheckCode) {

        std::cerr << "Ошибка проверки контрольной суммы!!" << std::endl;
        std::cout << "Контрольная сумма из сообщения: " << std::hex << static_cast<int>(checkCode) << std::endl;
        std::cout << "Вычисленная контрольная сумма: " << std::hex << static_cast<int>(calculatedCheckCode) << std::endl;

        return false;
    }

    return true;
}
