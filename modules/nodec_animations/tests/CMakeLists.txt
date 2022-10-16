project(nodec_animations_tests)

set(NODEC_DIR ../../../nodec)

add_subdirectory(${NODEC_DIR} nodec)

function(add_basic_test TARGET TEST_SOURCES)
    add_executable(${TARGET} ${TEST_SOURCES})
    target_link_libraries(${TARGET} nodec_animations)
    target_include_directories(${TARGET}
        PRIVATE ${NODEC_DIR}/tests/common
    )

    add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction(add_basic_test)

add_basic_test("animation_curve" animation_curve.cpp)