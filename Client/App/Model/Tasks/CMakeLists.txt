cmake_minimum_required(VERSION 3.26)

project(Model.Tasks)

add_library(${PROJECT_NAME}
    Blender/RenderTask.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC public
)

add_dependencies(${PROJECT_NAME}
    Model.Files
    Model.Server
    Model.Blender
    System
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC Model.Files
    PUBLIC Model.Server
    PUBLIC Model.Blender
    PUBLIC System
)