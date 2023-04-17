cmake_minimum_required(VERSION 3.26)

project(View)

add_library(${PROJECT_NAME}
    Connection.cpp
    Settings.cpp
    Main.cpp
    TestButton.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC public
)

add_dependencies(${PROJECT_NAME}
    nod
)
 
target_link_libraries(${PROJECT_NAME}
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    PUBLIC  nod
)