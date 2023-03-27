cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(dependencies.cmake)
include (functions.cmake)

project(BlendestClient
    LANGUAGES CXX
    VERSION 1.0.0
)

add_executable(${PROJECT_NAME}
    main.cpp
)

add_dependencies(${PROJECT_NAME}
    ViewModel
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE ViewModel
)

add_subdirectory(Modules)
add_subdirectory(App)