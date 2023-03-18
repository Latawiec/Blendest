cmake_minimum_required(VERSION 3.26)

project(Connection)

add_library(${PROJECT_NAME}
    BackgroundWebsocket.cpp
    HandlerPayload.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Boost::beast
)

