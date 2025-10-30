// #include <task.h>
// #include <board.h>
// #include <column.h>
// #include <developer.h>
// #include <json_worker.h>
// #include <manager.h>
// #include <ftxui.h>
// // #include <scrum_board.h>
// #include <iostream>


// int main(){

//   // Board board("Number_1");
//   // Task *task = new Task("Task1"), *task1 = new Task("Task2"), *task4 = new Task("Task4");
//   // Developer* develop = new Developer("Alex");
//   // Developer* develop1 = new Developer("Blala");
//   // Developer* develop2 = new Developer("lopuh");


//   // Column* Done = new Column("Done");
//   // Column* In_progress = new Column("In_Progress");
//   // Column* Backlog = new Column("Backlog");
//   // Column* Assigned = new Column("Assigned");
//   // Column* Bloked = new Column("Bloked");

//   // Task *task2 = new Task("Task3");

//   // Json_worker json_worker("../boards/board.json");
//   // board.add_developer(develop1);
//   // board.add_developer(develop);



//   // task->set_description("Hello world");
  

//   // In_progress->add_task(task1);
//   // Backlog->add_task(task1);

//   // task->set_developer(develop);

//   // board.add_column(Backlog);
//   // board.add_column(Assigned);
//   // board.add_column(In_progress);
//   // board.add_column(Bloked);
//   // board.add_column(Done);

//   // Assigned->add_task(task);
//   // move_task(Assigned, In_progress, task);
//   // move_task(In_progress, Assigned, task1);
//   // task1->set_title("Hello");
//   // In_progress->add_task(task2);
//   // move_task(Assigned, In_progress, task1);
//   // In_progress->delete_task(task2);
//   // Assigned->add_task(task2);
//   // task1->set_developer(develop1);
  
  
//   // task4->set_priority(2);
//   // task4->set_description("for lopuh");
//   // task4->set_developer(develop2);

//   // Done->add_task(task4);

//   // for (Task *element : Assigned->get_tasks()){
//   //   std::cout << element->get_title() << " ";
//   // }
//   // std::cout << std::endl;
//   // for (Developer *element : board.get_developer()){
//   //   std::cout << element->get_name() << " ";
//   // }
  
//   // std::cout << std::endl;
//   // // json_worker.board_load(board);
//   // int input;
//   // std::string tit;
//   // while (true){

//   //   std::cin >> input;

//   //   if (input == 1){
//   //     std::cout << "Enter name new task: ";
//   //     std::cin >> tit;
//   //     create_task(board, "Backlog", tit);
//   //     std::cout << "Succesful" << std::endl;

//   //   }
//   //   if (input == 2){

//   //     for (Column *element : board.get_columns()){
//   //       std::cout << element->get_name() << " ";
//   //       for (Task *elem : element->get_tasks()){ 
//   //         std::cout << elem->get_title() << " " << "(" << elem->get_developer()->get_name() <<")" << " ";
          
//   //       }
//   //     }
//   //     std::cout << std::endl;
//   //   }
//   //   if (input == 3){
//   //     std::cin >> tit;
//   //     std::cout << search_task(board,"Backlog", tit)->get_title() << std::endl;
//   //     std::cout << "Succesful" << std::endl;
//   //   }
//   //   if (input == 4){
//   //     std::cin >> tit;
//   //     move_task(Backlog, Assigned, search_task(board,"Backlog", tit));
//   //     std::cout << "Succesful" << std::endl;
//   //   }
//   //   if (input == 5){
//   //     std::cin >> tit;
//   //     Backlog->delete_task(search_task(board,"Backlog", tit));
//   //     std::cout << "Succesful" << std::endl;
//   //   }
//   //   if (input == 6){
//   //     std::cin >> tit;
//   //     create_developer(board, tit);
//   //     std::cout << "Succesful" << std::endl;
//   //   }
//   //   if (input == 7){
//   //     for(auto i: board.get_developer()){
//   //       std::cout << i->get_name() << " ";
//   //     }
//   //     std::cout << "Succesful" << std::endl;
//   //   }
//   //   if (input == 9){
//   //     break;
//   //   }

//   // }
//   // // json_worker.ids_add(ids);
//   // // json_worker.board_add(board, json_worker.ids_add(ids));
//   // // json_worker.save();
//   // std::cout << std::endl;
//   // json_worker.board_load(board);

//   // while (true){

//   //   std::cin >> input;

//   //   if (input == 1){
//   //     std::cout << "Enter name new task: ";
//   //     std::cin >> tit;
//   //     create_task(board, "Backlog", tit);
//   //     std::cout << "Succesful" << std::endl;

//   //   }
//   //   if (input == 2){

//   //     for (Column *element : board.get_columns()){
//   //       std::cout << element->get_name() << " ";
//   //       for (Task *elem : element->get_tasks()){ 
//   //         std::cout << elem->get_title() << " " << "(" << elem->get_developer()->get_name() <<")" << " ";
          
//   //       }
//   //     }
//   //     std::cout << std::endl;
//   //   }
//   //   if (input == 3){
//   //     std::cin >> tit;
//   //     std::cout << search_task(board,"Backlog", tit)->get_title() << std::endl;
//   //     std::cout << "Succesful" << std::endl;
//   //   }
//   //   if (input == 4){
//   //     std::cin >> tit;
//   //     move_task(Backlog, Assigned, search_task(board,"Backlog", tit));
//   //     std::cout << "Succesful" << std::endl;
//   //   }
//   //   if (input == 5){
//   //     std::cin >> tit;
//   //     for (auto *i:board.get_columns()){
//   //       if (i->get_name() == "Backlog"){
//   //         i->delete_task(search_task(board,"Backlog", tit));
//   //       }
//   //     }
//   //     std::cout << "Succesful" << std::endl;
//   //   }
//   //   if (input == 6){
//   //     std::cin >> tit;
//   //     create_developer(board, tit);
//   //     std::cout << "Succesful" << std::endl;
//   //   }
//   //   if (input == 7){
//   //     for(auto i: board.get_developer()){
//   //       std::cout << i->get_name() << " ";
//   //     }
//   //     std::cout << "Succesful" << std::endl;
//   //   }
//   //   if (input == 9){
//   //     break;
//   //   }

//   // }
//   // for (auto i:ids){
//   //   std::cout << i << " ";
//   // }

//   // // Перед сохранением
//   // json_worker.clear_ids();
//   // // Собираем актуальные IDs из всех задач
//   // std::vector<std::string> current_ids;
//   // for (Column* col : board.get_columns()) {
//   //     for (Task* task : col->get_tasks()) {
//   //         current_ids.push_back(task->get_id());
//   //     }
//   // }
//   // json_worker.board_add(board, json_worker.ids_add(current_ids));
//   // json_worker.save();


//   // delete Assigned;
//   // delete In_progress;
//   // delete Done;
//   // delete Backlog;
//   // delete Bloked;


// }





#include <ftxui.h>
#include <iostream>

int main() {
    try {
        ScrumBoardUI app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}