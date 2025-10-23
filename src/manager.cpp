#include<manager.h>
#include<iostream>


void create_task(Board &board, std::string col, std::string title){
    Task* new_task = new Task(title);
    for (auto i:board.get_columns()){
        if(col == i->get_name()){
            i->add_task(new_task);
        }
    }
}

void create_developer(Board &board, std::string name){
    Developer* develop = new Developer(name);
    board.add_developer(develop);
}