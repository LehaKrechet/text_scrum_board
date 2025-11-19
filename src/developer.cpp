#include "developer.h"
#include <string>
#include <stdexcept>

// Получение имени разработчика
std::string Developer::get_name() const {
    return name;
}

// Установка имени разработчика с проверкой на пустое значение
void Developer::set_name(std::string n) {
    if (n.empty()) {
        throw std::invalid_argument("Developer name cannot be empty");
    }
    name = n;
}