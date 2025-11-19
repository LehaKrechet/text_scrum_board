#include <gtest/gtest.h>
#include <memory>
#include "developer.h"

// Test fixture класс для тестирования Developer
// Fixture предоставляет общую среду для группы тестов
class DeveloperTest : public ::testing::Test {
protected:
    // Метод SetUp вызывается перед каждым тестом
    void SetUp() override {
        // Создание тестового разработчика для использования в тестах
        developer = std::make_unique<Developer>("Test Developer");
    }

    // Общие ресурсы для тестов
    std::unique_ptr<Developer> developer;
};

// Тест конструктора и методов получения данных
TEST_F(DeveloperTest, ConstructorAndGetters) {
    // Проверка, что конструктор правильно устанавливает имя
    EXPECT_EQ(developer->get_name(), "Test Developer");
}

// Тест метода установки имени
TEST_F(DeveloperTest, SetName) {
    // Изменение имени разработчика
    developer->set_name("New Developer Name");
    
    // Проверка, что имя было успешно изменено
    EXPECT_EQ(developer->get_name(), "New Developer Name");
}

// Тест обработки невалидных данных - пустое имя
TEST_F(DeveloperTest, SetEmptyName) {
    // Проверка, что установка пустого имени вызывает исключение
    EXPECT_THROW(developer->set_name(""), std::invalid_argument);
}

// Тест создания нескольких разработчиков
TEST_F(DeveloperTest, MultipleDevelopers) {
    // Создание нескольких разработчиков с разными именами
    auto dev1 = std::make_unique<Developer>("Developer One");
    auto dev2 = std::make_unique<Developer>("Developer Two");
    auto dev3 = std::make_unique<Developer>("Developer Three");
    
    // Проверка, что каждый разработчик имеет правильное имя
    EXPECT_EQ(dev1->get_name(), "Developer One");
    EXPECT_EQ(dev2->get_name(), "Developer Two");
    EXPECT_EQ(dev3->get_name(), "Developer Three");
}