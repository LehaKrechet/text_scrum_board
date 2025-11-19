#include <gtest/gtest.h>
#include <memory>
#include "task.h"
#include "developer.h"

// Test fixture для тестирования класса Task
class TaskTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Очистка статических ID перед каждым тестом для изоляции
        Task::clear_used_ids();
        
        // Создание тестовой задачи и разработчика
        task = std::make_unique<Task>("Test Task");
        developer = std::make_unique<Developer>("Test Developer");
    }

    // Общие ресурсы для тестов Task
    std::unique_ptr<Task> task;
    std::unique_ptr<Developer> developer;
};

// Тест конструктора и геттеров Task
TEST_F(TaskTest, ConstructorAndGetters) {
    // Проверка начальных значений после создания задачи
    EXPECT_EQ(task->get_title(), "Test Task");        // Заголовок установлен
    EXPECT_FALSE(task->get_id().empty());             // ID не пустой
    EXPECT_EQ(task->get_description(), "");           // Описание пустое по умолчанию
    EXPECT_EQ(task->get_priority(), 0);               // Приоритет 0 по умолчанию
    EXPECT_EQ(task->get_developer(), nullptr);        // Разработчик не назначен
}

// Тест изменения заголовка задачи
TEST_F(TaskTest, SetTitle) {
    task->set_title("New Task Title");
    EXPECT_EQ(task->get_title(), "New Task Title");
}

// Тест установки описания задачи
TEST_F(TaskTest, SetDescription) {
    task->set_description("This is a test description");
    EXPECT_EQ(task->get_description(), "This is a test description");
}

// Тест установки приоритета с валидацией
TEST_F(TaskTest, SetPriority) {
    // Установка валидного приоритета
    task->set_priority(5);
    EXPECT_EQ(task->get_priority(), 5);
    
    // Тест граничных значений (минимальный приоритет)
    task->set_priority(0);
    EXPECT_EQ(task->get_priority(), 0);
    
    // Тест граничных значений (максимальный приоритет)
    task->set_priority(10);
    EXPECT_EQ(task->get_priority(), 10);
    
    // Тест невалидных значений - должны вызывать исключения
    EXPECT_THROW(task->set_priority(-1), std::invalid_argument);
    EXPECT_THROW(task->set_priority(11), std::invalid_argument);
}

// Тест назначения и удаления разработчика у задачи
TEST_F(TaskTest, SetDeveloper) {
    // Проверка начального состояния (разработчик не назначен)
    EXPECT_EQ(task->get_developer(), nullptr);
    
    // Назначение разработчика
    task->set_developer(developer.get());
    EXPECT_EQ(task->get_developer(), developer.get());
    EXPECT_EQ(task->get_developer()->get_name(), "Test Developer");
    
    // Удаление разработчика (назначение nullptr)
    task->set_developer(nullptr);
    EXPECT_EQ(task->get_developer(), nullptr);
}

// Тест ручной установки ID
TEST_F(TaskTest, SetId) {
    std::string new_id = "custom_id_123";
    task->set_id(new_id);
    EXPECT_EQ(task->get_id(), new_id);
    
    // Тест установки пустого ID - должно вызывать исключение
    EXPECT_THROW(task->set_id(""), std::invalid_argument);
}

// Тест генерации уникальных ID
TEST_F(TaskTest, UniqueIdGeneration) {
    // Создание нескольких задач для проверки уникальности ID
    auto task1 = std::make_unique<Task>("Task 1");
    auto task2 = std::make_unique<Task>("Task 2");
    auto task3 = std::make_unique<Task>("Task 3");
    
    // Получение сгенерированных ID
    std::string id1 = task1->get_id();
    std::string id2 = task2->get_id();
    std::string id3 = task3->get_id();
    
    // Проверка, что все ID уникальны
    EXPECT_NE(id1, id2);
    EXPECT_NE(id1, id3);
    EXPECT_NE(id2, id3);
    
    // Проверка, что ID не пустые
    EXPECT_FALSE(id1.empty());
    EXPECT_FALSE(id2.empty());
    EXPECT_FALSE(id3.empty());
    
    // Проверка длины ID (должна быть 6 символов)
    EXPECT_EQ(id1.length(), 6);
    EXPECT_EQ(id2.length(), 6);
    EXPECT_EQ(id3.length(), 6);
}

// Тест оператора сравнения задач
TEST_F(TaskTest, TaskEquality) {
    auto task1 = std::make_unique<Task>("Same Task");
    auto task2 = std::make_unique<Task>("Same Task");
    
    // Задачи с одинаковыми заголовками но разными ID не должны быть равны
    // Оператор == сравнивает title, description и id
    EXPECT_FALSE(*task1 == *task2);
    
    // Создание задачи с таким же ID но другим заголовком
    auto task3 = std::make_unique<Task>("Different Task");
    task3->set_id(task1->get_id());
    task3->set_description(task1->get_description());
    task3->set_priority(task1->get_priority());
    
    // Задачи все еще не равны из-за разных заголовков
    EXPECT_FALSE(*task1 == *task3);
}