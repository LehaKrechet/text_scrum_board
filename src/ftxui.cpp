#include "ftxui.h"
#include "manager.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>

using namespace ftxui;

// Метод для создания стилизованных компонентов ввода
Component ScrumBoardUI::create_styled_input(std::string* content, const std::string& placeholder) {
    auto input = Input(content, placeholder);
    
    return Renderer(input, [input, this] {
        return input->Render() 
            | bgcolor(ftxui::Color::GrayDark);
    });

}

// Обновление данных пользовательского интерфейса
// Вызывает все методы обновления для синхронизации UI с данными
void ScrumBoardUI::refresh_ui_data() {
    update_task_list();        // Обновление списка задач
    update_developer_list();   // Обновление списка разработчиков
    
    // Обновление списка названий колонок
    column_names.clear();
    for (const auto& col : board->get_columns()) {
        column_names.push_back(col->get_name());
    }
}

// Конструктор UI - инициализирует все компоненты
ScrumBoardUI::ScrumBoardUI() {
    // Создание новой доски с именем по умолчанию
    // std::make_shared создает объект и возвращает shared_ptr
    board = std::make_shared<Board>("ScrumBoard");
    
    initialize_board();    // Инициализация начального состояния доски
    setup_ui_components(); // Настройка компонентов интерфейса
    previous_component = 2; // Установка начального состояния (стартовый экран)
}

// Инициализация доски начальными данными
// Создает стандартные колонки если доска пустая
void ScrumBoardUI::initialize_board() {
    // Если колонок нет - создаем стандартный набор
    // Это типичные колонки для Scrum доски
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
// Собирает все задачи со всех колонок и форматирует для показа
void ScrumBoardUI::update_task_list() {
    task_titles.clear();
    
    // Сбор всех задач со всех колонок
    for (const auto& col : board->get_columns()) {
        for (const auto& task : col->get_tasks()) {
            // Формат: "Название задачи (Название колонки)"
            // Это помогает пользователю видеть в какой колонке находится задача
            task_titles.push_back(task->get_title() + " (" + col->get_name() + ")");
        }
    }
    
    // Корректировка выбранной задачи если необходимо
    // Защита от выхода за границы массива при удалении задач
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
    // Защита от выхода за границы массива при удалении разработчиков
    if (!developer_names.empty() && selected_developer >= developer_names.size()) {
        selected_developer = 0;
    } else if (developer_names.empty()) {
        selected_developer = 0;
    }
}

// Обновление списка JSON файлов в текущей директории
// Используется для диалога сохранения/загрузки
void ScrumBoardUI::update_file_list() {
    json_files.clear();
    
    // Если путь пустой, нечего обновлять
    if (file_path_input_str.empty()) {
        return;
    }
    
    try {
        // Создаем путь из строки
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
            
            // Ищем JSON файлы в родительской директории
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
                
                // Ищем JSON файлы в родительской директории
                for (const auto& entry : std::filesystem::directory_iterator(parent_path)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".json") {
                        json_files.push_back(entry.path().filename().string());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        // Обработка ошибок доступа к файловой системе
        std::cout << "Error reading directory: " << e.what() << std::endl;
    }
    
    // Корректировка выбранного файла если необходимо
    // Защита от выхода за границы массива
    if (!json_files.empty() && selected_file >= json_files.size()) {
        selected_file = 0;
    } else if (json_files.empty()) {
        selected_file = 0;
    }
}

// Настройка всех компонентов пользовательского интерфейса
// Создает и настраивает все элементы управления FTXUI
void ScrumBoardUI::setup_ui_components() {
    // Создание стилизованных компонентов ввода с адаптивным цветом
    task_title_input = create_styled_input(&task_title, "Enter task title");
    task_description_input = create_styled_input(&task_description, "Enter task description");
    task_priority_input = create_styled_input(&task_priority_str, "Enter task priority (0-10)");
    developer_name_input = create_styled_input(&developer_name, "Enter developer name");
    file_path_input = create_styled_input(&file_path_input_str, "Enter file path");
    
    // Создание компонентов выбора
    // Radiobox компоненты позволяют выбирать из списка вариантов
    // Стилизуем их также для адаптивного цвета
    column_selection = Radiobox(&column_names, &selected_column);
    source_column_selection = Radiobox(&column_names, &selected_source_column);
    destination_column_selection = Radiobox(&column_names, &selected_destination_column);
    task_selection = Radiobox(&task_titles, &selected_task);
    developer_selection = Radiobox(&developer_names, &selected_developer);
    file_list_selection = Radiobox(&json_files, &selected_file);
    
    // Стилизация компонентов выбора
    auto style_radiobox = [](Component radiobox) {
        return Renderer(radiobox, [radiobox] {
            return radiobox->Render() | color(Color::Default);
        });
    };
    
    column_selection = style_radiobox(column_selection);
    source_column_selection = style_radiobox(source_column_selection);
    destination_column_selection = style_radiobox(destination_column_selection);
    task_selection = style_radiobox(task_selection);
    developer_selection = style_radiobox(developer_selection);
    file_list_selection = style_radiobox(file_list_selection);
}

// Обработчик создания новой задачи
// Вызывается при нажатии кнопки "Create Task"
void ScrumBoardUI::handle_create_task() {
    // Проверяем что заголовок задачи не пустой
    if (!task_title.empty()) {
        // Получаем название выбранной колонки
        std::string column_name = column_names[selected_column];
        
        try {
            // Создание задачи через менеджер
            // Менеджер инкапсулирует логику создания и добавления задачи
            create_task(*board, column_name, task_title);
            
            // Поиск созданной задачи для установки дополнительных полей
            // Задача только что создана, поэтому она должна существовать
            ::Task* task_ptr = search_task(*board, column_name, task_title);
            if (task_ptr) {
                // Устанавливаем описание задачи
                task_ptr->set_description(task_description);
                
                // Установка приоритета с валидацией
                if (!task_priority_str.empty()) {
                    try {
                        // Преобразуем строку в число
                        int priority = std::stoi(task_priority_str);
                        // Ограничиваем приоритет диапазоном 0-10
                        // std::max и std::min гарантируют что значение в пределах 0-10
                        task_ptr->set_priority(std::max(0, std::min(10, priority)));
                    } catch (const std::exception& e) {
                        // Если преобразование не удалось, устанавливаем приоритет по умолчанию
                        task_ptr->set_priority(0);
                    }
                }
                std::cout << "Task created successfully!" << std::endl;
            }
            
            // Очистка полей ввода после успешного создания
            task_title.clear();
            task_description.clear();
            task_priority_str.clear();
            
            refresh_ui_data(); // Обновление UI для отображения новой задачи
        } catch (const std::exception& e) {
            std::cout << "Error creating task: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Error: Task title cannot be empty" << std::endl;
    }
}

// Обработчик перемещения задачи между колонками
// Вызывается при нажатии кнопки "Move Task"
void ScrumBoardUI::handle_move_task() {
    // Проверяем условия для перемещения:
    // - исходная и целевая колонки разные
    // - есть задачи для перемещения  
    // - выбранная задача существует
    if (selected_source_column != selected_destination_column && 
        !task_titles.empty() && selected_task < task_titles.size()) {
        
        // Извлечение названия задачи и колонок из форматированной строки
        // Формат строки: "Название задачи (Название колонки)"
        std::string full_task_name = task_titles[selected_task];
        size_t pos = full_task_name.find(" (");
        if (pos != std::string::npos) {
            // Извлекаем только название задачи (до открывающей скобки)
            std::string task_title_only = full_task_name.substr(0, pos);
            // Получаем названия исходной и целевой колонок из списка
            std::string source_col = column_names[selected_source_column];
            std::string dest_col = column_names[selected_destination_column];
            
            try {
                // Поиск задачи в исходной колонке
                ::Task* task_ptr = search_task(*board, source_col, task_title_only);
                // Находим указатели на исходную и целевую колонки
                Column* source_column = board->find_column(source_col);
                Column* dest_column = board->find_column(dest_col);
                
                // Если все объекты найдены, выполняем перемещение
                if (source_column && dest_column && task_ptr) {
                    move_task(source_column, dest_column, task_ptr);
                    refresh_ui_data(); // Обновляем интерфейс после перемещения
                    std::cout << "Task moved successfully!" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error moving task: " << e.what() << std::endl;
            }
        }
    }
}

// Обработчик удаления задачи
// Вызывается при нажатии кнопки "Delete Task"
void ScrumBoardUI::handle_delete_task() {
    // Проверяем что есть задачи и выбранная задача существует
    if (!task_titles.empty() && selected_task < task_titles.size()) {
        std::string full_task_name = task_titles[selected_task];
        size_t pos = full_task_name.find(" (");
        if (pos != std::string::npos) {
            // Извлечение названия задачи и колонки из форматированной строки
            std::string task_title_only = full_task_name.substr(0, pos);
            std::string task_col = full_task_name.substr(pos + 2);
            task_col.pop_back(); // Удаление закрывающей скобки
            
            // Поиск колонки по имени
            Column* column = board->find_column(task_col);
            if (column) {
                try {
                    // Удаление задачи из колонки
                    column->delete_task(task_title_only);
                    refresh_ui_data(); // Обновление UI после удаления
                    std::cout << "Task deleted successfully!" << std::endl;
                } catch (const std::exception& e) {
                    std::cout << "Error deleting task: " << e.what() << std::endl;
                }
            }
        }
    }
}

// Обработчик добавления разработчика
// Вызывается при нажатии кнопки "Add Developer"
void ScrumBoardUI::handle_add_developer() {
    // Проверяем что имя разработчика не пустое
    if (!developer_name.empty()) {
        try {
            // Создание разработчика через менеджер
            create_developer(*board, developer_name);
            developer_name.clear(); // Очистка поля ввода
            refresh_ui_data(); // Обновление списка разработчиков
            std::cout << "Developer added successfully!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error adding developer: " << e.what() << std::endl;
        }
    }
}

// Обработчик удаления разработчика
// Вызывается при нажатии кнопки "Delete Developer"
void ScrumBoardUI::handle_delete_developer() {
    // Проверяем что есть разработчики и выбранный существует
    if (!developer_names.empty() && selected_developer < developer_names.size()) {
        std::string dev_name = developer_names[selected_developer];
        
        // Поиск разработчика на доске
        Developer* developer = board->find_developer(dev_name);
        if (developer) {
            try {
                // Удаление разработчика из всех задач
                // Проходим по всем колонкам и всем задачам
                for (const auto& col : board->get_columns()) {
                    for (const auto& task : col->get_tasks()) {
                        // Если задача назначена на этого разработчика
                        if (task->get_developer() == developer) {
                            // Снимаем назначение
                            task->set_developer(nullptr);
                        }
                    }
                }
                
                // Удаление разработчика из доски
                auto& developers = board->get_developers();
                // Ищем разработчика в списке по указателю
                auto it = std::find_if(developers.begin(), developers.end(),
                    [&](const std::unique_ptr<Developer>& dev) {
                        return dev.get() == developer;
                    });
                
                // Если разработчик найден, удаляем его
                if (it != developers.end()) {
                    developers.erase(it);
                    refresh_ui_data(); // Обновление UI
                    std::cout << "Developer deleted successfully!" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error deleting developer: " << e.what() << std::endl;
            }
        }
    }
}

// Обработчик назначения разработчика на задачу
// Вызывается при нажатии кнопки "Assign Developer"
void ScrumBoardUI::handle_assign_developer() {
    // Проверяем что есть задачи и разработчики, и выбранные существуют
    if (!task_titles.empty() && !developer_names.empty() && 
        selected_task < task_titles.size() && selected_developer < developer_names.size()) {
        
        std::string full_task_name = task_titles[selected_task];
        size_t pos = full_task_name.find(" (");
        if (pos != std::string::npos) {
            // Извлечение названия задачи и колонки
            std::string task_title_only = full_task_name.substr(0, pos);
            std::string task_col = full_task_name.substr(pos + 2);
            task_col.pop_back(); // Удаление закрывающей скобки
            
            std::string dev_name = developer_names[selected_developer];
            
            try {
                // Поиск задачи и разработчика
                ::Task* task_ptr = search_task(*board, task_col, task_title_only);
                Developer* developer = board->find_developer(dev_name);
                
                // Назначение разработчика на задачу
                if (developer && task_ptr) {
                    task_ptr->set_developer(developer);
                    refresh_ui_data(); // Обновление UI
                    std::cout << "Developer assigned successfully!" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error assigning developer: " << e.what() << std::endl;
            }
        }
    }
}

// Обработчик диалога сохранения/загрузки доски
// Универсальный метод для обоих операций
void ScrumBoardUI::handle_save_load_dialog(bool is_save, const std::string& new_file_name, int selected_file) {
    // Проверяем что путь не пустой
    if (!file_path_input_str.empty()) {
        std::filesystem::path full_path;
        
        // Формирование полного пути к файлу с учетом разных сценариев
        if (!new_file_name.empty()) {
            // Если указано новое имя файла, объединяем путь и имя
            full_path = std::filesystem::path(file_path_input_str) / new_file_name;
            // Гарантируем, что у файла будет расширение .json
            if (full_path.extension() != ".json") {
                full_path += ".json";
            }
        } else if (!json_files.empty() && selected_file < json_files.size()) {
            // Если выбран существующий файл из списка
            full_path = std::filesystem::path(file_path_input_str) / json_files[selected_file];
        } else {
            // Используем введенный путь как есть
            full_path = file_path_input_str;
            if (full_path.extension() != ".json") {
                full_path += ".json";
            }
        }
        
        // Создание директорий если необходимо (для операции сохранения)
        std::filesystem::path parent_dir = full_path.parent_path();
        if (!parent_dir.empty() && !std::filesystem::exists(parent_dir)) {
            try {
                // Рекурсивно создаем все недостающие директории
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
                // Сбор ID всех задач для сохранения (нужно для отслеживания уникальности)
                std::vector<std::string> current_ids;
                for (const auto& col : board->get_columns()) {
                    for (const auto& task : col->get_tasks()) {
                        current_ids.push_back(task->get_id());
                    }
                }
                
                // Инициализируем JSON worker с путем для сохранения
                json_worker = std::make_shared<Json_worker>(full_path.string());
                // Очищаем предыдущие ID
                json_worker->clear_ids();
                // Добавляем доску в JSON с текущими ID задач
                json_worker->board_add(*board, json_worker->ids_add(current_ids));
                // Сохраняем в файл
                json_worker->save();
                save_path = full_path.string();
                
                // Установка имени доски из имени файла (без расширения)
                std::string board_name = full_path.stem().string();
                board->set_name(board_name);
                
                std::cout << "Board successfully saved to: " << full_path.string() << std::endl;
                std::cout << "Board name set to: " << board_name << std::endl;
                
                // Проверка существования файла для подтверждения успешного сохранения
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
            // Проверяем существование файла перед загрузкой
            if (!std::filesystem::exists(full_path)) {
                std::cout << "Error: File does not exist: " << full_path.string() << std::endl;
                return;
            }
            
            // Проверяем валидность файла перед загрузкой
            Json_worker temp_worker(full_path.string());
            if (!temp_worker.is_valid_board_file(full_path.string())) {
                std::cout << "Error: Invalid board file format: " << full_path.string() << std::endl;
                return;
            }
            
            try {
                // Инициализируем JSON worker для загрузки
                json_worker = std::make_shared<Json_worker>(full_path.string());
                
                // Установка имени доски из имени файла
                std::string board_name = full_path.stem().string();
                
                // Загружаем данные доски из JSON
                json_worker->board_load(*board);
                
                // Устанавливаем имя доски
                board->set_name(board_name);
                
                // Инициализируем и обновляем UI после загрузки
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
// Создает визуальное представление Scrum доски
Element ScrumBoardUI::render_board() {
    Elements column_elements;
    auto text_color = get_text_color();
    
    // Проходим по всем колонкам доски
    for (const auto& column : board->get_columns()) {
        Elements task_elements;
        
        // Заголовок колонки с названием
        task_elements.push_back(text(column->get_name()) | bold | center | color(text_color));
        // Разделительная линия под заголовком
        task_elements.push_back(separator());
        
        auto& tasks = column->get_tasks();
        // Обработка пустой колонки
        if (tasks.empty()) {
            // Сообщение о отсутствии задач + занимает пространство
            task_elements.push_back(text("No tasks") | center | flex | size(HEIGHT, EQUAL, 10) | color(text_color));
        } else {
            int task_count = tasks.size();
            
            // УРОВНИ ДЕТАЛИЗАЦИИ:
            // Определяем сколько информации показывать в зависимости от количества задач
            int detail_level;
            if (task_count <= 3) detail_level = 4;      // Вся информация
            else if (task_count <= 4) detail_level = 3; // Почти вся информация  
            else if (task_count <= 5) detail_level = 2; // Основная информация
            else if (task_count <= 6) detail_level = 1; // Минимум информации
            else detail_level = 0;                      // Только заголовок
            
            // Высота задачи в зависимости от уровня детализации
            int task_height;
            switch (detail_level) {
                case 4: task_height = 8; break; // Высокая - полная информация
                case 3: task_height = 6; break; // Средняя - описание + разработчик + приоритет
                case 2: task_height = 5; break; // Компактная - разработчик + приоритет
                case 1: task_height = 4; break; // Минимальная - только разработчик
                case 0: task_height = 3; break; // Ультра-компактная - только заголовок
            }
            
            // Отрисовка каждой задачи в колонке
            for (size_t i = 0; i < tasks.size(); ++i) {
                const auto& task = tasks[i];
                
                // Получение имени разработчика
                std::string developer_name = "Unassigned";
                if (task->get_developer()) {
                    developer_name = task->get_developer()->get_name();
                }
                
                Elements task_content;
                
                // ЗАГОЛОВОК ЗАДАЧИ - отображается всегда
                task_content.push_back(text("📝 " + task->get_title()) | bold | center | color(text_color));
                
                // УРОВЕНЬ 1+: Разработчик
                if (detail_level >= 1 && task->get_developer()!=nullptr) {
                    task_content.push_back(separator()); // Разделитель
                    task_content.push_back(text("👨 " + developer_name) | center | color(text_color));
                }
                
                // УРОВЕНЬ 2+: Приоритет
                if (detail_level >= 2 && task->get_priority() != -1) {
                    task_content.push_back(text("🎯 " + std::to_string(task->get_priority())) | center | color(text_color));
                }
                
                // УРОВЕНЬ 3+: Описание (если есть)
                if (detail_level >= 3 && !task->get_description().empty()) {
                    std::string desc = task->get_description();
                    // Обрезаем длинные описания
                    if (desc.length() > 20) desc = desc.substr(0, 17) + "...";
                    task_content.push_back(text("📋 " + desc) | center | color(text_color));
                }
                
                // Создание элемента задачи
                auto task_element = vbox(std::move(task_content)) 
                    | border          // Рамка вокруг задачи
                    | size(HEIGHT, EQUAL, task_height); // Фиксированная высота
                    
                task_elements.push_back(task_element);
                
                // Добавляем отступ между задачами (кроме последней)
                if (i < tasks.size() - 1) task_elements.push_back(filler());
            }
        }
        
        // СОЗДАНИЕ КОЛОНКИ:
        // Каждая колонка получает равную долю пространства
        column_elements.push_back(
            vbox(std::move(task_elements)) 
            | border        // Рамка вокруг колонки
            | flex          // Растягивается по вертикали
            | frame         // Обрамление
            | vscroll_indicator // Индикатор прокрутки если контент не помещается
        );
    }
    
    // ГЛАВНЫЙ КОНТЕЙНЕР:
    // Простой и эффективный способ занять всю ширину
    return vbox({
        // Заголовок доски
        text("SCRUM Board - " + board->get_name()) | bold | hcenter | color(text_color),
        // Разделитель
        separator(),
        // Горизонтальное расположение колонок
        hbox(std::move(column_elements)) 
            | flex    // Растягивается по вертикали
            | xflex   // Растягивается по горизонтали (равномерное распределение колонок)
    }) 
    | flex   // Главный контейнер растягивается
    | xflex; // Занимает всю ширину терминала
}

// Основной метод запуска приложения
// Создает UI и запускает главный цикл обработки событий
void ScrumBoardUI::run() {
    // Создание интерактивного экрана
    // Fullscreen - занимает весь терминал
    auto screen = ScreenInteractive::Fullscreen();
    
    // Переменные состояния UI для диалоговых окон
    int active_component = 2;  // 0: main, 1: file dialog, 2: startup
    bool is_save_dialog = false; // true для сохранения, false для загрузки
    std::string new_file_name;   // Имя нового файла для сохранения
    
    // Создание кнопок с обработчиками
    // ButtonOption::Animated() добавляет анимацию при наведении
    
    // Кнопка создания задачи
    auto create_task_btn = Button("Create Task", [this] {
        handle_create_task();
    }, ButtonOption::Animated());

    // Кнопка очистки полей ввода задачи
    auto clear_task_btn = Button("Clear", [this] {
        task_title.clear();
        task_description.clear();
        task_priority_str.clear();
    }, ButtonOption::Animated());

    // Кнопка перемещения задачи
    auto move_task_btn = Button("Move Task", [this] {
        handle_move_task();
    }, ButtonOption::Animated());

    // Кнопка удаления задачи
    auto delete_task_btn = Button("Delete Task", [this] {
        handle_delete_task();
    }, ButtonOption::Animated());

    // Кнопка сохранения доски
    auto save_btn = Button("Save Board", [&] {
        previous_component = active_component;
        active_component = 1;  // Переходим к диалогу файлов
        is_save_dialog = true; // Режим сохранения
        file_path_input_str = std::filesystem::current_path().string(); // Текущая директория
        new_file_name.clear();
        update_file_list(); // Обновляем список файлов
    }, ButtonOption::Animated());

    // Кнопка загрузки доски
    auto load_btn = Button("Load Board", [&] {
        previous_component = active_component;
        active_component = 1;  // Переходим к диалогу файлов
        is_save_dialog = false; // Режим загрузки
        file_path_input_str = std::filesystem::current_path().string(); // Текущая директория
        new_file_name.clear();
        update_file_list(); // Обновляем список файлов
    }, ButtonOption::Animated());

    // Кнопка выхода из приложения
    auto exit_btn = Button("Exit", [&] { 
        screen.Exit(); // Завершаем главный цикл
    }, ButtonOption::Animated());

    // Кнопка создания новой доски
    auto new_board_btn = Button("Create New Board", [&] {
        // Создаем совершенно новую доску
        board = std::make_shared<Board>("ScrumBoard");
        initialize_board(); // Инициализируем стандартными колонками
        active_component = 0; // Переходим к главному интерфейсу
        std::cout << "Created new empty board" << std::endl;
    }, ButtonOption::Animated());

    // Кнопка загрузки существующей доски
    auto load_existing_btn = Button("Load Existing Board", [&] {
        previous_component = active_component;
        active_component = 1;  // Переходим к диалогу файлов
        is_save_dialog = false; // Режим загрузки
        file_path_input_str = std::filesystem::current_path().string(); // Текущая директория
        new_file_name.clear();
        update_file_list(); // Обновляем список файлов
    }, ButtonOption::Animated());

    // Компоновка компонентов в контейнеры
    // Container::Horizontal - располагает компоненты горизонтально
    // Container::Vertical - располагает компоненты вертикально
    
    // Контейнер для кнопок создания задачи
    auto task_creation_buttons = Container::Horizontal({
        create_task_btn,
        clear_task_btn
    });

    // Контейнер для всей вкладки создания задач
    auto task_creation_tab = Container::Vertical({
        task_title_input,
        task_description_input,
        task_priority_input,
        column_selection,
        task_creation_buttons
    });

    // Кнопка добавления разработчика
    auto dev_creation_add_btn = Button("Add Developer", [this] {
        handle_add_developer();
    }, ButtonOption::Animated());

    // Кнопка удаления разработчика
    auto dev_creation_delete_btn = Button("Delete Developer", [this] {
        handle_delete_developer();
    }, ButtonOption::Animated());

    // Контейнер для кнопок управления разработчиками
    auto developer_creation_buttons = Container::Horizontal({
        dev_creation_add_btn,
        dev_creation_delete_btn
    });

    // Выбор разработчика из списка
    auto developer_creation_selection = Radiobox(&developer_names, &selected_developer);

    // Контейнер для всей вкладки управления разработчиками
    auto developer_creation_tab = Container::Vertical({
        developer_name_input,
        developer_creation_selection,
        developer_creation_buttons
    });
    
    // Контейнер для кнопок управления задачами
    auto task_management_buttons = Container::Horizontal({
        move_task_btn,
        delete_task_btn
    });

    // Контейнер для вкладки управления задачами
    auto task_management_tab = Container::Vertical({
        source_column_selection,
        destination_column_selection,
        task_selection,
        task_management_buttons
    });
    
    // Кнопка назначения разработчика
    auto dev_assignment_btn = Button("Assign Developer", [this] {
        handle_assign_developer();
    }, ButtonOption::Animated());

    // Выбор задачи для назначения
    auto dev_assignment_task_selection = Radiobox(&task_titles, &selected_task);
    // Выбор разработчика для назначения
    auto dev_assignment_developer_selection = Radiobox(&developer_names, &selected_developer);

    // Контейнер для вкладки назначения разработчиков
    auto developer_assignment_tab = Container::Vertical({
        dev_assignment_task_selection,
        dev_assignment_developer_selection,
        dev_assignment_btn
    });
  
    // Контейнер для основных контрольных кнопок
    auto control_buttons = Container::Horizontal({
        save_btn,
        load_btn,
        exit_btn
    });

    // Кнопка обновления списка файлов
    auto refresh_dialog_btn = Button("Refresh", [this] {
        update_file_list();
    }, ButtonOption::Animated());

    // Поле ввода имени нового файла
    auto new_file_name_input_component = Input(&new_file_name, "Enter new file name") | bgcolor(ftxui::Color::GrayDark);

    // Кнопка подтверждения в диалоге (Сохранить/Загрузить)
    auto confirm_dialog_btn = Button(is_save_dialog ? "Save" : "Load", [&] {
        handle_save_load_dialog(is_save_dialog, new_file_name, selected_file);
        active_component = 0; // Возвращаемся к главному интерфейсу
        new_file_name.clear(); // Очищаем имя файла
    }, ButtonOption::Animated());

    // Кнопка отмены в диалоге
    auto cancel_dialog_btn = Button("Cancel", [&] {
        active_component = previous_component; // Возвращаемся к предыдущему состоянию
        new_file_name.clear();
        file_path_input_str.clear();
        json_files.clear();
    }, ButtonOption::Animated());
    
    // Кнопка создания нового файла
    auto create_new_file_btn = Button("Create New", [&] {
        if (!new_file_name.empty()) {
            // Формируем полный путь к новому файлу
            std::filesystem::path full_path = std::filesystem::path(file_path_input_str) / new_file_name;
            if (full_path.extension() != ".json") {
                full_path += ".json";
            }
            
            // Проверяем существует ли файл
            if (std::filesystem::exists(full_path)) {
                std::cout << "File already exists: " << full_path.string() << std::endl;
            } else {
                std::cout << "Ready to create new file: " << full_path.string() << std::endl;
            }
        }
    }, ButtonOption::Animated());

    // Контейнер для кнопок диалога файлов
    auto file_dialog_buttons = Container::Horizontal({
        confirm_dialog_btn,
        cancel_dialog_btn,
        refresh_dialog_btn
    });

    // Контейнер для всего диалога файлов
    auto file_dialog_component = Container::Vertical({
        file_path_input,
        file_list_selection,
        new_file_name_input_component,
        create_new_file_btn,
        file_dialog_buttons
    });

    // Рендереры для разных состояний UI
    // Renderer связывает компоненты с функциями отрисовки
    
    // Рендерер для диалога файлов
    auto file_dialog_renderer = Renderer(file_dialog_component, [&] {
        Elements elements;
        auto text_color = get_text_color();
        
        // Заголовок диалога в зависимости от режима
        std::string dialog_title = is_save_dialog ? "Save Board" : "Load Board";
        elements.push_back(text(dialog_title) | bold | hcenter | color(text_color));
        elements.push_back(separator());
        
        // Поле ввода пути
        elements.push_back(hbox({text("Path: ") | color(text_color), file_path_input->Render()}));
        elements.push_back(separator());
        
        // Список файлов или сообщение если файлов нет
        if (json_files.empty()) {
            elements.push_back(text("No JSON files found") | center | color(Color::GrayDark));
        } else {
            elements.push_back(text("Available JSON files:") | color(text_color));
            elements.push_back(file_list_selection->Render() | frame | vscroll_indicator | flex);
        }
        
        elements.push_back(separator());
        
        // Для режима сохранения - возможность создать новый файл
        if (is_save_dialog) {
            elements.push_back(text("Create New File:") | bold | color(text_color));
            elements.push_back(hbox({text("File name: ") | color(text_color), new_file_name_input_component->Render()}));
            elements.push_back(create_new_file_btn->Render() | center);
            elements.push_back(separator());
        }
        
        // Отображение текущего выбора
        elements.push_back(text("Selection: " + (new_file_name.empty() ? 
            (json_files.empty() ? "No file selected" : json_files[selected_file]) : 
            "New file: " + new_file_name)) | color(text_color));
        elements.push_back(separator());
        
        // Кнопки действий
        elements.push_back(file_dialog_buttons->Render() | center);
        
        return vbox(elements) | border | center;
    });

    // Контейнер для стартовых кнопок
    auto startup_buttons = Container::Horizontal({
        new_board_btn,
        load_existing_btn
    });

    // Контейнер для стартового экрана
    auto startup_component = Container::Vertical({
        startup_buttons
    });

    // Рендерер для стартового экрана
    auto startup_renderer = Renderer(startup_component, [&] {
        Elements elements;
        auto text_color = get_text_color();
        
        elements.push_back(text("SCRUM Board") | bold | hcenter | color(text_color));
        elements.push_back(separator());
        elements.push_back(text("Welcome to SCRUM Board!") | center | color(text_color));
        elements.push_back(separator());
        elements.push_back(text("Choose an option:") | center | color(text_color));
        elements.push_back(separator());
        elements.push_back(startup_buttons->Render() | center);
        
        return vbox(elements) | border | center;
    });

    // Рендерер для отображения доски
    auto board_renderer = Renderer([this] { 
        return render_board(); 
    });

    // Рендерер для вкладки создания задач
    auto task_creation_renderer = Renderer(task_creation_tab, [this, task_creation_buttons] {
        Elements elements;
        auto text_color = get_text_color();
        
        elements.push_back(text("Create New Task") | bold | hcenter | color(text_color));
        elements.push_back(separator());
        elements.push_back(hbox({text("Title: ") | color(text_color), task_title_input->Render()}));
        elements.push_back(hbox({text("Description: ") | color(text_color), task_description_input->Render()}));
        elements.push_back(hbox({text("Priority (0-10): ") | color(text_color), task_priority_input->Render()}));
        elements.push_back(text("Select Column:") | color(text_color));
        elements.push_back(column_selection->Render());
        elements.push_back(separator());
        elements.push_back(task_creation_buttons->Render() | center);
        return vbox(elements) | border;
    });

    // Рендерер для вкладки управления разработчиками
    auto developer_creation_renderer = Renderer(developer_creation_tab, [this, developer_creation_buttons, developer_creation_selection] {
        Elements elements;
        auto text_color = get_text_color();
        
        elements.push_back(text("Manage Developers") | bold | hcenter | color(text_color));
        elements.push_back(separator());
        elements.push_back(hbox({text("Developer Name: ") | color(text_color), developer_name_input->Render()}));
        elements.push_back(separator());
        elements.push_back(text("Current Developers:") | color(text_color));
        elements.push_back(developer_creation_selection->Render());
        elements.push_back(separator());
        elements.push_back(developer_creation_buttons->Render() | center);
        return vbox(elements) | border;
    });

    // Рендерер для вкладки управления задачами
    auto task_management_renderer = Renderer(task_management_tab, [this, task_management_buttons] {
        Elements elements;
        auto text_color = get_text_color();
        
        elements.push_back(text("Manage Tasks") | bold | hcenter | color(text_color));
        elements.push_back(separator());
        elements.push_back(text("Move Task Between Columns") | color(text_color));
        elements.push_back(hbox({
            vbox({text("From:") | color(text_color), source_column_selection->Render()}),
            vbox({text("To:") | color(text_color), destination_column_selection->Render()})
        }));
        elements.push_back(separator());
        elements.push_back(text("Available Tasks:") | color(text_color));
        elements.push_back(task_selection->Render());
        elements.push_back(separator());
        elements.push_back(task_management_buttons->Render() | center);
        return vbox(elements) | border;
    });

    // Рендерер для вкладки назначения разработчиков
    auto developer_assignment_renderer = Renderer(developer_assignment_tab, [this, dev_assignment_btn, dev_assignment_task_selection, dev_assignment_developer_selection] {
        Elements elements;
        auto text_color = get_text_color();
        
        elements.push_back(text("Assign Developer to Task") | bold | hcenter | color(text_color));
        elements.push_back(separator());
        
        // Горизонтальное расположение выбора задачи и разработчика
        Elements task_dev_elements;
        task_dev_elements.push_back(vbox({
            text("Select Task:") | color(text_color),
            dev_assignment_task_selection->Render()
        }));
        task_dev_elements.push_back(vbox({
            text("Select Developer:") | color(text_color),
            dev_assignment_developer_selection->Render()
        }));
        
        elements.push_back(hbox(std::move(task_dev_elements)));
        elements.push_back(separator());
        elements.push_back(dev_assignment_btn->Render() | center);
        return vbox(elements) | border;
    });

    // Создание системы вкладок
    // Container::Tab позволяет переключаться между разными вкладками
    std::vector<Component> tab_content_components = {
        board_renderer,
        task_creation_renderer,
        developer_creation_renderer,
        task_management_renderer,
        developer_assignment_renderer
    };
    
    // Контейнер вкладок
    auto tab_container = Container::Tab(tab_content_components, &current_tab);
    
    // Заголовки вкладок
    std::vector<std::string> tab_entries = {
        "Board View",
        "Create Task", 
        "Manage Developers",
        "Manage Tasks",
        "Assign Developer"
    };
    
    // Компонент переключения вкладок
    auto tab_selection = Toggle(&tab_entries, &current_tab);
    
    // Основной компонент приложения
    auto main_component = Container::Vertical({
        tab_selection,
        tab_container,
        control_buttons
    });

    // Рендерер для главного интерфейса
    auto main_renderer = Renderer(main_component, [&] {
        auto text_color = get_text_color();
        
        return vbox({
            text("SCRUM Board") | bold | hcenter | color(text_color),
            separator(),
            tab_selection->Render(),
            tab_container->Render() | flex,
            separator(),
            control_buttons->Render() | center
        });
    });
    
    // Финальная компоновка всех компонентов с учетом состояний
    auto main_state_component = Container::Vertical({
        main_component
    });
    
    auto file_dialog_state_component = Container::Vertical({
        file_dialog_component
    });
    
    auto startup_state_component = Container::Vertical({
        startup_component
    });
    
    // Финальный компонент который управляет всеми состояниями
    auto final_component = Container::Vertical({
        main_state_component,
        file_dialog_state_component, 
        startup_state_component
    });
    
    // Финальный рендерер с переключением между состояниями
    auto final_renderer = Renderer(final_component, [&] {
        if (active_component == 0) {
            return main_renderer->Render(); // Главный интерфейс
        } else if (active_component == 1) {
            return file_dialog_renderer->Render(); // Диалог файлов
        } else {
            return startup_renderer->Render(); // Стартовый экран
        }
    });
    
    // Запуск основного цикла приложения
    // Loop обрабатывает ввод пользователя и перерисовывает экран
    screen.Loop(final_renderer);
}