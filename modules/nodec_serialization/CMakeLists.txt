cmake_minimum_required(VERSION 3.0)

project(nodec_serialization LANGUAGES CXX)

add_subdirectory(cereal)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME}
    INTERFACE
    include cereal
)

target_link_libraries(${PROJECT_NAME}
    INTERFACE
    nodec nodec_physics nodec_rendering nodec_scene nodec_scene_audio
)