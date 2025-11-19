#include <gtest/gtest.h>
#include <memory>
#include "column.h"
#include "task.h"
#include "board.h"

// Test fixture для тестирования класса Column
class ColumnTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создание тестовой колонки и доски
        column = std::make_unique<Column>("Test Column");
        board = std::make_unique<Board>("Test Board");
    }

    std::unique_ptr<Column> column;
    std::unique_ptr<Board> board;
};

// Тест конструктора и базовых геттеров колонки
TEST_F(ColumnTest, ConstructorAndGetters) {
    EXPECT_EQ(column->get_name(), "Test Column");  // Проверка имени
    EXPECT_TRUE(column->get_tasks().empty());      // Проверка что задач нет
}

// Тест изменения имени колонки
TEST_F(ColumnTest, SetName) {
    column->set_name("New Column Name");
    EXPECT_EQ(column->get_name(), "New Column Name");
}

// Тест добавления и получения задач из колонки
TEST_F(ColumnTest, AddAndGetTasks) {
    // Создание тестовых задач
    auto task1 = std::make_unique<Task>("Task 1");
    auto task2 = std::make_unique<Task>("Task 2");
    
    // Сохранение указателей для последующей проверки
    Task* task1_ptr = task1.get();
    Task* task2_ptr = task2.get();
    
    // Добавление задач в колонку (перемещение владения)
    column->add_task(std::move(task1));
    column->add_task(std::move(task2));
    
    // Получение ссылки на список задач
    auto& tasks = column->get_tasks();
    
    // Проверки
    EXPECT_EQ(tasks.size(), 2);                    // Должно быть 2 задачи
    EXPECT_EQ(tasks[0].get(), task1_ptr);          // Первая задача соответствует ожидаемой
    EXPECT_EQ(tasks[1].get(), task2_ptr);          // Вторая задача соответствует ожидаемой
    EXPECT_EQ(tasks[0]->get_title(), "Task 1");    // Проверка заголовка первой задачи
    EXPECT_EQ(tasks[1]->get_title(), "Task 2");    // Проверка заголовка второй задачи
}

// Тест удаления задач из колонки
TEST_F(ColumnTest, DeleteTask) {
    // Добавление тестовых задач
    auto task1 = std::make_unique<Task>("Task 1");
    auto task2 = std::make_unique<Task>("Task 2");
    
    column->add_task(std::move(task1));
    column->add_task(std::move(task2));
    
    EXPECT_EQ(column->get_tasks().size(), 2);  // Проверка начального количества
    
    // Удаление задачи по заголовку
    column->delete_task("Task 1");
    EXPECT_EQ(column->get_tasks().size(), 1);  // Должна остаться одна задача
    EXPECT_EQ(column->get_tasks()[0]->get_title(), "Task 2");  // Проверка оставшейся задачи
    
    // Попытка удаления несуществующей задачи должна вызывать исключение
    EXPECT_THROW(column->delete_task("NonExistent"), std::runtime_error);
}

// Тест поиска задач в колонке
TEST_F(ColumnTest, FindTask) {
    // Создание и добавление задач
    auto task1 = std::make_unique<Task>("Task 1");
    auto task2 = std::make_unique<Task>("Task 2");
    
    Task* task1_ptr = task1.get();
    Task* task2_ptr = task2.get();
    
    column->add_task(std::move(task1));
    column->add_task(std::move(task2));
    
    // Поиск существующих задач
    Task* found_task1 = column->find_task("Task 1");
    Task* found_task2 = column->find_task("Task 2");
    
    // Поиск несуществующей задачи
    Task* not_found = column->find_task("Task 3");
    
    // Проверки
    EXPECT_EQ(found_task1, task1_ptr);    // Найденная задача соответствует ожидаемой
    EXPECT_EQ(found_task2, task2_ptr);    // Найденная задача соответствует ожидаемой
    EXPECT_EQ(not_found, nullptr);        // Несуществующая задача не найдена
}

// Тест перемещения задачи между колонками
TEST_F(ColumnTest, MoveTaskBetweenColumns) {
    // Создание исходной и целевой колонок
    auto start_column = std::make_unique<Column>("Start");
    auto end_column = std::make_unique<Column>("End");
    
    // Создание задачи для перемещения
    auto task = std::make_unique<Task>("Movable Task");
    Task* task_ptr = task.get();
    
    // Добавление задачи в исходную колонку
    start_column->add_task(std::move(task));
    
    // Проверка начального состояния
    EXPECT_EQ(start_column->get_tasks().size(), 1);  // В исходной колонке 1 задача
    EXPECT_TRUE(end_column->get_tasks().empty());    // Целевая колонка пуста
    
    // Перемещение задачи между колонками
    move_task(start_column.get(), end_column.get(), task_ptr);
    
    // Проверка конечного состояния
    EXPECT_TRUE(start_column->get_tasks().empty());  // Исходная колонка теперь пуста
    EXPECT_EQ(end_column->get_tasks().size(), 1);    // В целевой колонке 1 задача
    EXPECT_EQ(end_column->get_tasks()[0].get(), task_ptr);  // Задача соответствует перемещенной
}

// Тест поиска задачи на всей доске
TEST_F(ColumnTest, SearchTaskInBoard) {
    // Добавление колонок на доску
    board->add_column(std::make_unique<Column>("Backlog"));
    board->add_column(std::make_unique<Column>("Done"));
    
    // Добавление задач в колонки
    auto& columns = board->get_columns();
    columns[0]->add_task(std::make_unique<Task>("Task in Backlog"));
    columns[1]->add_task(std::make_unique<Task>("Task in Done"));
    
    // Поиск задач в разных колонках
    ::Task* found_task1 = search_task(*board, "Backlog", "Task in Backlog");
    ::Task* found_task2 = search_task(*board, "Done", "Task in Done");
    
    // Проверка что задачи найдены и имеют правильные заголовки
    EXPECT_NE(found_task1, nullptr);
    EXPECT_NE(found_task2, nullptr);
    EXPECT_EQ(found_task1->get_title(), "Task in Backlog");
    EXPECT_EQ(found_task2->get_title(), "Task in Done");
    
    // Проверка обработки ошибок - поиск несуществующих задач
    EXPECT_THROW(search_task(*board, "Backlog", "NonExistent"), std::runtime_error);
    EXPECT_THROW(search_task(*board, "NonExistent", "Task"), std::runtime_error);
}