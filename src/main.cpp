#include <task.h>
// #include <board.h>
// #include <column.h>
// #include <ftxui.h>
// #include <scrum_board.h>
#include <iostream>


int main(){
  Task task;
  task.id = task.generate_id();
  std::cout << task.id << std::endl;
  for (size_t i = 0; i < ids.size(); ++i) {
      std::cout << ids[i] << " ";
  }
  std::cout << std::endl;
  task.id = task.generate_id();
  std::cout << task.id << std::endl;
  for (size_t i = 0; i < ids.size(); ++i) {
      std::cout << ids[i] << " ";
  }
}