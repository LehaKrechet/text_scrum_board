#include "ftxui.h"
#include "manager.h"
#include <iostream>
#include <algorithm>

using namespace ftxui;

ScrumBoardUI::ScrumBoardUI() {
    json_worker = std::make_shared<Json_worker>(save_path);
    board = std::make_shared<Board>("ScrumBoard");
    
    // Попытка загрузить существующую доску
    json_worker->board_load(*board);
    
    initialize_board();
    setup_ui_components();
}

void ScrumBoardUI::initialize_board() {
    // Добавляем стандартные колонки если их нет
    if (board->get_columns().empty()) {
        board->add_column(new Column("Backlog"));
        board->add_column(new Column("To Do"));
        board->add_column(new Column("In Progress"));
        board->add_column(new Column("Review"));
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
    
    // Отладочный вывод
    std::cout << "Developers count: " << developer_names.size() << std::endl;
    for (const auto& name : developer_names) {
        std::cout << " - " << name << std::endl;
    }
}

void ScrumBoardUI::setup_ui_components() {
    // Инициализация компонентов ввода
    task_title_input = Input(&task_title, "Enter task title");
    task_description_input = Input(&task_description, "Enter task description");
    task_priority_input = Input(&task_priority_str, "Enter task priority (0-10)");
    developer_name_input = Input(&developer_name, "Enter developer name");
    
    // Компоненты выбора
    column_selection = Radiobox(&column_names, &selected_column);
    source_column_selection = Radiobox(&column_names, &selected_source_column);
    destination_column_selection = Radiobox(&column_names, &selected_destination_column);
    task_selection = Radiobox(&task_titles, &selected_task);
    developer_selection = Radiobox(&developer_names, &selected_developer);
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
                    // Безопасное получение имени разработчика
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
            update_task_list();
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
            update_developer_list();
            std::cout << "Developer added successfully!" << std::endl;
        }
    }, ButtonOption::Animated());

    auto delete_dev_btn = Button("Delete Developer", [this] {
        if (!developer_names.empty() && selected_developer < developer_names.size()) {
            std::string dev_name = developer_names[selected_developer];
            bool found = false;
            
            // Создаем копию вектора разработчиков для безопасной итерации
            auto developers = board->get_developer();
            for (auto* dev : developers) {
                if (dev->get_name() == dev_name) {
                    // Сначала удаляем разработчика из всех задач
                    for (auto* col : board->get_columns()) {
                        for (auto* task : col->get_tasks()) {
                            if (task->get_developer() == dev) {
                                task->set_developer(nullptr);
                            }
                        }
                    }
                    
                    // Затем удаляем разработчика из доски
                    board->delete_developer(dev);
                    delete dev;
                    found = true;
                    break;
                }
            }
            
            if (found) {
                update_developer_list();
                update_task_list(); // Обновляем задачи, так как мог измениться разработчик
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
                        update_task_list();
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
                                update_task_list();
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
                        std::cout << "Developer assigned successfully!" << std::endl;
                    }
                } catch (const std::out_of_range& e) {
                    std::cout << "Error: Task not found" << std::endl;
                }
            }
        }
    }, ButtonOption::Animated());

    auto save_btn = Button("Save Board", [this] {
        std::vector<std::string> current_ids;
        for (Column* col : board->get_columns()) {
            for (::Task* task : col->get_tasks()) {
                current_ids.push_back(task->get_id());
            }
        }
        json_worker->clear_ids();
        json_worker->board_add(*board, json_worker->ids_add(current_ids));
        json_worker->save();
        std::cout << "Board saved successfully!" << std::endl;
    }, ButtonOption::Animated());

    auto load_btn = Button("Load Board", [this] {
        json_worker->board_load(*board);
        initialize_board();
        setup_ui_components();
        std::cout << "Board loaded successfully!" << std::endl;
    }, ButtonOption::Animated());

    auto exit_btn = Button("Exit", [&] { 
        screen.Exit(); 
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

    // Developer Creation Tab - создаем отдельные компоненты для этой вкладки
    auto dev_creation_add_btn = Button("Add Developer", [this] {
        if (!developer_name.empty()) {
            create_developer(*board, developer_name);
            developer_name.clear();
            update_developer_list();
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
                    // Удаляем разработчика из всех задач
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
                update_developer_list();
                update_task_list();
                std::cout << "Developer deleted successfully!" << std::endl;
            }
        }
    }, ButtonOption::Animated());

    auto developer_creation_buttons = Container::Horizontal({
        dev_creation_add_btn,
        dev_creation_delete_btn
    });

    // Создаем отдельный radiobox для вкладки управления разработчиками
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

    // Developer Assignment Tab - создаем отдельные компоненты для этой вкладки
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
                        std::cout << "Developer assigned successfully!" << std::endl;
                    }
                } catch (const std::out_of_range& e) {
                    std::cout << "Error: Task not found" << std::endl;
                }
            }
        }
    }, ButtonOption::Animated());

    // Создаем отдельные компоненты для вкладки назначения разработчиков
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

    // Создаем табы для разных функций
    Components tab_components = {
        board_renderer,
        task_creation_renderer,
        developer_creation_renderer,
        task_management_renderer,
        developer_assignment_renderer
    };
    
    auto tab_container = Container::Tab(tab_components, &current_tab);
    
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
    
    auto renderer = Renderer(main_component, [&] {
        return vbox({
            text("SCRUM Board Manager") | bold | hcenter,
            separator(),
            tab_selection->Render(),
            tab_container->Render() | flex,
            separator(),
            control_buttons->Render() | center
        });
    });
    
    screen.Loop(renderer);
}