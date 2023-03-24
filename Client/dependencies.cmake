
include(FetchContent)

set(FETCHCONTENT_QUIET FALSE)

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

# Boost
set(BOOST_ENABLE_CMAKE ON)
set(BOOST_INCLUDE_LIBRARIES beast process dll asio thread chrono)

FetchContent_Declare(boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.81.0
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(boost)
# ---------

# Nlohmann
FetchContent_Declare(nlohmann
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(nlohmann)
# --------

# Nod (non-CMake)
project(nod)
FetchContent_Declare(nod
    GIT_REPOSITORY https://github.com/fr00b0/nod.git
    GIT_TAG v0.5.4
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(nod)

add_library(nod INTERFACE)
target_include_directories(nod INTERFACE ${nod_SOURCE_DIR}/include/nod)
# /Zc:__cplusplus is required to make __cplusplus accurate
# /Zc:__cplusplus is available starting with Visual Studio 2017 version 15.7
# (according to https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus)
# That version is equivalent to _MSC_VER==1914
# (according to https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019)
# CMake's ${MSVC_VERSION} is equivalent to _MSC_VER
# (according to https://cmake.org/cmake/help/latest/variable/MSVC_VERSION.html#variable:MSVC_VERSION)

## nod relies on __cplusplus macro...
if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
	target_compile_options(nod INTERFACE "/Zc:__cplusplus")
endif()

# --------