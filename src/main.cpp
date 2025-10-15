#include <task.h>
#include <board.h>
#include <column.h>
// #include <ftxui.h>
// #include <scrum_board.h>
#include <iostream>


int main(){
  Board board("Number_1");
  Task *task = new Task("Task1"), *task1 = new Task("Task2");


  Column* To_do = new Column("TO_DO");
  Column* In_progress = new Column("In_progress");

  Task *task2 = new Task("Task3");

  task->set_description("Hello world");
  

  In_progress->add_task(task1);
  

  board.add_column(To_do);
  board.add_column(In_progress);

  To_do->add_task(task);
  move_task(To_do, In_progress, task);
  move_task(In_progress, To_do, task1);
  task1->set_title("Hello");
  In_progress->add_task(task2);
  move_task(To_do, In_progress, task1);
  In_progress->delete_task(task2);
  To_do->add_task(task2);
  
  

  for (Task *element : To_do->get_tasks()){
    std::cout << element->get_title() << " ";
  }

  std::cout << std::endl;
  for (Column *element : board.get_columns()){
    std::cout << element->get_name() << " ";
    for (Task *elem : element->get_tasks()){
      std::cout << elem->get_title() << " ";
    }
  }

  delete To_do;
  delete In_progress;


}