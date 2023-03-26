cmake_minimum_required(VERSION 3.26)

project(Model.Main)

add_library(${PROJECT_NAME}
    Main.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC public
)

add_dependencies(${PROJECT_NAME}
    Model.Server
    Model.Blender
)
 
target_link_libraries(${PROJECT_NAME}
    PUBLIC Model.Server
    PUBLIC Model.Blender
)