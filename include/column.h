#pragma once

#include <vector>
#include <string>
#include <memory>
#include "task.h"

// Предварительные объявления для избежания циклических зависимостей
// (когда два класса ссылаются друг на друга)
class Task;
class Board;

// Класс Column представляет колонку на Scrum доске
// Каждая колонка содержит список задач и имеет название
// Примеры колонок: "In Progress", "Done"
class Column {
private:
    std::string name;  // Название колонки (например "In Progress")
    std::vector<std::unique_ptr<Task>> tasks;  // Список задач в колонке

public:
    // Конструктор колонки с обязательным названием
    Column(std::string n) : name(n) {}
    
    // Методы для работы с задачами в колонке
    
    // Добавление задачи в колонку
    // Принимает unique_ptr для передачи владения задачей
    void add_task(std::unique_ptr<Task> task);
    
    // Удаление задачи по названию
    // Ищет задачу по заголовку и удаляет ее из колонки
    void delete_task(const std::string& task_title);
    
    // Получение списка задач (неконстантная версия)
    // Позволяет модифицировать задачи
    std::vector<std::unique_ptr<Task>>& get_tasks();
    
    // Получение списка задач (константная версия)
    // Используется когда не нужно изменять задачи
    const std::vector<std::unique_ptr<Task>>& get_tasks() const;
    
    // Получение названия колонки
    std::string get_name() const;
    
    // Установка названия колонки
    void set_name(std::string n);
    
    // Поиск задачи по заголовку в колонке
    // Возвращает указатель на задачу или nullptr если не найдена
    Task* find_task(const std::string& title) const;
    
    // Оператор сравнения для проверки эквивалентности колонок
    // Сравнивает колонки по названию
    bool operator==(const Column& other) const {
        return name == other.name;
    }
};

// Функции для работы с задачами между колонками

// Перемещение задачи между колонками
// Берет задачу из start колонки и перемещает в end колонку
void move_task(Column* start, Column* end, Task* task);

// Поиск задачи на всей доске по названию колонки и заголовку задачи
// Удобная функция для быстрого доступа к задаче без ручного поиска по колонкам
Task* search_task(Board& board, const std::string& col, const std::string& title);