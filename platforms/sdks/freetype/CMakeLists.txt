cmake_minimum_required(VERSION 3.10)

project(freetype)

if(WIN32)
    # add_library(${PROJECT_NAME} "freetype-windows-binaries/release static/vs2015-2022/win64/freetype.lib")
    add_library(${PROJECT_NAME} INTERFACE)
    target_link_libraries(${PROJECT_NAME}
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/freetype-windows-binaries/release\ static/vs2015-2022/win64/freetype.lib
    )
    target_include_directories(${PROJECT_NAME}
        INTERFACE freetype-windows-binaries/include
    )
endif()
