cmake_minimum_required(VERSION 3.0)

project(imguizmo LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    ImGuizmo/GraphEditor.cpp
    ImGuizmo/ImCurveEdit.cpp
    ImGuizmo/ImGradient.cpp
    ImGuizmo/ImGuizmo.cpp
    ImGuizmo/ImSequencer.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ImGuizmo
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC imgui
)