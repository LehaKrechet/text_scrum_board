#include <string>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include "column.h"
#include "task.h"
#include "board.h"

// Добавление задачи в колонку
void Column::add_task(std::unique_ptr<Task> task) {
    // Проверяем что задача не nullptr (защита от ошибок)
    if (!task) {
        throw std::invalid_argument("Task cannot be null");
    }
    // Перемещаем задачу в конец списка задач колонки
    // std::move необходим потому что unique_ptr нельзя копировать
    this->tasks.push_back(std::move(task)); 
}

// Удаление задачи из колонки по заголовку
void Column::delete_task(const std::string& task_title) {
    // Поиск задачи по заголовку используя алгоритм find_if
    // find_if проходит по всем элементам контейнера и проверяет условие
    auto it = std::find_if(this->tasks.begin(), this->tasks.end(),
        // Лямбда-функция для сравнения заголовка задачи
        // [&] - захват всех переменных из внешней области по ссылке
        [&](const std::unique_ptr<Task>& task) {
            // Сравниваем заголовок текущей задачи с искомым
            return task->get_title() == task_title;
        });
    
    // Если задача найдена (итератор не указывает на конец контейнера)
    if (it != this->tasks.end()) {
        // Удаляем задачу из вектора
        // unique_ptr автоматически освободит память при удалении
        this->tasks.erase(it);
    } else {
        // Если задача не найдена, бросаем исключение
        throw std::runtime_error("Task not found: " + task_title);
    }
}

// Получение списка задач в колонке (неконстантная версия)
// Возвращает ссылку на вектор задач для модификации
std::vector<std::unique_ptr<Task>>& Column::get_tasks() {
    return tasks;
}

// Получение списка задач в колонке (константная версия)
// Возвращает константную ссылку для чтения без модификации
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
    // Используем алгоритм find_if для поиска задачи
    auto it = std::find_if(tasks.begin(), tasks.end(),
        // Лямбда-функция для проверки заголовка каждой задачи
        [&](const std::unique_ptr<Task>& task) {
            return task->get_title() == title;
        });
    
    // Если задача найдена, возвращаем указатель
    // Если не найдена, возвращаем nullptr
    return it != tasks.end() ? it->get() : nullptr;
}

// Перемещение задачи между колонками
void move_task(Column* start, Column* end, Task* task) {
    // Проверка валидности входных параметров
    // Защита от nullptr который может привести к падению программы
    if (!start || !end || !task) {
        throw std::invalid_argument("Columns and task cannot be null");
    }
    
    // Получаем ссылку на список задач исходной колонки
    auto& start_tasks = start->get_tasks();
    
    // Ищем задачу в исходной колонке по указателю
    // Сравниваем именно по указателю, а не по содержимому
    auto it = std::find_if(start_tasks.begin(), start_tasks.end(),
        // Лямбда-функция для сравнения указателей
        [&](const std::unique_ptr<Task>& t) {
            // Сравниваем указатель задачи с искомым указателем
            return t.get() == task;
        });
    
    // Если задача найдена в исходной колонке
    if (it != start_tasks.end()) {
        // Перемещаем владение задачей с помощью std::move
        // std::move необходим потому что unique_ptr нельзя копировать
        // После этого it становится невалидным, поэтому сохраняем задачу
        auto task_ptr = std::move(*it);
        
        // Удаляем задачу из исходной колонки
        // Теперь задача находится в task_ptr и готова к перемещению
        start_tasks.erase(it);
        
        // Добавляем задачу в целевую колонку, передавая владение
        end->add_task(std::move(task_ptr));
    } else {
        // Если задача не найдена в исходной колонке
        throw std::runtime_error("Task not found in source column");
    }
}

// Поиск задачи на всей доске по названию колонки и заголовку задачи
Task* search_task(Board& board, const std::string& col, const std::string& title) {
    // Поиск колонки по имени на доске
    Column* column = board.find_column(col);
    if (!column) {
        // Если колонка не найдена, бросаем исключение с информацией
        throw std::runtime_error("Column not found: " + col);
    }
    
    // Поиск задачи в найденной колонке по заголовку
    Task* task = column->find_task(title);
    if (!task) {
        // Если задача не найдена, бросаем исключение с детальной информацией
        throw std::runtime_error("Task not found: " + title + " in column: " + col);
    }
    
    // Возвращаем указатель на найденную задачу
    return task;
}