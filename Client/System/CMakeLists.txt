cmake_minimum_required(VERSION 3.26)

project(System)

add_library(${PROJECT_NAME}
# if windows etc... someday
    ./Windows/Process.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

