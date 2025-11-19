#include <gtest/gtest.h>

// Основная функция для запуска всех тестов Google Test
int main(int argc, char **argv) {
    // Инициализация Google Test framework с аргументами командной строки
    ::testing::InitGoogleTest(&argc, argv);
    
    // Запуск всех зарегистрированных тестов и возврат результата
    return RUN_ALL_TESTS();
}