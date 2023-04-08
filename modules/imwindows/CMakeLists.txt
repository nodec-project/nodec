cmake_minimum_required(VERSION 3.0)

project(imwindows LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE include
)

# target_link_libraries(${PROJECT_NAME}
#     INTERFACE 
#     nodec nodec_scene nodec_rendering
#     imgui
# )