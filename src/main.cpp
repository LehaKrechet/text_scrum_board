#include <task.h>
// #include <board.h>
#include <column.h>
// #include <ftxui.h>
// #include <scrum_board.h>
#include <iostream>


int main(){
  Task task("Task1"), task1("Task2");


  Column To_do;
  Column In_progress;
  In_progress.name = "In progress";
  To_do.name = "TO DO";

  task.set_description("Hello world");

  std::cout << task.get_description() << std::endl;
  std::cout << task.get_id()<< std::endl;
  std::cout << task.get_title()<< std::endl;

  task.set_title("Hello");
  std::cout << task.get_title()<< std::endl;

}