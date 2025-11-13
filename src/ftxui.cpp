#include "ftxui.h"
#include "manager.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>

using namespace ftxui;

// ДОБАВЛЯЕМ РЕАЛИЗАЦИЮ ФУНКЦИИ
void ScrumBoardUI::refresh_ui_data() {
    // Обновляем только данные, не пересоздавая компоненты
    update_task_list();
    update_developer_list();
    
    // Обновляем имена колонок
    column_names.clear();
    for (auto* col : board->get_columns()) {
        column_names.push_back(col->get_name());
    }
}

ScrumBoardUI::ScrumBoardUI() {
    // Создаем пустую доску вместо загрузки последней
    board = std::make_shared<Board>("ScrumBoard");
    initialize_board();
    setup_ui_components();
    previous_component = 2;
}

void ScrumBoardUI::initialize_board() {
    // Добавляем стандартные колонки если их нет
    if (board->get_columns().empty()) {
        board->add_column(new Column("Backlog"));
        board->add_column(new Column("Assigned"));
        board->add_column(new Column("In Progress"));
        board->add_column(new Column("Blocked"));
        board->add_column(new Column("Done"));
    }
    
    update_task_list();
    update_developer_list();
    
    // Заполняем имена колонок
    column_names.clear();
    for (auto* col : board->get_columns()) {
        column_names.push_back(col->get_name());
    }
}

void ScrumBoardUI::update_task_list() {
    task_titles.clear();
    for (auto* col : board->get_columns()) {
        for (auto* task : col->get_tasks()) {
            task_titles.push_back(task->get_title() + " (" + col->get_name() + ")");
        }
    }
    
    // Корректируем selected_task если нужно
    if (!task_titles.empty() && selected_task >= task_titles.size()) {
        selected_task = 0;
    } else if (task_titles.empty()) {
        selected_task = 0;
    }
}

void ScrumBoardUI::update_developer_list() {
    developer_names.clear();
    for (auto* dev : board->get_developer()) {
        developer_names.push_back(dev->get_name());
    }
    
    // Корректируем selected_developer если нужно
    if (!developer_names.empty() && selected_developer >= developer_names.size()) {
        selected_developer = 0;
    } else if (developer_names.empty()) {
        selected_developer = 0;
    }
}

void ScrumBoardUI::update_file_list() {
    json_files.clear();
    
    if (file_path_input_str.empty()) {
        return;
    }
    
    try {
        std::filesystem::path path(file_path_input_str);
        
        // Если путь - директория, ищем JSON файлы в ней
        if (std::filesystem::is_directory(path)) {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    json_files.push_back(entry.path().filename().string());
                }
            }
        }
        // Если путь - файл, показываем его и другие файлы в той же директории
        else if (std::filesystem::is_regular_file(path)) {
            auto parent_path = path.parent_path();
            file_path_input_str = parent_path.string();
            
            for (const auto& entry : std::filesystem::directory_iterator(parent_path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    json_files.push_back(entry.path().filename().string());
                }
            }
        }
        // Если путь не существует, пытаемся получить родительскую директорию
        else {
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
    
    // Корректируем selected_file если нужно
    if (!json_files.empty() && selected_file >= json_files.size()) {
        selected_file = 0;
    } else if (json_files.empty()) {
        selected_file = 0;
    }
}

void ScrumBoardUI::setup_ui_components() {
    // Инициализация компонентов ввода
    task_title_input = Input(&task_title, "Enter task title");
    task_description_input = Input(&task_description, "Enter task description");
    task_priority_input = Input(&task_priority_str, "Enter task priority (0-10)");
    developer_name_input = Input(&developer_name, "Enter developer name");
    file_path_input = Input(&file_path_input_str, "Enter file path");
    
    // Компоненты выбора
    column_selection = Radiobox(&column_names, &selected_column);
    source_column_selection = Radiobox(&column_names, &selected_source_column);
    destination_column_selection = Radiobox(&column_names, &selected_destination_column);
    task_selection = Radiobox(&task_titles, &selected_task);
    developer_selection = Radiobox(&developer_names, &selected_developer);
    file_list_selection = Radiobox(&json_files, &selected_file);
}

Element ScrumBoardUI::render_board() {
    Elements column_elements;
    
    for (auto* column : board->get_columns()) {
        Elements task_elements;
        task_elements.push_back(text(column->get_name()) | bold | center);
        task_elements.push_back(separator());
        
        auto tasks = column->get_tasks();
        if (tasks.empty()) {
            task_elements.push_back(text("No tasks") | center | color(Color::GrayDark));
        } else {
            for (auto* task : tasks) {
                std::string developer_name = "Unassigned";
                if (task->get_developer()) {
                    try {
                        developer_name = task->get_developer()->get_name();
                    } catch (const std::exception& e) {
                        developer_name = "Unknown";
                    }
                }
                
                auto task_element = vbox({
                    text("📝 " + task->get_title()) | bold,
                    text("📋 " + task->get_description()),
                    text("🎯 Priority: " + std::to_string(task->get_prioriy())),
                    text("👨‍💻 " + developer_name),
                    separator()
                }) | border | flex;
                
                task_elements.push_back(task_element);
            }
        }
        
        auto column_element = vbox(std::move(task_elements)) | border | flex;
        column_elements.push_back(column_element);
    }
    
    return vbox({
        text("SCRUM Board - " + board->get_name()) | bold | hcenter,
        separator(),
        hbox(std::move(column_elements)) | flex
    });
}

void ScrumBoardUI::run() {
    auto screen = ScreenInteractive::Fullscreen();
    
    // Используем int для управления активным компонентом 
    // 0 - главный интерфейс, 1 - диалог загрузки/сохранения, 2 - стартовый диалог
    int active_component = 2; // По умолчанию показываем стартовый диалог
    bool is_save_dialog = false;
    
    // Локальные переменные для диалога
    std::string new_file_name;
    
    // Создаем кнопки с поддержкой мыши
    auto create_task_btn = Button("Create Task", [this] {
        if (!task_title.empty()) {
            std::string column_name = column_names[selected_column];
            std::cout << "Creating task: " << task_title << " in column: " << column_name << std::endl;
            
            create_task(*board, column_name, task_title);
            
            try {
                ::Task* task = search_task(*board, column_name, task_title);
                if (task) {
                    task->set_description(task_description);
                    
                    if (!task_priority_str.empty()) {
                        try {
                            int priority = std::stoi(task_priority_str);
                            task->set_priority(std::max(0, std::min(10, priority)));
                        } catch (const std::exception& e) {
                            task->set_priority(0);
                        }
                    }
                    std::cout << "Task created successfully!" << std::endl;
                }
            } catch (const std::out_of_range& e) {
                std::cout << "Error: Task not found after creation" << std::endl;
            }
            
            task_title.clear();
            task_description.clear();
            task_priority_str.clear();
            refresh_ui_data(); // Только обновляем данные
        } else {
            std::cout << "Error: Task title cannot be empty" << std::endl;
        }
    }, ButtonOption::Animated());

    auto clear_task_btn = Button("Clear", [this] {
        task_title.clear();
        task_description.clear();
        task_priority_str.clear();
    }, ButtonOption::Animated());

    auto add_dev_btn = Button("Add Developer", [this] {
        if (!developer_name.empty()) {
            create_developer(*board, developer_name);
            developer_name.clear();
            refresh_ui_data(); // Только обновляем данные
            std::cout << "Developer added successfully!" << std::endl;
        }
    }, ButtonOption::Animated());

    auto delete_dev_btn = Button("Delete Developer", [this] {
        if (!developer_names.empty() && selected_developer < developer_names.size()) {
            std::string dev_name = developer_names[selected_developer];
            bool found = false;
            
            auto developers = board->get_developer();
            for (auto* dev : developers) {
                if (dev->get_name() == dev_name) {
                    for (auto* col : board->get_columns()) {
                        for (auto* task : col->get_tasks()) {
                            if (task->get_developer() == dev) {
                                task->set_developer(nullptr);
                            }
                        }
                    }
                    
                    board->delete_developer(dev);
                    delete dev;
                    found = true;
                    break;
                }
            }
            
            if (found) {
                refresh_ui_data(); // Только обновляем данные
                std::cout << "Developer deleted successfully!" << std::endl;
            }
        }
    }, ButtonOption::Animated());

    auto move_task_btn = Button("Move Task", [this] {
        if (selected_source_column != selected_destination_column && 
            !task_titles.empty() && selected_task < task_titles.size()) {
            
            std::string full_task_name = task_titles[selected_task];
            size_t pos = full_task_name.find(" (");
            if (pos != std::string::npos) {
                std::string task_title_only = full_task_name.substr(0, pos);
                std::string source_col = column_names[selected_source_column];
                std::string dest_col = column_names[selected_destination_column];
                
                try {
                    ::Task* task = search_task(*board, source_col, task_title_only);
                    Column* source_column = nullptr;
                    Column* dest_column = nullptr;
                    
                    for (auto* col : board->get_columns()) {
                        if (col->get_name() == source_col) source_column = col;
                        if (col->get_name() == dest_col) dest_column = col;
                    }
                    
                    if (source_column && dest_column && task) {
                        move_task(source_column, dest_column, task);
                        refresh_ui_data(); // Только обновляем данные
                        std::cout << "Task moved successfully!" << std::endl;
                    }
                } catch (const std::out_of_range& e) {
                    std::cout << "Error: Task not found" << std::endl;
                }
            }
        }
    }, ButtonOption::Animated());

    auto delete_task_btn = Button("Delete Task", [this] {
        if (!task_titles.empty() && selected_task < task_titles.size()) {
            std::string full_task_name = task_titles[selected_task];
            size_t pos = full_task_name.find(" (");
            if (pos != std::string::npos) {
                std::string task_title_only = full_task_name.substr(0, pos);
                std::string task_col = full_task_name.substr(pos + 2);
                task_col.pop_back();
                
                for (auto* col : board->get_columns()) {
                    if (col->get_name() == task_col) {
                        try {
                            ::Task* task = search_task(*board, task_col, task_title_only);
                            if (task) {
                                col->delete_task(task);
                                delete task;
                                refresh_ui_data(); // Только обновляем данные
                                std::cout << "Task deleted successfully!" << std::endl;
                                break;
                            }
                        } catch (const std::out_of_range& e) {
                            std::cout << "Error: Task not found" << std::endl;
                        }
                    }
                }
            }
        }
    }, ButtonOption::Animated());

    auto assign_dev_btn = Button("Assign Developer", [this] {
        if (!task_titles.empty() && !developer_names.empty() && 
            selected_task < task_titles.size() && selected_developer < developer_names.size()) {
            
            std::string full_task_name = task_titles[selected_task];
            size_t pos = full_task_name.find(" (");
            if (pos != std::string::npos) {
                std::string task_title_only = full_task_name.substr(0, pos);
                std::string task_col = full_task_name.substr(pos + 2);
                task_col.pop_back();
                
                std::string dev_name = developer_names[selected_developer];
                
                try {
                    ::Task* task = search_task(*board, task_col, task_title_only);
                    Developer* developer = nullptr;
                    
                    for (auto* dev : board->get_developer()) {
                        if (dev->get_name() == dev_name) {
                            developer = dev;
                            break;
                        }
                    }
                    
                    if (developer && task) {
                        task->set_developer(developer);
                        refresh_ui_data(); // Только обновляем данные
                        std::cout << "Developer assigned successfully!" << std::endl;
                    }
                } catch (const std::out_of_range& e) {
                    std::cout << "Error: Task not found" << std::endl;
                }
            }
        }
    }, ButtonOption::Animated());

    auto save_btn = Button("Save Board", [&] {
    previous_component = active_component; // Сохраняем текущее состояние
    active_component = 1; // Переключаемся на диалог
    is_save_dialog = true;
    file_path_input_str = std::filesystem::current_path().string();
    new_file_name.clear();
    update_file_list();
    }, ButtonOption::Animated());

    auto load_btn = Button("Load Board", [&] {
        previous_component = active_component; // Сохраняем текущее состояние
        active_component = 1; // Переключаемся на диалог
        is_save_dialog = false;
        file_path_input_str = std::filesystem::current_path().string();
        new_file_name.clear();
        update_file_list();
    }, ButtonOption::Animated());

    auto exit_btn = Button("Exit", [&] { 
        screen.Exit(); 
    }, ButtonOption::Animated());

    // Кнопки для стартового диалога
    auto new_board_btn = Button("Create New Board", [&] {
        // Оставляем текущую пустую доску
        active_component = 0; // Переходим к главному интерфейсу
        std::cout << "Created new empty board" << std::endl;
    }, ButtonOption::Animated());

    auto load_existing_btn = Button("Load Existing Board", [&] {
    previous_component = active_component; // Сохраняем текущее состояние (2 - стартовое меню)
    active_component = 1; // Переходим к диалогу загрузки
    is_save_dialog = false;
    file_path_input_str = std::filesystem::current_path().string();
    new_file_name.clear();
    update_file_list();
    }, ButtonOption::Animated());

    // Task Creation Tab
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

    // Developer Creation Tab
    auto dev_creation_add_btn = Button("Add Developer", [this] {
        if (!developer_name.empty()) {
            create_developer(*board, developer_name);
            developer_name.clear();
            refresh_ui_data(); // Только обновляем данные
            std::cout << "Developer added successfully!" << std::endl;
        }
    }, ButtonOption::Animated());

    auto dev_creation_delete_btn = Button("Delete Developer", [this] {
        if (!developer_names.empty() && selected_developer < developer_names.size()) {
            std::string dev_name = developer_names[selected_developer];
            bool found = false;
            
            auto developers = board->get_developer();
            for (auto* dev : developers) {
                if (dev->get_name() == dev_name) {
                    for (auto* col : board->get_columns()) {
                        for (auto* task : col->get_tasks()) {
                            if (task->get_developer() == dev) {
                                task->set_developer(nullptr);
                            }
                        }
                    }
                    
                    board->delete_developer(dev);
                    delete dev;
                    found = true;
                    break;
                }
            }
            
            if (found) {
                refresh_ui_data(); // Только обновляем данные
                std::cout << "Developer deleted successfully!" << std::endl;
            }
        }
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

    // Task Management Tab
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

    // Developer Assignment Tab
    auto dev_assignment_btn = Button("Assign Developer", [this] {
        if (!task_titles.empty() && !developer_names.empty() && 
            selected_task < task_titles.size() && selected_developer < developer_names.size()) {
            
            std::string full_task_name = task_titles[selected_task];
            size_t pos = full_task_name.find(" (");
            if (pos != std::string::npos) {
                std::string task_title_only = full_task_name.substr(0, pos);
                std::string task_col = full_task_name.substr(pos + 2);
                task_col.pop_back();
                
                std::string dev_name = developer_names[selected_developer];
                
                try {
                    ::Task* task = search_task(*board, task_col, task_title_only);
                    Developer* developer = nullptr;
                    
                    for (auto* dev : board->get_developer()) {
                        if (dev->get_name() == dev_name) {
                            developer = dev;
                            break;
                        }
                    }
                    
                    if (developer && task) {
                        task->set_developer(developer);
                        refresh_ui_data(); // Только обновляем данные
                        std::cout << "Developer assigned successfully!" << std::endl;
                    }
                } catch (const std::out_of_range& e) {
                    std::cout << "Error: Task not found" << std::endl;
                }
            }
        }
    }, ButtonOption::Animated());

    auto dev_assignment_task_selection = Radiobox(&task_titles, &selected_task);
    auto dev_assignment_developer_selection = Radiobox(&developer_names, &selected_developer);

    auto developer_assignment_tab = Container::Vertical({
        dev_assignment_task_selection,
        dev_assignment_developer_selection,
        dev_assignment_btn
    });

    // Control Buttons
    auto control_buttons = Container::Horizontal({
        save_btn,
        load_btn,
        exit_btn
    });

    // Диалоговые кнопки для файлового диалога
    auto refresh_dialog_btn = Button("Refresh", [this] {
        update_file_list();
    }, ButtonOption::Animated());

    // Компонент для ввода нового имени файла
    auto new_file_name_input_component = Input(&new_file_name, "Enter new file name");

    auto confirm_dialog_btn = Button(is_save_dialog ? "Save" : "Load", [&] {
        if (!file_path_input_str.empty()) {
            std::filesystem::path full_path;
            
            // Если указано имя нового файла, используем его
            if (!new_file_name.empty()) {
                full_path = std::filesystem::path(file_path_input_str) / new_file_name;
                if (full_path.extension() != ".json") {
                    full_path += ".json";
                }
            }
            // Иначе используем выбранный файл из списка
            else if (!json_files.empty() && selected_file < json_files.size()) {
                full_path = std::filesystem::path(file_path_input_str) / json_files[selected_file];
            }
            // Иначе используем введенный путь как есть
            else {
                full_path = file_path_input_str;
                if (full_path.extension() != ".json") {
                    full_path += ".json";
                }
            }
            
            // Создаем директорию, если она не существует
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
            
            if (is_save_dialog) {
                // Проверяем, существует ли файл
                bool file_exists = std::filesystem::exists(full_path);
                
                // Если файл существует, спрашиваем подтверждение перезаписи
                if (file_exists) {
                    std::cout << "File already exists. Overwriting: " << full_path.string() << std::endl;
                } else {
                    std::cout << "Creating new file: " << full_path.string() << std::endl;
                }
                
                try {
                    std::vector<std::string> current_ids;
                    for (Column* col : board->get_columns()) {
                        for (::Task* task : col->get_tasks()) {
                            current_ids.push_back(task->get_id());
                        }
                    }
                    
                    json_worker = std::make_shared<Json_worker>(full_path.string());
                    json_worker->clear_ids();
                    json_worker->board_add(*board, json_worker->ids_add(current_ids));
                    json_worker->save();
                    save_path = full_path.string();
                    std::cout << "Board successfully saved to: " << full_path.string() << std::endl;
                    
                    // Проверяем, что файл действительно создался
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
            // ДЛЯ ЗАГРУЗКИ: проверяем валидность файла
            if (!std::filesystem::exists(full_path)) {
                std::cout << "Error: File does not exist: " << full_path.string() << std::endl;
                return;
            }
            
            // Проверяем формат файла перед загрузкой
            Json_worker temp_worker(full_path.string());
            if (!temp_worker.is_valid_board_file(full_path.string())) {
                std::cout << "Error: Invalid board file format: " << full_path.string() << std::endl;
                return;
            }
            
            try {
                json_worker = std::make_shared<Json_worker>(full_path.string());
                json_worker->board_load(*board);
                initialize_board(); // Переинициализируем доску
                refresh_ui_data(); // ОБНОВЛЯЕМ ДАННЫЕ БЕЗ ПЕРЕСОЗДАНИЯ КОМПОНЕНТОВ
                save_path = full_path.string();
                std::cout << "Board successfully loaded from: " << full_path.string() << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error loading board: " << e.what() << std::endl;
                return;
            }
        }
        
        active_component = 0; // Возвращаемся к главному интерфейсу
        new_file_name.clear();
    }
}, ButtonOption::Animated());

    auto cancel_dialog_btn = Button("Cancel", [&] {
    // Возвращаемся к предыдущему состоянию
    active_component = previous_component;
    new_file_name.clear();
    file_path_input_str.clear();
    json_files.clear();
    }, ButtonOption::Animated());
    auto create_new_file_btn = Button("Create New", [&] {
        // Активируем поле ввода нового файла
        if (!new_file_name.empty()) {
            std::filesystem::path full_path = std::filesystem::path(file_path_input_str) / new_file_name;
            if (full_path.extension() != ".json") {
                full_path += ".json";
            }
            
            // Проверяем, не существует ли уже файл с таким именем
            if (std::filesystem::exists(full_path)) {
                std::cout << "File already exists: " << full_path.string() << std::endl;
            } else {
                std::cout << "Ready to create new file: " << full_path.string() << std::endl;
            }
        }
    }, ButtonOption::Animated());

    // Файловый диалог как отдельный компонент
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

    auto file_dialog_renderer = Renderer(file_dialog_component, [&] {
        Elements elements;
        
        std::string dialog_title = is_save_dialog ? "Save Board" : "Load Board";
        elements.push_back(text(dialog_title) | bold | hcenter);
        elements.push_back(separator());
        
        // Поле ввода пути
        elements.push_back(hbox({text("Path: "), file_path_input->Render()}));
        elements.push_back(separator());
        
        // Список существующих файлов
        if (json_files.empty()) {
            elements.push_back(text("No JSON files found") | center | color(Color::GrayDark));
        } else {
            elements.push_back(text("Available JSON files:"));
            elements.push_back(file_list_selection->Render() | frame | vscroll_indicator | flex);
        }
        
        elements.push_back(separator());
        
        // Секция создания нового файла (только для сохранения)
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

    // Стартовый диалог
    auto startup_buttons = Container::Horizontal({
        new_board_btn,
        load_existing_btn
    });

    auto startup_component = Container::Vertical({
        startup_buttons
    });

    auto startup_renderer = Renderer(startup_component, [&] {
        Elements elements;
        
        elements.push_back(text("SCRUM Board Manager") | bold | hcenter);
        elements.push_back(separator());
        elements.push_back(text("Welcome to SCRUM Board Manager!") | center);
        elements.push_back(separator());
        elements.push_back(text("Choose an option:") | center);
        elements.push_back(separator());
        elements.push_back(startup_buttons->Render() | center);
        
        return vbox(elements) | border | center;
    });

    // Создаем рендереры для каждой вкладки
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

    // Создаем табы для разных функций - ВОЗВРАЩАЕМ ОРИГИНАЛЬНЫЙ ДИЗАЙН
    // Первая вкладка - доска, остальные - функции управления
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
    
    // Главный компонент
    auto main_component = Container::Vertical({
        tab_selection,
        tab_container,
        control_buttons
    });

    auto main_renderer = Renderer(main_component, [&] {
        return vbox({
            text("SCRUM Board Manager") | bold | hcenter,
            separator(),
            tab_selection->Render(),
            tab_container->Render() | flex,
            separator(),
            control_buttons->Render() | center
        });
    });
    
    // Создаем общий контейнер который переключается между состояниями
    // Используем отдельные контейнеры для каждого состояния чтобы сохранить активность
    auto main_state_component = Container::Vertical({
        main_component
    });
    
    auto file_dialog_state_component = Container::Vertical({
        file_dialog_component
    });
    
    auto startup_state_component = Container::Vertical({
        startup_component
    });
    
    // Собираем все состояния в один контейнер
    auto final_component = Container::Vertical({
        main_state_component,
        file_dialog_state_component, 
        startup_state_component
    });
    
    // Рендерер который переключает между состояниями
    auto final_renderer = Renderer(final_component, [&] {
        if (active_component == 0) {
            return main_renderer->Render();
        } else if (active_component == 1) {
            return file_dialog_renderer->Render();
        } else {
            return startup_renderer->Render();
        }
    });
    
    screen.Loop(final_renderer);
}