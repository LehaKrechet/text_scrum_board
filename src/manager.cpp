#include "manager.h"
#include <iostream>
#include <stdexcept>

void create_task(Board& board, const std::string& col, const std::string& title) {
    if (title.empty()) {
        throw std::invalid_argument("Task title cannot be empty");
    }
    
    // Передаем board в конструктор Task
    auto new_task = std::make_unique<Task>(title, board);
    
    Column* column = board.find_column(col);
    
    if (!column) {
        throw std::runtime_error("Column not found: " + col);
    }
    
    column->add_task(std::move(new_task));
}

void create_developer(Board& board, const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Developer name cannot be empty");
    }
    
    auto developer = std::make_unique<Developer>(name);
    board.add_developer(std::move(developer));
}