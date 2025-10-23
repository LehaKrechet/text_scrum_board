#include <string>
#include <column.h>
#include <task.h>
#include <board.h>
#include <algorithm>
#include <stdexcept>

void Column::add_task(Task *task){
    this->tasks.push_back(task);
}

void Column::delete_task(Task *task){
    auto it = std::find(this->tasks.begin(), this->tasks.end(), task);
    if (it != this->tasks.end()){
        this->tasks.erase(it);
        auto id_it = std::find(ids.begin(), ids.end(), task->get_id());
        if (id_it != ids.end()) {
            ids.erase(id_it);
        }
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
Task* search_task(Board &board,std::string col, std::string title){
    for (auto *i: board.get_columns()){
        if (i->get_name() == col){
            for (auto *element: i->get_tasks()){
                if (element->get_title() == title){
                    return element;
                }
            }
        }
    }
    throw std::out_of_range("Not found");
}

