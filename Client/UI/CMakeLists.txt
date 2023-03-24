cmake_minimum_required(VERSION 3.26)

project(UI
    LANGUAGES CXX
    VERSION 1.0.0
)

add_library(${PROJECT_NAME}
    Connection.cpp
    Settings.cpp
    Main.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)