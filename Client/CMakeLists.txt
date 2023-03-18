cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)


set(BOOST_VERSION_TAG boost-1.81.0)
set(FETCHCONTENT_QUIET FALSE)
set(BOOST_ENABLE_CMAKE ON)

function(BoostMakeAvailable libraryName)
    include(FetchContent)
    FetchContent_Declare(boost_${libraryName}
        GIT_REPOSITORY https://github.com/boostorg/${libraryName}.git
        GIT_TAG ${BOOST_VERSION_TAG}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(boost_${libraryName})
endfunction()

# get all of boost
BoostMakeAvailable(boost)

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
    App
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE App
)

add_subdirectory(Network)
add_subdirectory(System)
add_subdirectory(UI)
add_subdirectory(Model)
add_subdirectory(App)