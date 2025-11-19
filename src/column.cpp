#include <string>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include "column.h"
#include "task.h"
#include "board.h"

// Добавление задачи в колонку
void Column::add_task(std::unique_ptr<Task> task) {
    if (!task) {
        throw std::invalid_argument("Task cannot be null");
    }
    this->tasks.push_back(std::move(task)); 
}

// Удаление задачи из колонки по заголовку
void Column::delete_task(const std::string& task_title) {
    // Поиск задачи по заголовку
    auto it = std::find_if(this->tasks.begin(), this->tasks.end(),
        [&](const std::unique_ptr<Task>& task) {
            return task->get_title() == task_title;
        });
    
    // Если задача найдена - удаляем ее
    if (it != this->tasks.end()) {
        this->tasks.erase(it);
    } else {
        throw std::runtime_error("Task not found: " + task_title);
    }
}

// Получение списка задач в колонке (неконстантная версия)
std::vector<std::unique_ptr<Task>>& Column::get_tasks() {
    return tasks;
}

// Получение списка задач в колонке (константная версия)
const std::vector<std::unique_ptr<Task>>& Column::get_tasks() const {
    return tasks;
}

// Получение названия колонки
std::string Column::get_name() const {
    return name;
}

// Установка названия колонки
void Column::set_name(std::string n) {
    name = n;
}

// Поиск задачи в колонке по заголовку
Task* Column::find_task(const std::string& title) const {
    auto it = std::find_if(tasks.begin(), tasks.end(),
        [&](const std::unique_ptr<Task>& task) {
            return task->get_title() == title;
        });
    return it != tasks.end() ? it->get() : nullptr;  // Возвращаем указатель или nullptr
}

// Перемещение задачи между колонками
void move_task(Column* start, Column* end, Task* task) {
    // Проверка валидности входных параметров
    if (!start || !end || !task) {
        throw std::invalid_argument("Columns and task cannot be null");
    }
    
    // Поиск задачи в исходной колонке
    auto& start_tasks = start->get_tasks();
    auto it = std::find_if(start_tasks.begin(), start_tasks.end(),
        [&](const std::unique_ptr<Task>& t) {
            return t.get() == task;  // Сравниваем по указателю
        });
    
    // Если задача найдена - перемещаем ее
    if (it != start_tasks.end()) {
        // Перемещаем владение задачей
        auto task_ptr = std::move(*it);
        start_tasks.erase(it);  // Удаляем из исходной колонки
        end->add_task(std::move(task_ptr));  // Добавляем в целевую колонку
    } else {
        throw std::runtime_error("Task not found in source column");
    }
}

// Поиск задачи на всей доске по названию колонки и заголовку задачи
Task* search_task(Board& board, const std::string& col, const std::string& title) {
    // Поиск колонки по имени
    Column* column = board.find_column(col);
    if (!column) {
        throw std::runtime_error("Column not found: " + col);
    }
    
    // Поиск задачи в колонке
    Task* task = column->find_task(title);
    if (!task) {
        throw std::runtime_error("Task not found: " + title + " in column: " + col);
    }
    
    return task;
}