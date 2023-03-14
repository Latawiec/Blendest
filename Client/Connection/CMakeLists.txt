cmake_minimum_required(VERSION 3.26)

# --- Fetch Boost --------------------------------------------------------------
include(FetchContent)
set(BOOST_INCLUDE_LIBRARIES thread system beast program_options)
set (BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.81.0
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(boost)
if(NOT boost_POPULATED)
FetchContent_Populate(boost)
add_subdirectory(
    ${boost_SOURCE_DIR}
    ${boost_BINARY_DIR}
    EXCLUDE_FROM_ALL
)
endif()
# ------------------------------------------------------------------------------

project(Connection)

add_library(${PROJECT_NAME}
    ServerConnection.cpp
)

target_include_directories(${PROJECT_NAME}
    INTERFACE public
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC Boost::beast
)

