cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)


project(BlendestClient
    LANGUAGES CXX
    VERSION 1.0.0
)

add_executable(${PROJECT_NAME}
    main.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
)

add_dependencies(${PROJECT_NAME}
    App
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE App
)

add_subdirectory(Connection)
add_subdirectory(System)
add_subdirectory(UI)
add_subdirectory(App)