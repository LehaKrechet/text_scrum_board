#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "board.h"
#include "json_worker.h"
#include <memory>
#include <filesystem>

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
    ftxui::Component file_path_input;
    ftxui::Component file_list_selection;
    
    // Компоненты для прокрутки колонок
    std::vector<ftxui::Component> column_containers;
    
    // Состояния
    std::string task_title;
    std::string task_description;
    std::string task_priority_str;
    std::string developer_name;
    std::string file_path_input_str;
    int selected_column = 0;
    int selected_source_column = 0;
    int selected_destination_column = 1;
    int selected_task = 0;
    int selected_developer = 0;
    int selected_file = 0;
    int current_tab = 0;
    bool show_file_dialog = false;
    bool is_save_dialog = false;
    int previous_component = 0;
    
    // Списки для выбора
    std::vector<std::string> column_names;
    std::vector<std::string> task_titles;
    std::vector<std::string> developer_names;
    std::vector<std::string> json_files;

    void initialize_board();
    void update_task_list();
    void update_developer_list();
    void update_file_list();
    void setup_ui_components();
    void recreate_ui_components();
    void refresh_ui_data();
    void setup_column_scroll_containers(); // Новая функция для настройки прокрутки
    ftxui::Element render_board();
    ftxui::Element render_task_creation();
    ftxui::Element render_developer_creation();
    ftxui::Element render_task_management();
    ftxui::Element render_developer_assignment();
    ftxui::Element render_file_dialog();

public:
    ScrumBoardUI();
    void run();
};