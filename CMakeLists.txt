cmake_minimum_required(VERSION 3.15)

project(nodec LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    src/nodec/unicode.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC include
)

# Tests
option(NODEC_BUILD_TESTS "Enable building tests." OFF)

if(NODEC_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
