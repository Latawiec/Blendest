cmake_minimum_required(VERSION 3.26)

# Nlohmann
include(FetchContent)
FetchContent_Declare(nlohmann
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(nlohmann)
# --------

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