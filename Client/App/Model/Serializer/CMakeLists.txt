cmake_minimum_required(VERSION 3.26)

project(Model.Serializer)


add_dependencies(${PROJECT_NAME}
    View
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE nlohmann_json
    PRIVATE Boost::dll
    PUBLIC  View
)