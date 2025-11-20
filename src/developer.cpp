#include "developer.h"
#include <string>
#include <stdexcept>

// Получение имени разработчика
// Простой геттер, возвращает копию имени
std::string Developer::get_name() const {
    return name;
}

// Установка имени разработчика с проверкой на пустое значение
// Защита от некорректных данных
void Developer::set_name(std::string n) {
    // Проверяем что имя не пустое
    if (n.empty()) {
        throw std::invalid_argument("Developer name cannot be empty");
    }
    // Если проверка пройдена, устанавливаем новое имя
    name = n;
}