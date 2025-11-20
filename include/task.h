#pragma once

#include <string>
#include <vector>
#include <memory>
#include "developer.h"

// Предварительное объявление класса Developer для избежания циклических зависимостей
// Позволяет использовать указатель на Developer без включения всего заголовка
class Developer;

// Класс Task представляет задачу в Scrum доске
// Содержит всю информацию о задаче: описание, ID, заголовок, приоритет и разработчика
class Task {
private:
    std::string description;  // Подробное описание задачи
    std::string id;           // Уникальный идентификатор задачи (генерируется автоматически)
    std::string title;        // Краткий заголовок задачи
    int priority;             // Приоритет задачи от 0 до 10
    Developer* developer;     // Указатель на разработчика, назначенного на задачу

    // Вектор для хранения всех использованных ID задач
    static std::vector<std::string> used_ids;
    
    // Метод для генерации случайной строки заданной длины
    // Используется для создания уникальных идентификаторов
    static std::string generate_random_string(int length);
    
public:
    // Конструктор задачи с обязательным заголовком
    // Автоматически генерирует уникальный ID
    Task(std::string titl);
    
    // Разрешаем перемещение для эффективной работы с умными указателями
    // Компилятор сгенерирует конструктор перемещения и оператор присваивания перемещением
    Task(Task&& other) = default;
    Task& operator=(Task&& other) = default;
    
    // Методы для работы с ID задач
    
    // Генерация уникального ID для задачи
    // Гарантирует что все ID в системе уникальны
    static std::string generate_id();
    
    // Очистка списка использованных ID
    // Полезно при загрузке новой доски или сбросе состояния
    static void clear_used_ids();
    
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
    // Сравнивает задачи по всем полям кроме указателя на разработчика
    bool operator==(const Task& other) const {
        return title == other.title && 
               description == other.description && 
               id == other.id;
    }
};