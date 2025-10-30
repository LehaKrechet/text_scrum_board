#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "board.h"
#include "json_worker.h"
#include <memory>

class ScrumBoardUI {
private:
    std::shared_ptr<Board> board;
    std::shared_ptr<Json_worker> json_worker;
    std::string save_path = "../boards/board.json";
    
    // UI компоненты
    ftxui::Component task_title_input;
    ftxui::Component task_description_input;
    ftxui::Component task_priority_input;
    ftxui::Component developer_name_input;
    ftxui::Component column_selection;
    ftxui::Component source_column_selection;
    ftxui::Component destination_column_selection;
    ftxui::Component task_selection;
    ftxui::Component developer_selection;
    
    // Состояния
    std::string task_title;
    std::string task_description;
    std::string task_priority_str;
    std::string developer_name;
    int selected_column = 0;
    int selected_source_column = 0;
    int selected_destination_column = 1;
    int selected_task = 0;
    int selected_developer = 0;
    int current_tab = 0;
    
    // Списки для выбора
    std::vector<std::string> column_names;
    std::vector<std::string> task_titles;
    std::vector<std::string> developer_names;
    
    void initialize_board();
    void update_task_list();
    void update_developer_list();
    void setup_ui_components();
    ftxui::Element render_board();
    ftxui::Element render_task_creation();
    ftxui::Element render_developer_creation();
    ftxui::Element render_task_management();
    ftxui::Element render_developer_assignment();

public:
    ScrumBoardUI();
    void run();
};