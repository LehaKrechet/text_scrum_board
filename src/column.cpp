#include <string>
#include <column.h>
#include <task.h>
#include <algorithm>

void Column::add_task(Task *task){
    this->tasks.push_back(task);
}

void Column::delete_task(Task *task){
    auto it = std::find(this->tasks.begin(), this->tasks.end(), task);
    if (it != this->tasks.end()){
        this->tasks.erase(it);
    }
}

void move_task(Column *start, Column *end, Task *task){
    end->add_task(task);
    start->delete_task(task);
}

std::vector<Task*>& Column::get_tasks(){
    return tasks;
}
const std::vector<Task*>& Column::get_tasks() const {
        return tasks;
    }
std::string Column::get_name(){
    return name;
}
void Column::set_name(std::string n){
    name = n;
}