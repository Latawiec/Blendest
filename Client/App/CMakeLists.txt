cmake_minimum_required(VERSION 3.26)
project(App)

add_library(${PROJECT_NAME}
    App.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

add_dependencies(${PROJECT_NAME}
    UI
    Network
    System
    Model
    Serializer
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE UI
    PRIVATE Network
    PRIVATE System
    PRIVATE Model
    PRIVATE Serializer
)
