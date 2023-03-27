cmake_minimum_required(VERSION 3.26)

project(ViewModel)

add_library(${PROJECT_NAME}
    App.cpp
    Connection.cpp
    Settings.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC public
)

add_dependencies(${PROJECT_NAME}
    nod
    ViewModel.Serializer
    Model.Main
    View
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE ftxui::screen # TODO: Move this to View somehow.
    PRIVATE ViewModel.Serializer
    PUBLIC Model.Main
    PUBLIC View
    PUBLIC nod
)

add_subdirectory(Serializer)