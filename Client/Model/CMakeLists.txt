cmake_minimum_required(VERSION 3.26)

project(Model)

add_library(${PROJECT_NAME}
    BlenderTask.cpp
    Connection.cpp
    Main.cpp
    Settings.cpp
    TaskRunner.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

add_dependencies(${PROJECT_NAME}
    UI
    Network
    System
    Assets
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC UI
    PUBLIC Network
    PUBLIC System
    PRIVATE Assets
)

add_subdirectory(Serializer)
add_subdirectory(Assets)