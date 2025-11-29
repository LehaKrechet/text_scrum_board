#include <gtest/gtest.h>
#include <memory>
#include "board.h"
#include "column.h"
#include "developer.h"
#include "task.h"  // Добавляем для тестирования ID

//Тест для тестирования класса Board
class BoardTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создание тестовой доски перед каждым тестом
        board = std::make_unique<Board>("Test Board");
        // Очистка статических ID перед каждым тестом
        Board::clear_used_ids();
    }

    std::unique_ptr<Board> board;
};

// Тест конструктора и базовых геттеров доски
TEST_F(BoardTest, ConstructorAndGetters) {
    EXPECT_EQ(board->get_name(), "Test Board");    // Проверка имени доски
    EXPECT_TRUE(board->get_columns().empty());     // Проверка что колонок нет
    EXPECT_TRUE(board->get_developers().empty());  // Проверка что разработчиков нет
}

// Тест изменения имени доски
TEST_F(BoardTest, SetName) {
    board->set_name("New Board Name");
    EXPECT_EQ(board->get_name(), "New Board Name");
}

// Тест добавления и получения колонок
TEST_F(BoardTest, AddAndGetColumns) {
    auto column = std::make_unique<Column>("Backlog");
    Column* column_ptr = column.get();  // Сохранение указателя для проверки
    
    // Добавление колонки на доску
    board->add_column(std::move(column));
    
    auto& columns = board->get_columns();
    
    // Проверки
    EXPECT_EQ(columns.size(), 1);                  // Должна быть одна колонка
    EXPECT_EQ(columns[0].get(), column_ptr);       // Указатель соответствует добавленной колонке
    EXPECT_EQ(columns[0]->get_name(), "Backlog");  // Имя колонки корректно
}

// Тест добавления и получения разработчиков
TEST_F(BoardTest, AddAndGetDevelopers) {
    auto developer = std::make_unique<Developer>("Aleksey");
    Developer* dev_ptr = developer.get();  // Сохранение указателя для проверки
    
    // Добавление разработчика на доску
    board->add_developer(std::move(developer));
    
    auto& developers = board->get_developers();
    
    // Проверки
    EXPECT_EQ(developers.size(), 1);              // Должен быть один разработчик
    EXPECT_EQ(developers[0].get(), dev_ptr);      // Указатель соответствует добавленному разработчику
    EXPECT_EQ(developers[0]->get_name(), "Aleksey");  // Имя разработчика корректно
}

// Тест очистки колонок
TEST_F(BoardTest, ClearColumns) {
    // Добавление тестовых колонок
    board->add_column(std::make_unique<Column>("Backlog"));
    board->add_column(std::make_unique<Column>("In Progress"));
    
    EXPECT_EQ(board->get_columns().size(), 2);  // Проверка начального количества
    
    // Очистка колонок
    board->clear_columns();
    
    // Проверка что колонки очищены
    EXPECT_TRUE(board->get_columns().empty());
}

// Тест очистки разработчиков
TEST_F(BoardTest, ClearDevelopers) {
    // Добавление тестовых разработчиков
    board->add_developer(std::make_unique<Developer>("Dev1"));
    board->add_developer(std::make_unique<Developer>("Dev2"));
    
    EXPECT_EQ(board->get_developers().size(), 2);  // Проверка начального количества
    
    // Очистка разработчиков
    board->clear_developers();
    
    // Проверка что разработчики очищены
    EXPECT_TRUE(board->get_developers().empty());
}

// Тест поиска колонки по имени
TEST_F(BoardTest, FindColumn) {
    // Создание и добавление колонок
    auto column1 = std::make_unique<Column>("Backlog");
    auto column2 = std::make_unique<Column>("Done");
    
    Column* col1_ptr = column1.get();
    Column* col2_ptr = column2.get();
    
    board->add_column(std::move(column1));
    board->add_column(std::move(column2));
    
    // Поиск существующих колонок
    Column* found_col1 = board->find_column("Backlog");
    Column* found_col2 = board->find_column("Done");
    
    // Поиск несуществующей колонки
    Column* not_found = board->find_column("NonExistent");
    
    // Проверки
    EXPECT_EQ(found_col1, col1_ptr);    // Найденная колонка соответствует ожидаемой
    EXPECT_EQ(found_col2, col2_ptr);    // Найденная колонка соответствует ожидаемой
    EXPECT_EQ(not_found, nullptr);      // Несуществующая колонка не найдена
}

// Тест поиска разработчика по имени
TEST_F(BoardTest, FindDeveloper) {
    // Создание и добавление разработчиков
    auto dev1 = std::make_unique<Developer>("Alice");
    auto dev2 = std::make_unique<Developer>("Bob");
    
    Developer* dev1_ptr = dev1.get();
    Developer* dev2_ptr = dev2.get();
    
    board->add_developer(std::move(dev1));
    board->add_developer(std::move(dev2));
    
    // Поиск существующих разработчиков
    Developer* found_dev1 = board->find_developer("Alice");
    Developer* found_dev2 = board->find_developer("Bob");
    
    // Поиск несуществующего разработчика
    Developer* not_found = board->find_developer("Charlie");
    
    // Проверки
    EXPECT_EQ(found_dev1, dev1_ptr);    // Найденный разработчик соответствует ожидаемому
    EXPECT_EQ(found_dev2, dev2_ptr);    // Найденный разработчик соответствует ожидаемому
    EXPECT_EQ(not_found, nullptr);      // Несуществующий разработчик не найден
}

// НОВЫЕ ТЕСТЫ: тестирование статических методов управления ID
TEST_F(BoardTest, StaticIdManagement) {
    // Тест добавления и проверки ID
    Board::add_used_id("test_id_1");
    Board::add_used_id("test_id_2");
    
    EXPECT_TRUE(Board::is_id_used("test_id_1"));
    EXPECT_TRUE(Board::is_id_used("test_id_2"));
    EXPECT_FALSE(Board::is_id_used("non_existent_id"));
    
    // Тест очистки ID
    Board::clear_used_ids();
    EXPECT_FALSE(Board::is_id_used("test_id_1"));
    EXPECT_FALSE(Board::is_id_used("test_id_2"));
}

// Тест интеграции Task с Board для генерации ID
TEST_F(BoardTest, TaskIdGenerationIntegration) {
    // Создание задач с использованием Board
    auto task1 = std::make_unique<Task>("Task 1", *board);
    auto task2 = std::make_unique<Task>("Task 2", *board);
    
    // Проверка что ID уникальны
    EXPECT_NE(task1->get_id(), task2->get_id());
    
    // Проверка что ID добавлены в used_ids
    EXPECT_TRUE(Board::is_id_used(task1->get_id()));
    EXPECT_TRUE(Board::is_id_used(task2->get_id()));
}

// Комплексный тест множественных операций с доской
TEST_F(BoardTest, MultipleOperations) {
    // Тест добавления нескольких колонок и разработчиков
    board->add_column(std::make_unique<Column>("Column1"));
    board->add_column(std::make_unique<Column>("Column2"));
    board->add_developer(std::make_unique<Developer>("Dev1"));
    board->add_developer(std::make_unique<Developer>("Dev2"));
    
    // Проверка количества
    EXPECT_EQ(board->get_columns().size(), 2);
    EXPECT_EQ(board->get_developers().size(), 2);
    
    // Тест изменения имени доски
    board->set_name("Updated Board");
    EXPECT_EQ(board->get_name(), "Updated Board");
    
    // Тест очистки данных
    board->clear_columns();
    board->clear_developers();
    
    // Проверка что данные очищены
    EXPECT_TRUE(board->get_columns().empty());
    EXPECT_TRUE(board->get_developers().empty());
}