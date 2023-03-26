cmake_minimum_required(VERSION 3.26)

project(System)

add_library(${PROJECT_NAME}
    Process.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Boost::process
    PRIVATE Boost::asio
)

