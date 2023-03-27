cmake_minimum_required(VERSION 3.26)

project (Model.Files)

add_library(${PROJECT_NAME}
    FileManager.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC public
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Boost::dll
)