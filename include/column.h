#pragma once

#include <vector>
#include <task.h> 
#include <string>
#include <board.h>

class Task;
class Board;

class Column {
    private:
        std::string name;
        std::vector<Task*> tasks;

    public:
        Column(std::string n){
            name = n;
            tasks = {};
        }
        void add_task(Task *task);
        void delete_task(Task *task);
        std::vector<Task*>& get_tasks();
        const std::vector<Task*>& get_tasks() const;

        std::string get_name();
        void set_name(std::string n);
        bool operator==(const Column& other) const {
            return name == other.name;
        }

};
void move_task(Column *start, Column *end, Task *task);
Task* search_task(Board &board, std::string col ,std::string title);