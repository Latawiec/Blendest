cmake_minimum_required(VERSION 3.26)

project(UI)

add_library(${PROJECT_NAME}
    Connection.cpp
    Settings.cpp
    Main.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)