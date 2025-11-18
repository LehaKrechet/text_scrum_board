#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>
#include <fstream>
#include <json_worker.h>
#include <board.h>
#include <sstream>
#include <algorithm>
#include <memory>
#include <task.h>

using namespace rapidjson;



void Json_worker::save(){
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::ofstream file(save_path);
    if (file.is_open()){
      file << buffer.GetString();
      file.close();
      std::cout << "Good";
    }
}
Value Json_worker::ids_add(std::vector<std::string> id){
  Value ids(kArrayType);
  Value id_value;
  for(auto &elem : id){
    id_value.SetString(elem.c_str(), static_cast<rapidjson::SizeType>(elem.length()), allocator);
    ids.PushBack(id_value, allocator);
  }
  return ids;

}
void Json_worker::board_add(Board board, Value ids){
  Value board_json(kObjectType);
  Value developers_json(kArrayType);
  Value board_name;
  board_name.SetString(board.get_name().c_str(), board.get_name().length(), allocator);

  board_json.AddMember("ids", ids, allocator);

  for (auto *d: board.get_developer()){
    developers_json.PushBack(Value(d->get_name().c_str(), allocator), allocator);
  }
  board_json.AddMember("developers", developers_json, allocator);
  for (auto *i: board.get_columns()){
    Value tasks_json(kObjectType);
    Value Column_name;
    Value task_title;

    
    Column_name.SetString(i->get_name().c_str(), i->get_name().length(), allocator);
    
    for (auto *j: i->get_tasks()){
      Value task_pole(kObjectType);
      task_pole.AddMember("description", Value(j->get_description().c_str(), allocator), allocator);
      task_pole.AddMember("id",Value(j->get_id().c_str(), allocator),allocator);
      task_pole.AddMember("priority",j->get_prioriy(),allocator);
      task_pole.AddMember("developer", Value(j->get_developer()->get_name().c_str(), allocator), allocator);

      
      task_title.SetString(j->get_title().c_str(), static_cast<rapidjson::SizeType>(j->get_title().length()), allocator);
      tasks_json.AddMember(task_title, task_pole, allocator);
    }
    board_json.AddMember(Column_name, tasks_json, allocator);
  }
  
  doc.AddMember(board_name, board_json, allocator);
}


std::vector<std::string> Json_worker::ids_get(){
    std::vector<std::string> result;
    
    std::ifstream file(save_path);
    if (!file.is_open()) {
        std::cout << "Cannot open file: " << save_path << std::endl;
        return result;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    Document temp_doc;
    temp_doc.Parse(buffer.str().c_str());
    
    // Проверяем успешность парсинга
    if (temp_doc.HasParseError()) {
        std::cout << "JSON parse error: " << temp_doc.GetParseError() << std::endl;
        return result;
    }
    
    // Получаем первый ключ корневого объекта
    if (temp_doc.MemberBegin() != temp_doc.MemberEnd()) {
        const std::string board_name = temp_doc.MemberBegin()->name.GetString();
        const Value& board_obj = temp_doc[board_name.c_str()];
        
        // Теперь ищем "ids" внутри объекта доски
        if (board_obj.HasMember("ids") && board_obj["ids"].IsArray()) {
            const Value& ids_array = board_obj["ids"];
            for (SizeType i = 0; i < ids_array.Size(); i++) {
                if (ids_array[i].IsString()) {
                    result.push_back(ids_array[i].GetString());
                }
            }
            std::cout << "Loaded " << result.size() << " IDs from board: " << board_name << std::endl;
        } else {
            std::cout << "No 'ids' array found in board: " << board_name << std::endl;
        }
    } else {
        std::cout << "No boards found in JSON" << std::endl;
    }
    
    return result;
}




void Json_worker::board_load(Board& board) {
    std::ifstream file(save_path);
    if (!file.is_open()) {
        std::cout << "Cannot open file: " << save_path << std::endl;
        throw std::runtime_error("Cannot open file");
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    Document temp_doc;
    temp_doc.Parse(buffer.str().c_str());
    
    if (temp_doc.HasParseError()) {
        std::cout << "JSON parse error: " << temp_doc.GetParseError() << std::endl;
        throw std::runtime_error("Invalid JSON format");
    }
    
    
    // Получаем первую доску из JSON
    if (temp_doc.MemberBegin() == temp_doc.MemberEnd()) {
        std::cout << "No boards found in JSON" << std::endl;
        return;
    }
    
    const std::string board_name = temp_doc.MemberBegin()->name.GetString();
    const Value& board_obj = temp_doc[board_name.c_str()];
    
    // Устанавливаем имя доски
    board.set_name(board_name);
    
    for (Column* col : board.get_columns()) {
        delete col;
    }
    board.get_columns().clear();
    
    for (Developer* dev : board.get_developer()) {
        delete dev;
    }
    board.get_developer().clear();
    ids.clear();
    
    // Восстанавливаем developers
    if (board_obj.HasMember("developers") && board_obj["developers"].IsArray()) {
        const Value& developers_array = board_obj["developers"];
        for (SizeType i = 0; i < developers_array.Size(); i++) {
            if (developers_array[i].IsString()) {
                std::string dev_name = developers_array[i].GetString();
                Developer* developer = new Developer(dev_name);
                board.add_developer(developer);
            }
        }
    }
    
    // Восстанавливаем ids в глобальный вектор
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
    
    // Восстанавливаем колонки и задачи
    for (Value::ConstMemberIterator itr = board_obj.MemberBegin(); 
         itr != board_obj.MemberEnd(); ++itr) {
        
        // Пропускаем уже обработанные поля
        if (std::string(itr->name.GetString()) == "developers" || 
            std::string(itr->name.GetString()) == "ids") {
            continue;
        }
        
        // Создаем колонку
        std::string column_name = itr->name.GetString();
        Column* column = new Column(column_name);
        
        // Восстанавливаем задачи в колонке
        if (itr->value.IsObject()) {
            const Value& tasks_obj = itr->value;
            
            for (Value::ConstMemberIterator task_itr = tasks_obj.MemberBegin();
                 task_itr != tasks_obj.MemberEnd(); ++task_itr) {
                
                std::string task_title = task_itr->name.GetString();
                const Value& task_data = task_itr->value;
                
                // Создаем задачу
                Task* task = new Task(task_title);
                
                // Восстанавливаем поля задачи
                if (task_data.HasMember("description") && task_data["description"].IsString()) {
                    task->set_description(task_data["description"].GetString());
                }
                
                if (task_data.HasMember("priority") && task_data["priority"].IsInt()) {
                    task->set_priority(task_data["priority"].GetInt());
                }
                
                // Восстанавливаем разработчика
                if (task_data.HasMember("developer") && task_data["developer"].IsString()) {
                    std::string dev_name = task_data["developer"].GetString();
                    
                    // Ищем разработчика в доске
                    for (Developer* dev : board.get_developer()) {
                        if (dev->get_name() == dev_name) {
                            task->set_developer(dev);
                            break;
                        }
                    }
                }
                
                // Добавляем задачу в колонку
                column->add_task(task);
            }
        }
        
        // Добавляем колонку в доску
        board.add_column(column);
    }
    
    std::cout << "Board '" << board_name << "' loaded successfully!" << std::endl;
}

void Json_worker::clear_ids() {
    ids.clear();
}
bool Json_worker::is_valid_board_file(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cout << "Cannot open file: " << file_path << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    Document temp_doc;
    temp_doc.Parse(buffer.str().c_str());
    
    // Проверяем успешность парсинга
    if (temp_doc.HasParseError()) {
        std::cout << "Invalid JSON format" << std::endl;
        return false;
    }
    
    // Проверяем что это объект
    if (!temp_doc.IsObject()) {
        std::cout << "Root element is not an object" << std::endl;
        return false;
    }
    
    // Проверяем что есть хотя бы один член (имя доски)
    if (temp_doc.MemberBegin() == temp_doc.MemberEnd()) {
        std::cout << "No board found in file" << std::endl;
        return false;
    }
    
    // Проверяем структуру доски
    const std::string board_name = temp_doc.MemberBegin()->name.GetString();
    const Value& board_obj = temp_doc[board_name.c_str()];
    
    if (!board_obj.IsObject()) {
        std::cout << "Board data is not an object" << std::endl;
        return false;
    }
    
    // Проверяем наличие обязательных полей или структуры
    bool has_valid_structure = false;
    
    // Проверяем наличие колонок или developers
    for (Value::ConstMemberIterator itr = board_obj.MemberBegin(); 
         itr != board_obj.MemberEnd(); ++itr) {
        std::string field_name = itr->name.GetString();
        
        // Если есть поле developers или ids, считаем файл валидным
        if (field_name == "developers" || field_name == "ids") {
            has_valid_structure = true;
            break;
        }
        
        // Или если есть хотя бы одна колонка с задачами
        if (itr->value.IsObject()) {
            const Value& tasks_obj = itr->value;
            if (tasks_obj.MemberBegin() != tasks_obj.MemberEnd()) {
                has_valid_structure = true;
                break;
            }
        }
    }
    
    if (!has_valid_structure) {
        std::cout << "File doesn't contain valid board structure" << std::endl;
    }
    
    return has_valid_structure;
}