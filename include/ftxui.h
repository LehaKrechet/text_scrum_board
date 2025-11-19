#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "board.h"
#include "json_worker.h"
#include <memory>
#include <filesystem>

// Класс ScrumBoardUI реализует пользовательский интерфейс
// для управления Scrum доской с использованием библиотеки FTXUI
class ScrumBoardUI {
private:
    // Основные данные приложения
    std::shared_ptr<Board> board;           // Умный указатель на доску
    std::shared_ptr<Json_worker> json_worker; // Умный указатель на JSON worker
    std::string save_path = "../boards/board.json";  // Путь по умолчанию для сохранения
    
    // Компоненты UI (поля ввода, выбора и т.д.)
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
    
    // Переменные состояния UI
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
    int previous_component = 0;
    
    // Контейнеры данных для UI
    std::vector<std::string> column_names;
    std::vector<std::string> task_titles;
    std::vector<std::string> developer_names;
    std::vector<std::string> json_files;

    // Приватные методы для внутренней логики UI
    void initialize_board();      // Инициализация доски
    void update_task_list();      // Обновление списка задач
    void update_developer_list(); // Обновление списка разработчиков
    void update_file_list();      // Обновление списка файлов
    void setup_ui_components();   // Настройка компонентов UI
    void refresh_ui_data();       // Обновление всех данных UI
    ftxui::Element render_board(); // Отрисовка доски
    
    // Обработчики событий
    void handle_create_task();     // Создание задачи
    void handle_move_task();       // Перемещение задачи
    void handle_delete_task();     // Удаление задачи
    void handle_add_developer();   // Добавление разработчика
    void handle_delete_developer(); // Удаление разработчика
    void handle_assign_developer(); // Назначение разработчика
    void handle_save_load_dialog(bool is_save, const std::string& new_file_name, int selected_file); // Диалог сохранения/загрузки

public:
    ScrumBoardUI();  // Конструктор
    void run();      // Основной метод запуска приложения
};