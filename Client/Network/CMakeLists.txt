cmake_minimum_required(VERSION 3.26)

project(Network)

add_library(${PROJECT_NAME}
    Websocket.cpp
    WebsocketPayload.cpp
    HttpSession.cpp
    HttpSessionPayload.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

add_dependencies(${PROJECT_NAME}
    nod
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Boost::beast
    PUBLIC nod
)

