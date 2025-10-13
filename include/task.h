#pragma once

#include <string>
#include <vector>
class Task;

extern std::vector<std::string> ids;

class Task{

    std::string description;
    std::string id;
    std::string title;
    int priority;
    std::string status;

    public:
        Task(std::string titl){
            title = titl;
            id = generate_id();
            description = "";
        }
        Task(const Task &other){
            description = other.description;
            id = other.id;
            title = other.title;
            priority = other.priority;
        }
        std::string generate_id();
        bool operator==(const Task& other) const{
            return title == other.title && description == other.description && id == other.id;
        }
        void set_description(std::string descript);
        std::string get_description();
        std::string get_id();
        std::string get_title();
        void set_title(std::string titl);
        int get_prioriy();
        void set_priority(int p);
};