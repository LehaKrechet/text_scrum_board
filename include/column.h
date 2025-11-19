#pragma once

#include <vector>
#include <string>
#include <memory>
#include "task.h"

// Предварительные объявления для избежания циклических зависимостей
class Task;
class Board;

// Класс Column представляет колонку на Scrum доске
// Каждая колонка содержит список задач и имеет название
class Column {
private:
    std::string name;  // Название колонки (например, "To Do", "In Progress")
    std::vector<std::unique_ptr<Task>> tasks;  // Список задач в колонке

public:
    // Конструктор колонки с обязательным названием
    Column(std::string n) : name(n) {}
    
    // Методы для работы с задачами в колонке
    void add_task(std::unique_ptr<Task> task);              // Добавление задачи
    void delete_task(const std::string& task_title);        // Удаление задачи по названию
    std::vector<std::unique_ptr<Task>>& get_tasks();        // Получение списка задач
    const std::vector<std::unique_ptr<Task>>& get_tasks() const;  // Константная версия
    std::string get_name() const;                           // Получение названия колонки
    void set_name(std::string n);                           // Установка названия колонки
    
    // Поиск задачи по заголовку в колонке
    Task* find_task(const std::string& title) const;
    
    // Оператор сравнения для проверки эквивалентности колонок
    bool operator==(const Column& other) const {
        return name == other.name;
    }
};

// Функции для работы с задачами между колонками
void move_task(Column* start, Column* end, Task* task);  // Перемещение задачи между колонками
Task* search_task(Board& board, const std::string& col, const std::string& title);  // Поиск задачи на доске