#pragma once

#include <string>
#include <vector>
#include <memory>
#include "developer.h"

// Предварительное объявление класса Developer для избежания циклических зависимостей
class Developer;

// Класс Task представляет задачу в Scrum доске
class Task {
private:
    std::string description;  // Описание задачи
    std::string id;           // Уникальный идентификатор задачи
    std::string title;        // Заголовок задачи
    int priority;             // Приоритет задачи
    Developer* developer;     // Указатель на разработчика, назначенного на задачу

    // Вектор для хранения всех использованных ID задач
    static std::vector<std::string> used_ids;
    
    // Метод для генерации случайной строки заданной длины
    static std::string generate_random_string(int length);
    
public:
    // Конструктор задачи с обязательным заголовком
    Task(std::string titl);
    
    // Разрешаем перемещение для эффективной работы с умными указателями
    Task(Task&& other) = default;
    Task& operator=(Task&& other) = default;
    
    // Статические методы для работы с ID задач
    static std::string generate_id();      // Генерация уникального ID
    static void clear_used_ids();          // Очистка списка использованных ID
    
    // Методы доступа и модификации полей задачи
    void set_description(std::string descript);
    std::string get_description() const;
    std::string get_id() const;
    std::string get_title() const;
    void set_title(std::string titl);
    int get_priority() const;
    void set_priority(int p);
    void set_developer(Developer* develop);
    void set_id(std::string new_id);
    Developer* get_developer() const;
    
    // Оператор сравнения для проверки эквивалентности задач
    bool operator==(const Task& other) const {
        return title == other.title && 
               description == other.description && 
               id == other.id;
    }
};