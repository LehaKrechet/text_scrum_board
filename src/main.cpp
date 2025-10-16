#include <task.h>
#include <board.h>
#include <column.h>
#include <developer.h>
#include <json_worker.h>
// #include <ftxui.h>
// #include <scrum_board.h>
#include <iostream>


int main(){
  Board board("Number_1");
  Task *task = new Task("Task1"), *task1 = new Task("Task2"), *task4 = new Task("Task4");
  Developer* develop = new Developer("Alex");
  Developer* develop1 = new Developer("Blala");
  Developer* develop2 = new Developer("lopuh");


  Column* Done = new Column("Done");
  Column* In_progress = new Column("In_Progress");
  Column* Backlog = new Column("Backlog");
  Column* Assigned = new Column("Assigned");
  Column* Bloked = new Column("Bloked");

  Task *task2 = new Task("Task3");

  Json_worker json_worker("../boards/board.json");
  board.add_developer(develop1);
  board.add_developer(develop);



  task->set_description("Hello world");
  

  In_progress->add_task(task1);
  Backlog->add_task(task1);

  task->set_developer(develop);

  board.add_column(Backlog);
  board.add_column(Assigned);
  board.add_column(In_progress);
  board.add_column(Bloked);
  board.add_column(Done);

  Assigned->add_task(task);
  move_task(Assigned, In_progress, task);
  move_task(In_progress, Assigned, task1);
  task1->set_title("Hello");
  In_progress->add_task(task2);
  move_task(Assigned, In_progress, task1);
  In_progress->delete_task(task2);
  Assigned->add_task(task2);
  task1->set_developer(develop1);
  
  
  task4->set_priority(2);
  task4->set_description("for lopuh");
  task4->set_developer(develop2);

  Done->add_task(task4);

  for (Task *element : Assigned->get_tasks()){
    std::cout << element->get_title() << " ";
  }
  std::cout << std::endl;
  for (Developer *element : board.get_developer()){
    std::cout << element->get_name() << " ";
  }

  std::cout << std::endl;
  for (Column *element : board.get_columns()){
    std::cout << element->get_name() << " ";
    for (Task *elem : element->get_tasks()){ 
      // std::cout << elem->get_title() << " " << "(" << elem->get_developer()->get_name() <<")" << " ";
    }
  }
  // json_worker.ids_add(ids);
  json_worker.board_add(board, json_worker.ids_add(ids));
  json_worker.save();
  std::cout << std::endl;

  delete Assigned;
  delete In_progress;
  delete Done;
  delete Backlog;
  delete Bloked;


}





