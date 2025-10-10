#pragma once

#include <vector>
#include <task.h> 

class Column {
    public:
        std::string name;
        std::vector<Task> tasks;
        void add_task(Task &task);
        void delete_task(Task &task);

};
void move_task(Column &start, Column &end, Task &task);