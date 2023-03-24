cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(dependencies.cmake)

project(BlendestClient
    LANGUAGES CXX
    VERSION 1.0.0
)

add_executable(${PROJECT_NAME}
    main.cpp
)

add_dependencies(${PROJECT_NAME}
    App
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE App
)

add_subdirectory(Network)
add_subdirectory(System)
add_subdirectory(UI)
add_subdirectory(Model)
add_subdirectory(App)