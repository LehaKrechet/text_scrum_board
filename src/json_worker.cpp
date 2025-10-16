#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>
#include <fstream>
#include <json_worker.h>
#include <board.h>

using namespace rapidjson;

void test(){
    Document doc;
    doc.SetObject();

    Document::AllocatorType& allocator = doc.GetAllocator();

    doc.AddMember("name", "Alex", allocator);
    doc.AddMember("old", "20", allocator);

    //масиив
    Value hobbies(kArrayType);
    hobbies.PushBack("PC", allocator);
    hobbies.PushBack("VR", allocator);
    doc.AddMember("hobies", hobbies, allocator);

    Value addres2(kObjectType);

    addres2.AddMember("street", "123", allocator);
    addres2.AddMember("city", "New", allocator);
    doc.AddMember("addres2", addres2, allocator);

    // Value addres(kObjectType);

    // addres.AddMember("street", addres2, allocator);
    // addres.AddMember("city", "New", allocator);
    // doc.AddMember("addres", addres, allocator);
}

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


// std::vector<std::string> Json_worker::ids_get(){

// }