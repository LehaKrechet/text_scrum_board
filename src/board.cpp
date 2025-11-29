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
// Возвращает ссылку для модификации колонок
std::vector<std::unique_ptr<Column>>& Board::get_columns() {
    return columns;
}

// Получение списка колонок (константная версия)
// Возвращает константную ссылку для чтения без модификации
const std::vector<std::unique_ptr<Column>>& Board::get_columns() const {
    return columns;
}

// Добавление колонки на доску
void Board::add_column(std::unique_ptr<Column> col) {
    // Проверяем что колонка не nullptr
    if (!col) {
        throw std::invalid_argument("Column cannot be null");
    }
    // Перемещаем колонку в список колонок доски
    columns.push_back(std::move(col));
}

// Очистка всех колонок
// Удаляет все колонки и их задачи
void Board::clear_columns() {
    columns.clear();
}

// Получение списка разработчиков (неконстантная версия)
// Возвращает ссылку для модификации разработчиков
std::vector<std::unique_ptr<Developer>>& Board::get_developers() {
    return developers;
}

// Получение списка разработчиков (константная версия)
// Возвращает константную ссылку для чтения без модификации
const std::vector<std::unique_ptr<Developer>>& Board::get_developers() const {
    return developers;
}

// Добавление разработчика
void Board::add_developer(std::unique_ptr<Developer> develop) {
    // Проверяем что разработчик не nullptr
    if (!develop) {
        throw std::invalid_argument("Developer cannot be null");
    }
    // Перемещаем разработчика в список разработчиков доски
    developers.push_back(std::move(develop));
}

// Очистка списка разработчиков
// Удаляет всех разработчиков с доски
void Board::clear_developers() {
    developers.clear();
}

// Поиск разработчика по имени
Developer* Board::find_developer(const std::string& name) const {
    // Используем алгоритм find_if для поиска по имени
    // find_if проходит по всем разработчикам и проверяет условие
    auto it = std::find_if(developers.begin(), developers.end(),
        // Лямбда-функция для сравнения имен разработчиков
        [&](const std::unique_ptr<Developer>& dev) {
            return dev->get_name() == name;
        });
    
    // Если разработчик найден, возвращаем указатель
    // Если не найден, возвращаем nullptr
    return it != developers.end() ? it->get() : nullptr;
}

// Поиск колонки по имени
Column* Board::find_column(const std::string& name) const {
    // Используем алгоритм find_if для поиска по имени
    auto it = std::find_if(columns.begin(), columns.end(),
        // Лямбда-функция для сравнения имен колонок
        [&](const std::unique_ptr<Column>& col) {
            return col->get_name() == name;
        });
    
    // Если колонка найдена, возвращаем указатель
    // Если не найдена, возвращаем nullptr
    return it != columns.end() ? it->get() : nullptr;
}

std::vector<std::string> Board::used_ids = {};

// Добавление ID в список использованных
void Board::add_used_id(const std::string& id) {
    used_ids.push_back(id);
}

// Проверка, используется ли ID
bool Board::is_id_used(const std::string& id) {
    return std::find(used_ids.begin(), used_ids.end(), id) != used_ids.end();
}

// Очистка списка использованных ID
void Board::clear_used_ids() {
    used_ids.clear();
}