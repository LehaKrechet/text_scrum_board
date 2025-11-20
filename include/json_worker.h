#pragma once

#include <vector>
#include <string>
#include <memory>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;

// Предварительное объявление класса Board
class Board;

// Класс Json_worker отвечает за сериализацию и десериализацию
// состояния Scrum доски в формат JSON
// Использует библиотеку RapidJSON для эффективной работы с JSON
class Json_worker {
private:
    Document doc;                              // DOM-представление JSON документа
    Document::AllocatorType& allocator = doc.GetAllocator();  // Аллокатор для создания JSON значений
    std::string save_path;                     // Путь для сохранения/загрузки файла
    std::vector<std::string> ids;              // Временное хранилище ID задач

public:
    // Конструктор с указанием пути к файлу
    Json_worker(std::string sp) : save_path(sp) {
        doc.SetObject();  // Инициализация документа как JSON объекта
    }
    
    // Основные методы работы с JSON
    
    void save();                                      // Сохранение документа в файл
    void set_save_path(const std::string& path) { save_path = path; }  // Установка пути
    std::string get_save_path() const { return save_path; }            // Получение пути
    Value ids_add(const std::vector<std::string>& id);  // Добавление ID в JSON
    std::vector<std::string> ids_get();               // Получение ID из JSON
    void board_add(const Board& board, Value ids);    // Добавление доски в JSON
    void board_load(Board& board);                    // Загрузка доски из JSON
    void clear_ids();                                 // Очистка временного хранилища ID
    bool is_valid_board_file(const std::string& file_path) const;  // Проверка валидности файла
};