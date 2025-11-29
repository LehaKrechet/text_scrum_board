#include "task.h"
#include "board.h"
#include <string>
#include <random>
#include <algorithm>
#include <stdexcept>


// Генерация случайной строки заданной длины
// Используется для создания уникальных идентификаторов задач
std::string Task::generate_random_string(int length) {
    // Набор символов для генерации ID
    // Используем цифры и буквы в обоих регистрах для большего разнообразия
    // Исключаем похожие символы типа 0/O, 1/I для избежания путаницы
    const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    // Инициализация генератора случайных чисел
    // std::random_device - источник энтропии (аппаратный генератор если доступен)
    std::random_device rd;
    // std::mt19937 - вихрь Мерсенна, качественный псевдослучайный генератор
    std::mt19937 generator(rd());
    // Равномерное распределение для индексов в charset
    // От 0 до размера charset - 1
    std::uniform_int_distribution<int> distribution(0, charset.size() - 1);
    
    // Генерация строки заданной длины
    std::string result;
    for (int i = 0; i < length; ++i) {
        // Добавление случайного символа из charset
        // distribution(generator) генерирует случайный индекс
        result += charset[distribution(generator)];
    }
    return result;
}




// Конструктор задачи
// Создает задачу с обязательным заголовком и автоматически генерирует ID
Task::Task(std::string titl, Board& board) : 
    title(titl),
    description(""),
    priority(0),
    developer(nullptr) {
    
    // Генерация уникального ID с использованием Board
    std::string new_id;
    bool unique_found = false;
    const int max_attempts = 100;
    int attempts = 0;
    
    while (!unique_found && attempts < max_attempts) {
        new_id = generate_random_string(6);
        
        // Используем Board для проверки уникальности
        if (!Board::is_id_used(new_id)) {
            Board::add_used_id(new_id);
            unique_found = true;
        }
        attempts++;
    }
    
    if (!unique_found) {
        throw std::runtime_error("Failed to generate unique task ID");
    }
    
    id = new_id;
}
// Установка описания задачи
void Task::set_description(std::string descript) {
    description = descript;
}

// Получение описания задачи
std::string Task::get_description() const {
    return description;
}

// Получение ID задачи
std::string Task::get_id() const {
    return id;
}

// Получение заголовка задачи
std::string Task::get_title() const {
    return title;
}

// Установка заголовка задачи
void Task::set_title(std::string titl) {
    title = titl;
}

// Получение приоритета задачи
int Task::get_priority() const {
    return priority;
}

// Установка приоритета задачи с валидацией
void Task::set_priority(int p) {
    // Проверяем что приоритет в допустимом диапазоне
    if (p < 0 || p > 10) {
        throw std::invalid_argument("Priority must be between 0 and 10");
    }
    priority = p;
}

// Назначение разработчика на задачу
void Task::set_developer(Developer* develop) {
    developer = develop;
}

// Получение разработчика, назначенного на задачу
Developer* Task::get_developer() const {
    return developer;
}

// Установка ID задачи вручную (с валидацией)
void Task::set_id(std::string new_id) {
    // Проверяем что ID не пустой
    if (new_id.empty()) {
        throw std::invalid_argument("Task ID cannot be empty");
    }
    id = new_id;
}