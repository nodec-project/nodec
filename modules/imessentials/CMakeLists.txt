cmake_minimum_required(VERSION 3.0)

project(imessentials LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    src/menu_impl.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC include
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC nodec imgui
)