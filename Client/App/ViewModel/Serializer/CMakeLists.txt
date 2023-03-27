cmake_minimum_required(VERSION 3.26)

project(ViewModel.Serializer)


add_library(${PROJECT_NAME}
    Serializer.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC public
)

add_dependencies(${PROJECT_NAME}
    View
    Model.Files
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE nlohmann_json
    PUBLIC Model.Files
    PUBLIC View
)