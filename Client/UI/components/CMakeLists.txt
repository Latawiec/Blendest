cmake_minimum_required(VERSION 3.26)

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v4.0.0
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
# ------------------------------------------------------------------------------

project(UI.Components
    LANGUAGES CXX
    VERSION 1.0.0
)

add_library(${PROJECT_NAME}
    Settings.cpp
    Connection.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)