#pragma once

#include <string>

// Класс Developer представляет разработчика в команде
// Содержит базовую информацию о разработчике
class Developer {
private:
    std::string name;  // Имя разработчика
    
public:
    // Конструктор с обязательным именем
    // Использует список инициализации для эффективной инициализации
    Developer(std::string n) : name(n) {}
    
    // Получение имени разработчика
    std::string get_name() const;
    
    // Установка имени разработчика
    void set_name(std::string n);
};