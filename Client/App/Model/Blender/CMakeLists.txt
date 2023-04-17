cmake_minimum_required(VERSION 3.26)

project(Model.Blender)

add_library(${PROJECT_NAME}
    Handle.cpp
    Settings.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC public
)

target_link_libraries(${PROJECT_NAME}
    System
)

target_add_exe_local_asset(${PROJECT_NAME}
    assets/BlenderFileInfo.py
    BlenderFileInfo.py
)