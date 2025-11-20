#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "board.h"
#include "json_worker.h"
#include <memory>
#include <filesystem>

// Класс ScrumBoardUI реализует пользовательский интерфейс
// для управления Scrum доской с использованием библиотеки FTXUI
// FTXUI - это кроссплатформенная библиотека для создания терминальных UI
class ScrumBoardUI {
private:
    // Основные данные приложения
    
    // Умный указатель на доску
    // shared_ptr позволяет нескольким компонентам владеть одной доской
    std::shared_ptr<Board> board;
    
    // Умный указатель на JSON worker для сохранения/загрузки
    std::shared_ptr<Json_worker> json_worker;
    
    // Путь по умолчанию для сохранения досок
    std::string save_path = "../boards/board.json";
    
    // Компоненты UI FTXUI
    // Каждый Component представляет интерактивный элемент интерфейса
    
    ftxui::Component task_title_input;           // Поле ввода заголовка задачи
    ftxui::Component task_description_input;     // Поле ввода описания задачи
    ftxui::Component task_priority_input;        // Поле ввода приоритета задачи
    ftxui::Component developer_name_input;       // Поле ввода имени разработчика
    ftxui::Component column_selection;           // Выбор колонки для новой задачи
    ftxui::Component source_column_selection;    // Выбор исходной колонки для перемещения
    ftxui::Component destination_column_selection; // Выбор целевой колонки для перемещения
    ftxui::Component task_selection;             // Выбор задачи из списка
    ftxui::Component developer_selection;        // Выбор разработчика из списка
    ftxui::Component file_path_input;            // Поле ввода пути к файлу
    ftxui::Component file_list_selection;        // Выбор файла из списка
    
    // Переменные состояния UI - хранят текущие значения элементов управления
    
    std::string task_title;           // Введенный заголовок задачи
    std::string task_description;     // Введенное описание задачи
    std::string task_priority_str;    // Введенный приоритет (как строка)
    std::string developer_name;       // Введенное имя разработчика
    std::string file_path_input_str;  // Введенный путь к файлу
    
    // Индексы выбранных элементов в спискаов
    int selected_column = 0;          // Индекс выбранной колонки
    int selected_source_column = 0;   // Индекс исходной колонки для перемещения
    int selected_destination_column = 1; // Индекс целевой колонки для перемещения
    int selected_task = 0;            // Индекс выбранной задачи
    int selected_developer = 0;       // Индекс выбранного разработчика
    int selected_file = 0;            // Индекс выбранного файла
    
    int current_tab = 0;              // Текущая активная вкладка
    int previous_component = 0;       // Предыдущее состояние UI (для навигации назад)
    
    // Контейнеры данных для отображения в UI
    
    std::vector<std::string> column_names;    // Названия колонок для выбора
    std::vector<std::string> task_titles;     // Форматированные названия задач
    std::vector<std::string> developer_names; // Имена разработчиков
    std::vector<std::string> json_files;      // Список JSON файлов в директории

    // Методы для внутренней логики UI
    
    // Инициализация доски начальными данными
    // Создает стандартные колонки если их нет
    void initialize_board();
    
    // Обновление списка задач для отображения
    // Собирает все задачи со всех колонок и форматирует их
    void update_task_list();
    
    // Обновление списка разработчиков
    void update_developer_list();
    
    // Обновление списка файлов в текущей директории
    // Ищет все JSON файлы для загрузки/сохранения
    void update_file_list();
    
    // Настройка и создание компонентов UI
    // Инициализирует все элементы управления FTXUI
    void setup_ui_components();
    
    // Обновление всех данных UI
    // Вызывает все отдельные методы обновления
    void refresh_ui_data();
    
    // Отрисовка визуального представления доски
    // Создает графическое отображение колонок и задач
    ftxui::Element render_board();
    
    // Обработчики событий - вызываются при взаимодействии пользователя
    
    void handle_create_task();     // Создание новой задачи
    void handle_move_task();       // Перемещение задачи между колонками
    void handle_delete_task();     // Удаление задачи
    void handle_add_developer();   // Добавление нового разработчика
    void handle_delete_developer(); // Удаление разработчика
    void handle_assign_developer(); // Назначение разработчика на задачу
    
    // Диалог сохранения/загрузки доски
    // is_save - true для сохранения, false для загрузки
    // new_file_name - имя нового файла (для сохранения)
    // selected_file - индекс выбранного файла (для загрузки)
    void handle_save_load_dialog(bool is_save, const std::string& new_file_name, int selected_file);

    // Методы для адаптивной цветовой схемы
    ftxui::Color get_text_color() const { return ftxui::Color::Default; }
    
    // Метод для создания стилизованных компонентов ввода
    ftxui::Component create_styled_input(std::string* content, const std::string& placeholder);

    ftxui::Component create_adaptive_button(const std::string& label, std::function<void()> on_click);

public:
    ScrumBoardUI();  // Конструктор - инициализирует UI и данные
    void run();      // Основной метод запуска приложения
};