cmake_minimum_required(VERSION 3.26)

project(Assets)

add_library(${PROJECT_NAME}
    Assets.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE ASSETS_PATH="${CMAKE_CURRENT_LIST_DIR}"
)