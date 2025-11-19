#include "manager.h"
#include <iostream>
#include <stdexcept>

// Создание новой задачи на доске
void create_task(Board& board, const std::string& col, const std::string& title) {
    // Валидация входных параметров
    if (title.empty()) {
        throw std::invalid_argument("Task title cannot be empty");
    }
    
    // Создание новой задачи с помощью умного указателя
    auto new_task = std::make_unique<Task>(title);
    
    // Поиск целевой колонки
    Column* column = board.find_column(col);
    
    if (!column) {
        throw std::runtime_error("Column not found: " + col);
    }
    
    // Добавление задачи в колонку
    column->add_task(std::move(new_task));
}

// Создание нового разработчика
void create_developer(Board& board, const std::string& name) {
    // Валидация входных параметров
    if (name.empty()) {
        throw std::invalid_argument("Developer name cannot be empty");
    }
    
    // Создание нового разработчика с помощью умного указателя
    auto developer = std::make_unique<Developer>(name);
    
    // Добавление разработчика на доску
    board.add_developer(std::move(developer));
}