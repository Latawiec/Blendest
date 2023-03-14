cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v4.0.0
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
# ------------------------------------------------------------------------------

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
    UI.Components
    # UI.Views
    Connection
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    PRIVATE UI.Components
    # PRIVATE UI.Views
    PRIVATE Connection
)

add_subdirectory(UI)
add_subdirectory(Connection)