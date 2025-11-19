#include "ftxui.h"
#include "manager.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>

using namespace ftxui;

// Обновление данных пользовательского интерфейса
void ScrumBoardUI::refresh_ui_data() {
    update_task_list();        // Обновление списка задач
    update_developer_list();   // Обновление списка разработчиков
    
    // Обновление списка названий колонок
    column_names.clear();
    for (const auto& col : board->get_columns()) {
        column_names.push_back(col->get_name());
    }
}

// Конструктор UI
ScrumBoardUI::ScrumBoardUI() {
    // Создание новой доски с именем по умолчанию
    board = std::make_shared<Board>("ScrumBoard");
    initialize_board();    // Инициализация начального состояния доски
    setup_ui_components(); // Настройка компонентов интерфейса
    previous_component = 2; // Установка начального состояния
}

// Инициализация доски начальными данными
void ScrumBoardUI::initialize_board() {
    // Если колонок нет - создаем стандартный набор
    if (board->get_columns().empty()) {
        board->add_column(std::make_unique<Column>("Backlog"));
        board->add_column(std::make_unique<Column>("Assigned"));
        board->add_column(std::make_unique<Column>("In Progress"));
        board->add_column(std::make_unique<Column>("Blocked"));
        board->add_column(std::make_unique<Column>("Done"));
    }
    
    refresh_ui_data(); // Обновление UI после инициализации
}

// Обновление списка задач для отображения в UI
void ScrumBoardUI::update_task_list() {
    task_titles.clear();
    
    // Сбор всех задач со всех колонок
    for (const auto& col : board->get_columns()) {
        for (const auto& task : col->get_tasks()) {
            // Формат: "Название задачи (Название колонки)"
            task_titles.push_back(task->get_title() + " (" + col->get_name() + ")");
        }
    }
    
    // Корректировка выбранной задачи если необходимо
    if (!task_titles.empty() && selected_task >= task_titles.size()) {
        selected_task = 0;
    } else if (task_titles.empty()) {
        selected_task = 0;
    }
}

// Обновление списка разработчиков для отображения в UI
void ScrumBoardUI::update_developer_list() {
    developer_names.clear();
    
    // Сбор имен всех разработчиков
    for (const auto& dev : board->get_developers()) {
        developer_names.push_back(dev->get_name());
    }
    
    // Корректировка выбранного разработчика если необходимо
    if (!developer_names.empty() && selected_developer >= developer_names.size()) {
        selected_developer = 0;
    } else if (developer_names.empty()) {
        selected_developer = 0;
    }
}

// Обновление списка JSON файлов в текущей директории
void ScrumBoardUI::update_file_list() {
    json_files.clear();
    
    if (file_path_input_str.empty()) {
        return;
    }
    
    try {
        std::filesystem::path path(file_path_input_str);
        
        // Обработка разных случаев пути
        if (std::filesystem::is_directory(path)) {
            // Если путь - директория, ищем все JSON файлы в ней
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    json_files.push_back(entry.path().filename().string());
                }
            }
        } else if (std::filesystem::is_regular_file(path)) {
            // Если путь - файл, переходим к родительской директории
            auto parent_path = path.parent_path();
            file_path_input_str = parent_path.string();
            
            for (const auto& entry : std::filesystem::directory_iterator(parent_path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    json_files.push_back(entry.path().filename().string());
                }
            }
        } else {
            // Если путь не существует, пробуем родительскую директорию
            auto parent_path = path.parent_path();
            if (std::filesystem::is_directory(parent_path)) {
                file_path_input_str = parent_path.string();
                
                for (const auto& entry : std::filesystem::directory_iterator(parent_path)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".json") {
                        json_files.push_back(entry.path().filename().string());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error reading directory: " << e.what() << std::endl;
    }
    
    // Корректировка выбранного файла если необходимо
    if (!json_files.empty() && selected_file >= json_files.size()) {
        selected_file = 0;
    } else if (json_files.empty()) {
        selected_file = 0;
    }
}

// Настройка всех компонентов пользовательского интерфейса
void ScrumBoardUI::setup_ui_components() {
    // Создание компонентов ввода
    task_title_input = Input(&task_title, "Enter task title");
    task_description_input = Input(&task_description, "Enter task description");
    task_priority_input = Input(&task_priority_str, "Enter task priority (0-10)");
    developer_name_input = Input(&developer_name, "Enter developer name");
    file_path_input = Input(&file_path_input_str, "Enter file path");
    
    // Создание компонентов выбора
    column_selection = Radiobox(&column_names, &selected_column);
    source_column_selection = Radiobox(&column_names, &selected_source_column);
    destination_column_selection = Radiobox(&column_names, &selected_destination_column);
    task_selection = Radiobox(&task_titles, &selected_task);
    developer_selection = Radiobox(&developer_names, &selected_developer);
    file_list_selection = Radiobox(&json_files, &selected_file);
}

// Обработчик создания новой задачи
void ScrumBoardUI::handle_create_task() {
    if (!task_title.empty()) {
        std::string column_name = column_names[selected_column];
        
        try {
            // Создание задачи через менеджер
            create_task(*board, column_name, task_title);
            
            // Поиск созданной задачи для установки дополнительных полей
            ::Task* task_ptr = search_task(*board, column_name, task_title);
            if (task_ptr) {
                task_ptr->set_description(task_description);
                
                // Установка приоритета с валидацией
                if (!task_priority_str.empty()) {
                    try {
                        int priority = std::stoi(task_priority_str);
                        task_ptr->set_priority(std::max(0, std::min(10, priority)));
                    } catch (const std::exception& e) {
                        task_ptr->set_priority(0); // Приоритет по умолчанию при ошибке
                    }
                }
                std::cout << "Task created successfully!" << std::endl;
            }
            
            // Очистка полей ввода
            task_title.clear();
            task_description.clear();
            task_priority_str.clear();
            refresh_ui_data(); // Обновление UI
        } catch (const std::exception& e) {
            std::cout << "Error creating task: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Error: Task title cannot be empty" << std::endl;
    }
}

// Обработчик перемещения задачи между колонками
void ScrumBoardUI::handle_move_task() {
    if (selected_source_column != selected_destination_column && 
        !task_titles.empty() && selected_task < task_titles.size()) {
        
        // Извлечение названия задачи и колонок из форматированной строки
        std::string full_task_name = task_titles[selected_task];
        size_t pos = full_task_name.find(" (");
        if (pos != std::string::npos) {
            std::string task_title_only = full_task_name.substr(0, pos);
            std::string source_col = column_names[selected_source_column];
            std::string dest_col = column_names[selected_destination_column];
            
            try {
                // Поиск задачи и колонок
                ::Task* task_ptr = search_task(*board, source_col, task_title_only);
                Column* source_column = board->find_column(source_col);
                Column* dest_column = board->find_column(dest_col);
                
                // Перемещение задачи
                if (source_column && dest_column && task_ptr) {
                    move_task(source_column, dest_column, task_ptr);
                    refresh_ui_data();
                    std::cout << "Task moved successfully!" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error moving task: " << e.what() << std::endl;
            }
        }
    }
}

// Обработчик удаления задачи
void ScrumBoardUI::handle_delete_task() {
    if (!task_titles.empty() && selected_task < task_titles.size()) {
        std::string full_task_name = task_titles[selected_task];
        size_t pos = full_task_name.find(" (");
        if (pos != std::string::npos) {
            // Извлечение названия задачи и колонки
            std::string task_title_only = full_task_name.substr(0, pos);
            std::string task_col = full_task_name.substr(pos + 2);
            task_col.pop_back(); // Удаление закрывающей скобки
            
            // Поиск колонки и удаление задачи
            Column* column = board->find_column(task_col);
            if (column) {
                try {
                    column->delete_task(task_title_only);
                    refresh_ui_data();
                    std::cout << "Task deleted successfully!" << std::endl;
                } catch (const std::exception& e) {
                    std::cout << "Error deleting task: " << e.what() << std::endl;
                }
            }
        }
    }
}

// Обработчик добавления разработчика
void ScrumBoardUI::handle_add_developer() {
    if (!developer_name.empty()) {
        try {
            create_developer(*board, developer_name);
            developer_name.clear();
            refresh_ui_data();
            std::cout << "Developer added successfully!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error adding developer: " << e.what() << std::endl;
        }
    }
}

// Обработчик удаления разработчика
void ScrumBoardUI::handle_delete_developer() {
    if (!developer_names.empty() && selected_developer < developer_names.size()) {
        std::string dev_name = developer_names[selected_developer];
        
        Developer* developer = board->find_developer(dev_name);
        if (developer) {
            try {
                // Удаление разработчика из всех задач
                for (const auto& col : board->get_columns()) {
                    for (const auto& task : col->get_tasks()) {
                        if (task->get_developer() == developer) {
                            task->set_developer(nullptr);
                        }
                    }
                }
                
                // Удаление разработчика из доски
                auto& developers = board->get_developers();
                auto it = std::find_if(developers.begin(), developers.end(),
                    [&](const std::unique_ptr<Developer>& dev) {
                        return dev.get() == developer;
                    });
                
                if (it != developers.end()) {
                    developers.erase(it);
                    refresh_ui_data();
                    std::cout << "Developer deleted successfully!" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error deleting developer: " << e.what() << std::endl;
            }
        }
    }
}

// Обработчик назначения разработчика на задачу
void ScrumBoardUI::handle_assign_developer() {
    if (!task_titles.empty() && !developer_names.empty() && 
        selected_task < task_titles.size() && selected_developer < developer_names.size()) {
        
        std::string full_task_name = task_titles[selected_task];
        size_t pos = full_task_name.find(" (");
        if (pos != std::string::npos) {
            // Извлечение названия задачи и колонки
            std::string task_title_only = full_task_name.substr(0, pos);
            std::string task_col = full_task_name.substr(pos + 2);
            task_col.pop_back();
            
            std::string dev_name = developer_names[selected_developer];
            
            try {
                // Поиск задачи и разработчика
                ::Task* task_ptr = search_task(*board, task_col, task_title_only);
                Developer* developer = board->find_developer(dev_name);
                
                // Назначение разработчика на задачу
                if (developer && task_ptr) {
                    task_ptr->set_developer(developer);
                    refresh_ui_data();
                    std::cout << "Developer assigned successfully!" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error assigning developer: " << e.what() << std::endl;
            }
        }
    }
}

// Обработчик диалога сохранения/загрузки доски
void ScrumBoardUI::handle_save_load_dialog(bool is_save, const std::string& new_file_name, int selected_file) {
    if (!file_path_input_str.empty()) {
        std::filesystem::path full_path;
        
        // Формирование полного пути к файлу
        if (!new_file_name.empty()) {
            full_path = std::filesystem::path(file_path_input_str) / new_file_name;
            if (full_path.extension() != ".json") {
                full_path += ".json";
            }
        } else if (!json_files.empty() && selected_file < json_files.size()) {
            full_path = std::filesystem::path(file_path_input_str) / json_files[selected_file];
        } else {
            full_path = file_path_input_str;
            if (full_path.extension() != ".json") {
                full_path += ".json";
            }
        }
        
        // Создание директорий если необходимо
        std::filesystem::path parent_dir = full_path.parent_path();
        if (!parent_dir.empty() && !std::filesystem::exists(parent_dir)) {
            try {
                std::filesystem::create_directories(parent_dir);
                std::cout << "Created directory: " << parent_dir.string() << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error creating directory: " << e.what() << std::endl;
                return;
            }
        }
        
        if (is_save) {
            // Логика сохранения доски
            bool file_exists = std::filesystem::exists(full_path);
            
            if (file_exists) {
                std::cout << "File already exists. Overwriting: " << full_path.string() << std::endl;
            } else {
                std::cout << "Creating new file: " << full_path.string() << std::endl;
            }
            
            try {
                // Сбор ID всех задач для сохранения
                std::vector<std::string> current_ids;
                for (const auto& col : board->get_columns()) {
                    for (const auto& task : col->get_tasks()) {
                        current_ids.push_back(task->get_id());
                    }
                }
                
                // Сохранение доски через JSON worker
                json_worker = std::make_shared<Json_worker>(full_path.string());
                json_worker->clear_ids();
                json_worker->board_add(*board, json_worker->ids_add(current_ids));
                json_worker->save();
                save_path = full_path.string();
                
                // Установка имени доски из имени файла
                std::string board_name = full_path.stem().string();
                board->set_name(board_name);
                
                std::cout << "Board successfully saved to: " << full_path.string() << std::endl;
                std::cout << "Board name set to: " << board_name << std::endl;
                
                // Проверка существования файла
                if (std::filesystem::exists(full_path)) {
                    std::cout << "File verified: " << full_path.string() << std::endl;
                } else {
                    std::cout << "Warning: File may not have been created: " << full_path.string() << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error saving board: " << e.what() << std::endl;
                return;
            }
        } else {
            // Логика загрузки доски
            if (!std::filesystem::exists(full_path)) {
                std::cout << "Error: File does not exist: " << full_path.string() << std::endl;
                return;
            }
            
            // Проверка валидности файла
            Json_worker temp_worker(full_path.string());
            if (!temp_worker.is_valid_board_file(full_path.string())) {
                std::cout << "Error: Invalid board file format: " << full_path.string() << std::endl;
                return;
            }
            
            try {
                json_worker = std::make_shared<Json_worker>(full_path.string());
                
                // Установка имени доски из имени файла
                std::string board_name = full_path.stem().string();
                
                // Загрузка доски
                json_worker->board_load(*board);
                
                // Установка имени доски
                board->set_name(board_name);
                
                // Инициализация и обновление UI
                initialize_board();
                refresh_ui_data();
                save_path = full_path.string();
                std::cout << "Board successfully loaded from: " << full_path.string() << std::endl;
                std::cout << "Board name set to: " << board_name << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error loading board: " << e.what() << std::endl;
                return;
            }
        }
    }
}

// Отрисовка доски в виде колонок с задачами
Element ScrumBoardUI::render_board() {
    Elements column_elements;
    
    // Отрисовка каждой колонки
    for (const auto& column : board->get_columns()) {
        Elements task_elements;
        
        // Заголовок колонки
        task_elements.push_back(text(column->get_name()) | bold | center);
        task_elements.push_back(separator());
        
        auto& tasks = column->get_tasks();
        if (tasks.empty()) {
            // Сообщение для пустой колонки
            task_elements.push_back(
                text("No tasks") | center | size(HEIGHT, EQUAL, 3)
            );
        } else {
            // Отрисовка каждой задачи в колонке
            for (const auto& task : tasks) {
                std::string developer_name = "Unassigned";
                if (task->get_developer()) {
                    try {
                        developer_name = task->get_developer()->get_name();
                    } catch (const std::exception& e) {
                        developer_name = "Unknown";
                    }
                }
                
                // Создание элемента задачи с иконками и информацией
                auto task_element = vbox({
                    text("📝 " + task->get_title()) | bold,                    // Иконка и заголовок
                    separator(),
                    text("📋 " + (task->get_description().empty() ? "No description" : task->get_description())), // Описание
                    text("🎯 Priority: " + std::to_string(task->get_priority())), // Приоритет
                    text("👨 " + developer_name)                               // Разработчик
                }) | border | flex | size(WIDTH, EQUAL, 25) | size(HEIGHT, EQUAL, 8);
                
                task_elements.push_back(task_element);
                task_elements.push_back(filler()); // Заполнитель для spacing
            }
        }
        
        // Создание элемента колонки
        auto column_element = vbox(std::move(task_elements)) | border | flex | frame | vscroll_indicator;
        column_elements.push_back(column_element);
    }
    
    // Компоновка всех колонок в горизонтальный layout
    return vbox({
        text("SCRUM Board - " + board->get_name()) | bold | hcenter, // Заголовок доски
        separator(),
        hbox(std::move(column_elements)) | flex // Горизонтальное расположение колонок
    });
}

// Основной метод запуска приложения
void ScrumBoardUI::run() {
    // Создание интерактивного экрана
    auto screen = ScreenInteractive::Fullscreen();
    
    // Переменные состояния UI
    int active_component = 2;
    bool is_save_dialog = false;
    std::string new_file_name;
    
    // Создание кнопок с обработчиками
    auto create_task_btn = Button("Create Task", [this] {
        handle_create_task();
    }, ButtonOption::Animated());

    auto clear_task_btn = Button("Clear", [this] {
        task_title.clear();
        task_description.clear();
        task_priority_str.clear();
    }, ButtonOption::Animated());

    auto move_task_btn = Button("Move Task", [this] {
        handle_move_task();
    }, ButtonOption::Animated());

    auto delete_task_btn = Button("Delete Task", [this] {
        handle_delete_task();
    }, ButtonOption::Animated());

    auto save_btn = Button("Save Board", [&] {
        previous_component = active_component;
        active_component = 1;
        is_save_dialog = true;
        file_path_input_str = std::filesystem::current_path().string();
        new_file_name.clear();
        update_file_list();
    }, ButtonOption::Animated());

    auto load_btn = Button("Load Board", [&] {
        previous_component = active_component;
        active_component = 1;
        is_save_dialog = false;
        file_path_input_str = std::filesystem::current_path().string();
        new_file_name.clear();
        update_file_list();
    }, ButtonOption::Animated());

    auto exit_btn = Button("Exit", [&] { 
        screen.Exit(); 
    }, ButtonOption::Animated());

    auto new_board_btn = Button("Create New Board", [&] {
        board = std::make_shared<Board>("ScrumBoard");
        initialize_board();
        active_component = 0;
        std::cout << "Created new empty board" << std::endl;
    }, ButtonOption::Animated());

    auto load_existing_btn = Button("Load Existing Board", [&] {
        previous_component = active_component;
        active_component = 1;
        is_save_dialog = false;
        file_path_input_str = std::filesystem::current_path().string();
        new_file_name.clear();
        update_file_list();
    }, ButtonOption::Animated());

    // Компоновка компонентов в контейнеры
    auto task_creation_buttons = Container::Horizontal({
        create_task_btn,
        clear_task_btn
    });

    auto task_creation_tab = Container::Vertical({
        task_title_input,
        task_description_input,
        task_priority_input,
        column_selection,
        task_creation_buttons
    });

    auto dev_creation_add_btn = Button("Add Developer", [this] {
        handle_add_developer();
    }, ButtonOption::Animated());

    auto dev_creation_delete_btn = Button("Delete Developer", [this] {
        handle_delete_developer();
    }, ButtonOption::Animated());

    auto developer_creation_buttons = Container::Horizontal({
        dev_creation_add_btn,
        dev_creation_delete_btn
    });

    auto developer_creation_selection = Radiobox(&developer_names, &selected_developer);

    auto developer_creation_tab = Container::Vertical({
        developer_name_input,
        developer_creation_selection,
        developer_creation_buttons
    });
    
    auto task_management_buttons = Container::Horizontal({
        move_task_btn,
        delete_task_btn
    });

    auto task_management_tab = Container::Vertical({
        source_column_selection,
        destination_column_selection,
        task_selection,
        task_management_buttons
    });
    
    auto dev_assignment_btn = Button("Assign Developer", [this] {
        handle_assign_developer();
    }, ButtonOption::Animated());

    auto dev_assignment_task_selection = Radiobox(&task_titles, &selected_task);
    auto dev_assignment_developer_selection = Radiobox(&developer_names, &selected_developer);

    auto developer_assignment_tab = Container::Vertical({
        dev_assignment_task_selection,
        dev_assignment_developer_selection,
        dev_assignment_btn
    });
  
    auto control_buttons = Container::Horizontal({
        save_btn,
        load_btn,
        exit_btn
    });

    auto refresh_dialog_btn = Button("Refresh", [this] {
        update_file_list();
    }, ButtonOption::Animated());

    auto new_file_name_input_component = Input(&new_file_name, "Enter new file name");

    auto confirm_dialog_btn = Button(is_save_dialog ? "Save" : "Load", [&] {
        handle_save_load_dialog(is_save_dialog, new_file_name, selected_file);
        active_component = 0;
        new_file_name.clear();
    }, ButtonOption::Animated());

    auto cancel_dialog_btn = Button("Cancel", [&] {
        active_component = previous_component;
        new_file_name.clear();
        file_path_input_str.clear();
        json_files.clear();
    }, ButtonOption::Animated());
    
    auto create_new_file_btn = Button("Create New", [&] {
        if (!new_file_name.empty()) {
            std::filesystem::path full_path = std::filesystem::path(file_path_input_str) / new_file_name;
            if (full_path.extension() != ".json") {
                full_path += ".json";
            }
            
            if (std::filesystem::exists(full_path)) {
                std::cout << "File already exists: " << full_path.string() << std::endl;
            } else {
                std::cout << "Ready to create new file: " << full_path.string() << std::endl;
            }
        }
    }, ButtonOption::Animated());

    auto file_dialog_buttons = Container::Horizontal({
        confirm_dialog_btn,
        cancel_dialog_btn,
        refresh_dialog_btn
    });

    auto file_dialog_component = Container::Vertical({
        file_path_input,
        file_list_selection,
        new_file_name_input_component,
        create_new_file_btn,
        file_dialog_buttons
    });

    // Рендереры для разных состояний UI
    auto file_dialog_renderer = Renderer(file_dialog_component, [&] {
        Elements elements;
        
        std::string dialog_title = is_save_dialog ? "Save Board" : "Load Board";
        elements.push_back(text(dialog_title) | bold | hcenter);
        elements.push_back(separator());
        
        elements.push_back(hbox({text("Path: "), file_path_input->Render()}));
        elements.push_back(separator());
        
        if (json_files.empty()) {
            elements.push_back(text("No JSON files found") | center | color(Color::GrayDark));
        } else {
            elements.push_back(text("Available JSON files:"));
            elements.push_back(file_list_selection->Render() | frame | vscroll_indicator | flex);
        }
        
        elements.push_back(separator());
        
        if (is_save_dialog) {
            elements.push_back(text("Create New File:") | bold);
            elements.push_back(hbox({text("File name: "), new_file_name_input_component->Render()}));
            elements.push_back(create_new_file_btn->Render() | center);
            elements.push_back(separator());
        }
        
        elements.push_back(text("Selection: " + (new_file_name.empty() ? 
            (json_files.empty() ? "No file selected" : json_files[selected_file]) : 
            "New file: " + new_file_name)));
        elements.push_back(separator());
        
        elements.push_back(file_dialog_buttons->Render() | center);
        
        return vbox(elements) | border | center;
    });

    auto startup_buttons = Container::Horizontal({
        new_board_btn,
        load_existing_btn
    });

    auto startup_component = Container::Vertical({
        startup_buttons
    });

    auto startup_renderer = Renderer(startup_component, [&] {
        Elements elements;
        
        elements.push_back(text("SCRUM Board") | bold | hcenter);
        elements.push_back(separator());
        elements.push_back(text("Welcome to SCRUM Board!") | center);
        elements.push_back(separator());
        elements.push_back(text("Choose an option:") | center);
        elements.push_back(separator());
        elements.push_back(startup_buttons->Render() | center);
        
        return vbox(elements) | border | center;
    });

    auto board_renderer = Renderer([this] { 
        return render_board(); 
    });

    auto task_creation_renderer = Renderer(task_creation_tab, [this, task_creation_buttons] {
        Elements elements;
        elements.push_back(text("Create New Task") | bold | hcenter);
        elements.push_back(separator());
        elements.push_back(hbox({text("Title: "), task_title_input->Render()}));
        elements.push_back(hbox({text("Description: "), task_description_input->Render()}));
        elements.push_back(hbox({text("Priority (0-10): "), task_priority_input->Render()}));
        elements.push_back(text("Select Column:"));
        elements.push_back(column_selection->Render());
        elements.push_back(separator());
        elements.push_back(task_creation_buttons->Render() | center);
        return vbox(elements) | border;
    });

    auto developer_creation_renderer = Renderer(developer_creation_tab, [this, developer_creation_buttons, developer_creation_selection] {
        Elements elements;
        elements.push_back(text("Manage Developers") | bold | hcenter);
        elements.push_back(separator());
        elements.push_back(hbox({text("Developer Name: "), developer_name_input->Render()}));
        elements.push_back(separator());
        elements.push_back(text("Current Developers:"));
        elements.push_back(developer_creation_selection->Render());
        elements.push_back(separator());
        elements.push_back(developer_creation_buttons->Render() | center);
        return vbox(elements) | border;
    });

    auto task_management_renderer = Renderer(task_management_tab, [this, task_management_buttons] {
        Elements elements;
        elements.push_back(text("Manage Tasks") | bold | hcenter);
        elements.push_back(separator());
        elements.push_back(text("Move Task Between Columns"));
        elements.push_back(hbox({
            vbox({text("From:"), source_column_selection->Render()}),
            vbox({text("To:"), destination_column_selection->Render()})
        }));
        elements.push_back(separator());
        elements.push_back(text("Available Tasks:"));
        elements.push_back(task_selection->Render());
        elements.push_back(separator());
        elements.push_back(task_management_buttons->Render() | center);
        return vbox(elements) | border;
    });

    auto developer_assignment_renderer = Renderer(developer_assignment_tab, [this, dev_assignment_btn, dev_assignment_task_selection, dev_assignment_developer_selection] {
        Elements elements;
        elements.push_back(text("Assign Developer to Task") | bold | hcenter);
        elements.push_back(separator());
        
        Elements task_dev_elements;
        task_dev_elements.push_back(vbox({
            text("Select Task:"),
            dev_assignment_task_selection->Render()
        }));
        task_dev_elements.push_back(vbox({
            text("Select Developer:"),
            dev_assignment_developer_selection->Render()
        }));
        
        elements.push_back(hbox(std::move(task_dev_elements)));
        elements.push_back(separator());
        elements.push_back(dev_assignment_btn->Render() | center);
        return vbox(elements) | border;
    });

    // Создание системы вкладок
    std::vector<Component> tab_content_components = {
        board_renderer,
        task_creation_renderer,
        developer_creation_renderer,
        task_management_renderer,
        developer_assignment_renderer
    };
    
    auto tab_container = Container::Tab(tab_content_components, &current_tab);
    
    std::vector<std::string> tab_entries = {
        "Board View",
        "Create Task", 
        "Manage Developers",
        "Manage Tasks",
        "Assign Developer"
    };
    
    auto tab_selection = Toggle(&tab_entries, &current_tab);
    
    // Основной компонент приложения
    auto main_component = Container::Vertical({
        tab_selection,
        tab_container,
        control_buttons
    });

    auto main_renderer = Renderer(main_component, [&] {
        return vbox({
            text("SCRUM Board") | bold | hcenter,
            separator(),
            tab_selection->Render(),
            tab_container->Render() | flex,
            separator(),
            control_buttons->Render() | center
        });
    });
    
    // Финальная компоновка всех компонентов
    auto main_state_component = Container::Vertical({
        main_component
    });
    
    auto file_dialog_state_component = Container::Vertical({
        file_dialog_component
    });
    
    auto startup_state_component = Container::Vertical({
        startup_component
    });
    
    auto final_component = Container::Vertical({
        main_state_component,
        file_dialog_state_component, 
        startup_state_component
    });
    
    // Финальный рендерер с переключением между состояниями
    auto final_renderer = Renderer(final_component, [&] {
        if (active_component == 0) {
            return main_renderer->Render();
        } else if (active_component == 1) {
            return file_dialog_renderer->Render();
        } else {
            return startup_renderer->Render();
        }
    });
    
    // Запуск основного цикла приложения
    screen.Loop(final_renderer);
}