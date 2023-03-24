cmake_minimum_required(VERSION 3.26)

project(Serializer)

add_library(${PROJECT_NAME}
    Serializer.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

add_dependencies(${PROJECT_NAME}
    Model
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC Model
    PRIVATE nlohmann_json
    PRIVATE Boost::dll
)