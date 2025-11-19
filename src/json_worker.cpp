#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include "json_worker.h"
#include "board.h"
#include "task.h"

using namespace rapidjson;

// Сохранение JSON документа в файл
void Json_worker::save() {
    // Создание буфера и писателя для форматированного вывода
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    doc.Accept(writer);

    // Запись в файл
    std::ofstream file(save_path);
    if (file.is_open()) {
        file << buffer.GetString();
        file.close();
        std::cout << "Board saved successfully to: " << save_path << std::endl;
    } else {
        throw std::runtime_error("Cannot open file for writing: " + save_path);
    }
}

// Создание JSON массива из вектора ID задач
Value Json_worker::ids_add(const std::vector<std::string>& id) {
    Value ids_array(kArrayType);
    
    // Добавление каждого ID в JSON массив
    for (const auto& elem : id) {
        Value id_value;
        id_value.SetString(elem.c_str(), static_cast<rapidjson::SizeType>(elem.length()), allocator);
        ids_array.PushBack(id_value, allocator);
    }
    return ids_array;
}

// Добавления всей доски в JSON формат
void Json_worker::board_add(const Board& board, Value ids) {
    Value board_json(kObjectType);  // Основной объект доски
    Value developers_json(kArrayType);  // Массив разработчиков
    
    // Добавление названия доски
    std::string current_board_name = board.get_name();
    Value board_name;
    board_name.SetString(current_board_name.c_str(), current_board_name.length(), allocator);

    // Добавление ID задач
    board_json.AddMember("ids", ids, allocator);

    // Добавление разработчиков
    for (const auto& d : board.get_developers()) {
        developers_json.PushBack(Value(d->get_name().c_str(), allocator), allocator);
    }
    board_json.AddMember("developers", developers_json, allocator);
    
    // Добавление колонок и задач
    for (const auto& column_ptr : board.get_columns()) {
        Value tasks_json(kObjectType);  // Объект для задач колонки
        Value column_name;
        column_name.SetString(column_ptr->get_name().c_str(), column_ptr->get_name().length(), allocator);
        
        // Добавление каждой задачи в колонке
        for (const auto& task_ptr : column_ptr->get_tasks()) {
            Value task_data(kObjectType);  // Объект с данными задачи
            
            // Добавление полей задачи
            task_data.AddMember("description", Value(task_ptr->get_description().c_str(), allocator), allocator);
            task_data.AddMember("id", Value(task_ptr->get_id().c_str(), allocator), allocator);
            task_data.AddMember("priority", task_ptr->get_priority(), allocator);
            
            // Добавление разработчика (или "Unassigned")
            if (task_ptr->get_developer()) {
                task_data.AddMember("developer", Value(task_ptr->get_developer()->get_name().c_str(), allocator), allocator);
            } else {
                task_data.AddMember("developer", Value("Unassigned", allocator), allocator);
            }

            // Использование заголовка задачи как ключа в JSON объекте
            Value task_title;
            task_title.SetString(task_ptr->get_title().c_str(), static_cast<rapidjson::SizeType>(task_ptr->get_title().length()), allocator);
            tasks_json.AddMember(task_title, task_data, allocator);
        }
        // Добавление колонки в основной объект доски
        board_json.AddMember(column_name, tasks_json, allocator);
    }
    
    // Добавление всей доски в документ
    doc.AddMember(board_name, board_json, allocator);
}

// Получение ID задач из JSON файла
std::vector<std::string> Json_worker::ids_get() {
    std::vector<std::string> result;
    
    // Чтение файла
    std::ifstream file(save_path);
    if (!file.is_open()) {
        std::cout << "Cannot open file: " << save_path << std::endl;
        return result;
    }
    
    // Загрузка всего содержимого файла в буфер
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    // Парсинг JSON
    Document temp_doc;
    temp_doc.Parse(buffer.str().c_str());
    
    if (temp_doc.HasParseError()) {
        std::cout << "JSON parse error: " << temp_doc.GetParseError() << std::endl;
        return result;
    }
    
    // Извлечение ID из JSON структуры
    if (temp_doc.MemberBegin() != temp_doc.MemberEnd()) {
        const std::string board_name = temp_doc.MemberBegin()->name.GetString();
        const Value& board_obj = temp_doc[board_name.c_str()];
        
        if (board_obj.HasMember("ids") && board_obj["ids"].IsArray()) {
            const Value& ids_array = board_obj["ids"];
            for (SizeType i = 0; i < ids_array.Size(); i++) {
                if (ids_array[i].IsString()) {
                    result.push_back(ids_array[i].GetString());
                }
            }
            std::cout << "Loaded " << result.size() << " IDs from board: " << board_name << std::endl;
        }
    }
    
    return result;
}

// Загрузка доски из JSON файла
void Json_worker::board_load(Board& board) {
    // Чтение файла
    std::ifstream file(save_path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + save_path);
    }
    
    // Загрузка содержимого файла
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    // Парсинг JSON
    Document temp_doc;
    temp_doc.Parse(buffer.str().c_str());
    
    if (temp_doc.HasParseError()) {
        throw std::runtime_error("Invalid JSON format in file: " + save_path);
    }
    
    if (temp_doc.MemberBegin() == temp_doc.MemberEnd()) {
        throw std::runtime_error("No boards found in JSON file");
    }
    
    // Извлечение данных основной доски
    const std::string board_name = temp_doc.MemberBegin()->name.GetString();
    const Value& board_obj = temp_doc[board_name.c_str()];
    
    // Очистка существующих данных доски
    board.clear_columns();
    board.clear_developers();
    Task::clear_used_ids();
    ids.clear();
    
    // Загрузка разработчиков
    if (board_obj.HasMember("developers") && board_obj["developers"].IsArray()) {
        const Value& developers_array = board_obj["developers"];
        for (SizeType i = 0; i < developers_array.Size(); i++) {
            if (developers_array[i].IsString()) {
                std::string dev_name = developers_array[i].GetString();
                auto developer = std::make_unique<Developer>(dev_name);
                board.add_developer(std::move(developer));
            }
        }
    }
    
    // Загрузка ID задач
    if (board_obj.HasMember("ids") && board_obj["ids"].IsArray()) {
        const Value& ids_array = board_obj["ids"];
        for (SizeType i = 0; i < ids_array.Size(); i++) {
            if (ids_array[i].IsString()) {
                std::string id = ids_array[i].GetString();
                if (std::find(ids.begin(), ids.end(), id) == ids.end()) {
                    ids.push_back(id);
                }
            }
        }
    }
    
    // Загрузка колонок и задач
    for (Value::ConstMemberIterator itr = board_obj.MemberBegin(); 
         itr != board_obj.MemberEnd(); ++itr) {
        
        std::string field_name = itr->name.GetString();
        // Пропускаем служебные поля
        if (field_name == "developers" || field_name == "ids") {
            continue;
        }
        
        // Создание колонки
        std::string column_name = itr->name.GetString();
        auto column = std::make_unique<Column>(column_name);
        
        if (itr->value.IsObject()) {
            const Value& tasks_obj = itr->value;
            
            // Загрузка задач в колонку
            for (Value::ConstMemberIterator task_itr = tasks_obj.MemberBegin();
                 task_itr != tasks_obj.MemberEnd(); ++task_itr) {
                
                std::string task_title = task_itr->name.GetString();
                const Value& task_data = task_itr->value;
                
                // Создание задачи
                auto task = std::make_unique<Task>(task_title);
                
                // Загрузка полей задачи
                if (task_data.HasMember("description") && task_data["description"].IsString()) {
                    task->set_description(task_data["description"].GetString());
                }
                
                if (task_data.HasMember("priority") && task_data["priority"].IsInt()) {
                    task->set_priority(task_data["priority"].GetInt());
                }
                
                if (task_data.HasMember("id") && task_data["id"].IsString()) {
                    task->set_id(task_data["id"].GetString());
                }
                
                // Назначение разработчика
                if (task_data.HasMember("developer") && task_data["developer"].IsString()) {
                    std::string dev_name = task_data["developer"].GetString();
                    if (dev_name != "Unassigned") {
                        Developer* dev = board.find_developer(dev_name);
                        if (dev) {
                            task->set_developer(dev);
                        }
                    }
                }
                
                // Добавление задачи в колонку
                column->add_task(std::move(task));
            }
        }
        
        // Добавление колонки на доску
        board.add_column(std::move(column));
    }
    
    std::cout << "Board loaded successfully from file: " << save_path << std::endl;
}

// Очистка временного хранилища ID
void Json_worker::clear_ids() {
    ids.clear();
}

// Проверка валидности файла доски
bool Json_worker::is_valid_board_file(const std::string& file_path) const {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cout << "Cannot open file: " << file_path << std::endl;
        return false;
    }
    
    // Чтение и парсинг файла
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    Document temp_doc;
    temp_doc.Parse(buffer.str().c_str());
    
    if (temp_doc.HasParseError()) {
        std::cout << "Invalid JSON format" << std::endl;
        return false;
    }
    
    // Проверка структуры JSON
    if (!temp_doc.IsObject()) {
        std::cout << "Root element is not an object" << std::endl;
        return false;
    }
    
    if (temp_doc.MemberBegin() == temp_doc.MemberEnd()) {
        std::cout << "No board found in file" << std::endl;
        return false;
    }
    
    const std::string board_name = temp_doc.MemberBegin()->name.GetString();
    const Value& board_obj = temp_doc[board_name.c_str()];
    
    if (!board_obj.IsObject()) {
        std::cout << "Board data is not an object" << std::endl;
        return false;
    }
    
    // Проверка наличия обязательной структуры
    bool has_valid_structure = false;
    
    for (Value::ConstMemberIterator itr = board_obj.MemberBegin(); 
         itr != board_obj.MemberEnd(); ++itr) {
        std::string field_name = itr->name.GetString();
        
        // Проверяем наличие стандартных полей или колонок с задачами
        if (field_name == "developers" || field_name == "ids") {
            has_valid_structure = true;
            break;
        }
        
        if (itr->value.IsObject()) {
            const Value& tasks_obj = itr->value;
            if (tasks_obj.MemberBegin() != tasks_obj.MemberEnd()) {
                has_valid_structure = true;
                break;
            }
        }
    }
    
    return has_valid_structure;
}