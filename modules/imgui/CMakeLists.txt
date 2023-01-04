cmake_minimum_required(VERSION 3.0)

project(imgui LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC imgui
)
