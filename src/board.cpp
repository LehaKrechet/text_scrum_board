#include <vector>
#include <memory>
#include <algorithm>
#include "board.h"
#include "column.h"
#include <stdexcept>

// Установка названия доски
void Board::set_name(std::string n) {
    name = n;
}

// Получение названия доски
std::string Board::get_name() const {
    return name;
}

// Получение списка колонок (неконстантная версия)
std::vector<std::unique_ptr<Column>>& Board::get_columns() {
    return columns;
}

// Получение списка колонок (константная версия)
const std::vector<std::unique_ptr<Column>>& Board::get_columns() const {
    return columns;
}

// Добавление колонки на доску
void Board::add_column(std::unique_ptr<Column> col) {
    if (!col) {
        throw std::invalid_argument("Column cannot be null");
    }
    columns.push_back(std::move(col));
}

// Очистка всех колонки
void Board::clear_columns() {
    columns.clear();
}

// Получение списка разработчиков (неконстантная версия)
std::vector<std::unique_ptr<Developer>>& Board::get_developers() {
    return developers;
}

// Получение списка разработчиков (константная версия)
const std::vector<std::unique_ptr<Developer>>& Board::get_developers() const {
    return developers;
}

// Добавление разработчика
void Board::add_developer(std::unique_ptr<Developer> develop) {
    if (!develop) {
        throw std::invalid_argument("Developer cannot be null");
    }
    developers.push_back(std::move(develop));  
}

// Очистка списка разработчиков
void Board::clear_developers() {
    developers.clear();
}

// Поиск разработчика по имени
Developer* Board::find_developer(const std::string& name) const {
    // Используем алгоритм find_if для поиска по имени
    auto it = std::find_if(developers.begin(), developers.end(),
        [&](const std::unique_ptr<Developer>& dev) {
            return dev->get_name() == name;
        });
    return it != developers.end() ? it->get() : nullptr;  // Возвращаем указатель или nullptr
}

// Поиск колонки по имени
Column* Board::find_column(const std::string& name) const {
    // Используем алгоритм find_if для поиска по имени
    auto it = std::find_if(columns.begin(), columns.end(),
        [&](const std::unique_ptr<Column>& col) {
            return col->get_name() == name;
        });
    return it != columns.end() ? it->get() : nullptr;  // Возвращаем указатель или nullptr
}