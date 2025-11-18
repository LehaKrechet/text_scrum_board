cmake_minimum_required (VERSION 3.11)
 
# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)
 
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

# Загружаем googletest первым
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG main
)

FetchContent_Declare(rapidjson
  GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
  GIT_TAG master
)

# Загружаем все зависимости
FetchContent_MakeAvailable(googletest ftxui rapidjson)

# ------------------------------------------------------------------------------
 
project(text_scrum_board
  LANGUAGES CXX
  VERSION 1.0.0
)

enable_testing()

# Создаем исполняемые файлы
add_executable(text_scrum_board 
    src/main.cpp 
    src/task.cpp 
    src/column.cpp 
    src/board.cpp 
    src/developer.cpp 
    src/json_worker.cpp 
    src/manager.cpp 
    src/ftxui.cpp
)

add_executable(scrum_board_tests
    test/test_main.cpp
    test/test_board.cpp
    test/test_developer.cpp
    test/test_column.cpp
    test/test_task.cpp
    test/test_manager.cpp
    src/board.cpp
    src/column.cpp
    src/task.cpp
    src/developer.cpp
    src/manager.cpp
)

# Настраиваем include директории
target_include_directories(text_scrum_board PRIVATE include)
target_include_directories(scrum_board_tests PRIVATE include)

# Настраиваем зависимости для rapidjson
target_include_directories(text_scrum_board PRIVATE 
    ${rapidjson_SOURCE_DIR}/include
)
target_include_directories(scrum_board_tests PRIVATE 
    ${rapidjson_SOURCE_DIR}/include
)

# Настраиваем линковку для основного приложения
target_link_libraries(text_scrum_board
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)

# Настраиваем линковку для тестов
target_link_libraries(scrum_board_tests
  PRIVATE GTest::gtest_main
  PRIVATE gmock
)

# Добавляем тесты
add_test(NAME ScrumBoardTests COMMAND scrum_board_tests)