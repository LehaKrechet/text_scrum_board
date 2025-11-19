#include <gtest/gtest.h>
#include <memory>
#include "manager.h"
#include "board.h"

// Test fixture для тестирования менеджерских функций
class ManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создание тестовой доски
        board = std::make_unique<Board>("Test Board");
        
        // Добавление стандартных колонок для тестирования
        board->add_column(std::make_unique<Column>("Backlog"));
        board->add_column(std::make_unique<Column>("In Progress"));
        board->add_column(std::make_unique<Column>("Done"));
    }

    std::unique_ptr<Board> board;
};

// Тест функции создания задачи
TEST_F(ManagerTest, CreateTask) {
    // Создание задачи в существующей колонке
    create_task(*board, "Backlog", "New Task");
    
    // Проверка что задача создана в правильной колонке
    auto& columns = board->get_columns();
    EXPECT_EQ(columns[0]->get_tasks().size(), 1);              // В колонке 1 задача
    EXPECT_EQ(columns[0]->get_tasks()[0]->get_title(), "New Task");  // Заголовок правильный
    
    // Тест создания задачи в несуществующей колонке - должно вызывать исключение
    EXPECT_THROW(create_task(*board, "NonExistent", "Task"), std::runtime_error);
    
    // Тест создания задачи с пустым заголовком - должно вызывать исключение
    EXPECT_THROW(create_task(*board, "Backlog", ""), std::invalid_argument);
}

// Тест функции создания разработчика
TEST_F(ManagerTest, CreateDeveloper) {
    // Создание разработчика
    create_developer(*board, "New Developer");
    
    // Проверка что разработчик создан
    auto& developers = board->get_developers();
    EXPECT_EQ(developers.size(), 1);                          // Один разработчик
    EXPECT_EQ(developers[0]->get_name(), "New Developer");    // Имя правильное
    
    // Тест создания разработчика с пустым именем - должно вызывать исключение
    EXPECT_THROW(create_developer(*board, ""), std::invalid_argument);
}

// Комплексный тест создания множества задач и разработчиков
TEST_F(ManagerTest, MultipleTasksAndDevelopers) {
    // Создание нескольких задач в разных колонках
    create_task(*board, "Backlog", "Task 1");
    create_task(*board, "In Progress", "Task 2");
    create_task(*board, "Done", "Task 3");
    
    // Создание нескольких разработчиков
    create_developer(*board, "Alice");
    create_developer(*board, "Bob");
    create_developer(*board, "Charlie");
    
    // Получение ссылок на колонки и разработчиков
    auto& columns = board->get_columns();
    auto& developers = board->get_developers();
    
    // Проверка количества задач в каждой колонке
    EXPECT_EQ(columns[0]->get_tasks().size(), 1);  // Backlog: 1 задача
    EXPECT_EQ(columns[1]->get_tasks().size(), 1);  // In Progress: 1 задача
    EXPECT_EQ(columns[2]->get_tasks().size(), 1);  // Done: 1 задача
    
    // Проверка количества разработчиков
    EXPECT_EQ(developers.size(), 3);  // 3 разработчика
    
    // Проверка заголовков задач
    EXPECT_EQ(columns[0]->get_tasks()[0]->get_title(), "Task 1");
    EXPECT_EQ(columns[1]->get_tasks()[0]->get_title(), "Task 2");
    EXPECT_EQ(columns[2]->get_tasks()[0]->get_title(), "Task 3");
    
    // Проверка имен разработчиков
    EXPECT_EQ(developers[0]->get_name(), "Alice");
    EXPECT_EQ(developers[1]->get_name(), "Bob");
    EXPECT_EQ(developers[2]->get_name(), "Charlie");
}