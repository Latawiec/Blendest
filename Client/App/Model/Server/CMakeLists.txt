cmake_minimum_required(VERSION 3.26)

project(Model.Server)

add_library(${PROJECT_NAME}
    Connection.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC public
)

add_dependencies(${PROJECT_NAME}
    Network
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC Network
)
