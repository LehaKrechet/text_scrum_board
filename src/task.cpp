#include <task.h>
#include <column.h>
#include <string>
#include <iostream>
#include <random>
#include <algorithm>
#include <json_worker.h>

std::vector<std::string> ids = {}; //Из json файла 

std::string generate_random_string(int length) {
    const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, charset.size() - 1);
    
    std::string result;
    for (int i = 0; i < length; ++i) {
        result += charset[distribution(generator)];
    }
    return result;
}

std::string Task::generate_id(){
    std::string new_id;
    bool flag = true;
    while (flag){
        new_id = generate_random_string(6);
        if (std::find(ids.begin(), ids.end(), new_id) == ids.end()){
            ids.push_back(new_id);
            flag = false;
        }
    }
    return new_id;

}


void Task::set_description(std::string descript){
    description = descript;
}
std::string Task::get_description(){
    return description;
}
std::string Task::get_id(){
    return id;
}
std::string Task::get_title(){
    return title;
}
void Task::set_title(std::string titl){
    title = titl;
}

int Task::get_prioriy(){
    return priority;
}
void Task::set_priority(int p){
    priority = p;
}
void Task::set_developer(Developer *develop){
    developer = develop;
}
Developer* Task::get_developer(){
    return developer;
}

void Task::set_id(std::string new_id){
    id = new_id;
}
