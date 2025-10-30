cmake_minimum_required (VERSION 3.11)
 
# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)
 
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG main
)
FetchContent_Declare(rapidjson
  GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
  GIT_TAG master
)
 
FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
 
# Загружаем RapidJSON
FetchContent_GetProperties(rapidjson)
if(NOT rapidjson_POPULATED)
  FetchContent_Populate(rapidjson)
endif()
# ------------------------------------------------------------------------------
 
project(text_scrum_board
  LANGUAGES CXX
  VERSION 1.0.0
)
 
add_executable(text_scrum_board src/main.cpp src/task.cpp src/column.cpp src/board.cpp src/developer.cpp src/json_worker.cpp src/manager.cpp src/ftxui.cpp)
target_include_directories(text_scrum_board PRIVATE include)
 
target_link_libraries(text_scrum_board
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
)
# RapidJSON
target_include_directories(text_scrum_board PRIVATE 
    ${rapidjson_SOURCE_DIR}/include
)