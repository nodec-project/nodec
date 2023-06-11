project(nodec_scene_tests)

set(NODEC_DIR ../../../nodec)

add_subdirectory(${NODEC_DIR} nodec)

function(add_basic_test TARGET TEST_SOURCES)
    add_executable(${TARGET} ${TEST_SOURCES})
    target_link_libraries(${TARGET} nodec_scene)
    target_include_directories(${TARGET}
        PRIVATE ${NODEC_DIR}/tests/common
    )

    add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction(add_basic_test)

add_basic_test("scene" scene.cpp)
add_basic_test("transform_system" transform_system.cpp)