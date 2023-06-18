project(nodec_animations_tests)

set(NODEC_DIR ../../../nodec)
set(MODULE_DIR ../..)

add_subdirectory(${NODEC_DIR} nodec)
add_subdirectory(${MODULE_DIR}/nodec_scene nodec_scene)
add_subdirectory(${MODULE_DIR}/nodec_serialization nodec_serialization)
add_subdirectory(${MODULE_DIR}/nodec_scene_serialization nodec_scene_serialization)

function(add_basic_test TARGET TEST_SOURCES)
    add_executable(${TARGET} ${TEST_SOURCES})
    target_link_libraries(${TARGET} nodec_animations)
    target_include_directories(${TARGET}
        PRIVATE ${NODEC_DIR}/tests/common
    )

    add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction(add_basic_test)

add_basic_test("animation_curve" animation_curve.cpp)
add_basic_test("animation_clip" animation_clip.cpp)