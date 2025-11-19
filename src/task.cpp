#include "task.h"
#include <string>
#include <random>
#include <algorithm>
#include <stdexcept>

// Инициализация списка использованных ID
std::vector<std::string> Task::used_ids = {};

// Генерация случайной строки заданной длины
// Используется для создания уникальных идентификаторов задач
std::string Task::generate_random_string(int length) {
    // Набор символов для генерации ID (цифры и буквы в обоих регистрах)
    const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    // Инициализация генератора случайных чисел
    std::random_device rd;  // Источник энтропии
    std::mt19937 generator(rd());  // Вихрь Мерсенна как генератор
    std::uniform_int_distribution<int> distribution(0, charset.size() - 1);  // Равномерное распределение
    
    // Генерация строки заданной длины
    std::string result;
    for (int i = 0; i < length; ++i) {
        result += charset[distribution(generator)];  // Добавление случайного символа
    }
    return result;
}

// Генерация уникального ID для задачи
std::string Task::generate_id() {
    std::string new_id;
    bool unique_found = false;
    const int max_attempts = 100;  // Максимальное количество попыток генерации
    int attempts = 0;
    
    // Попытка найти уникальный ID
    while (!unique_found && attempts < max_attempts) {
        new_id = generate_random_string(6);  // Генерация ID длиной 6 символов
        
        // Проверка уникальности ID
        if (std::find(used_ids.begin(), used_ids.end(), new_id) == used_ids.end()) {
            used_ids.push_back(new_id);  // Добавление в список использованных
            unique_found = true;         // Уникальный ID найден
        }
        attempts++;
    }
    
    // Если не удалось сгенерировать уникальный ID за максимальное число попыток
    if (!unique_found) {
        throw std::runtime_error("Failed to generate unique task ID");
    }
    
    return new_id;
}

// Очистка списка использованных ID
// Полезно при загрузке новой доски или сбросе состояния
void Task::clear_used_ids() {
    used_ids.clear();
}

// Конструктор задачи
// Создает задачу с обязательным заголовком и автоматически генерирует ID
Task::Task(std::string titl) : 
    title(titl), 
    id(generate_id()),      // Автоматическая генерация уникального ID
    description(""),        // Пустое описание по умолчанию
    priority(0),            // Приоритет 0 по умолчанию
    developer(nullptr) {}   // Разработчик не назначен по умолчанию

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
    if (new_id.empty()) {
        throw std::invalid_argument("Task ID cannot be empty");
    }
    id = new_id;
}