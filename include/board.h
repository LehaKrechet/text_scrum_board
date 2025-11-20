#pragma once

#include <string>
#include <vector>
#include <memory>
#include "column.h"
#include "developer.h"

// Предварительное объявление класса Column
// Позволяет использовать указатели на Column без включения всего заголовка
class Column;

// Класс Board представляет всю Scrum доску
// Содержит колонки, разработчиков и общую информацию о доске
class Board {
private:
    std::string name;  // Название доски
    std::vector<std::unique_ptr<Column>> columns;      // Список колонок на доске
    std::vector<std::unique_ptr<Developer>> developers; // Список разработчиков команды

public:
    // Конструктор доски с обязательным названием
    // Векторы columns и developers инициализируются по умолчанию пустыми
    Board(std::string n) : name(n) {}
    
    // Методы для работы с названием доски
    void set_name(std::string n);
    std::string get_name() const;
    
    // Методы для работы с колонками
    std::vector<std::unique_ptr<Column>>& get_columns();
    const std::vector<std::unique_ptr<Column>>& get_columns() const;
    void add_column(std::unique_ptr<Column> col);
    void clear_columns();
    
    // Методы для работы с разработчиками
    std::vector<std::unique_ptr<Developer>>& get_developers();
    const std::vector<std::unique_ptr<Developer>>& get_developers() const;
    void add_developer(std::unique_ptr<Developer> develop);
    void clear_developers();
    
    // Методы поиска
    Developer* find_developer(const std::string& name) const;  // Поиск разработчика по имени
    Column* find_column(const std::string& name) const;        // Поиск колонки по имени
};