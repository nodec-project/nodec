cmake_minimum_required(VERSION 3.0)

project(nodec_animation LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE include
)

target_link_libraries(${PROJECT_NAME}
    INTERFACE nodec nodec_scene nodec_scene_serialization
)

# Tests
option(NODEC_ANIMATION_BUILD_TESTS "Enable building tests." OFF)

if(NODEC_ANIMATION_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
