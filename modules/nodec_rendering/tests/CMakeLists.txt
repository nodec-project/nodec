project(nodec_rendering_tests)

set(NODEC_DIR ../../../nodec)
set(NODEC_SCENE_DIR ../../nodec_scene)

add_subdirectory(${NODEC_DIR} nodec)
add_subdirectory(${NODEC_SCENE_DIR} nodec_scene)

function(add_basic_test TARGET TEST_SOURCES)
    add_executable(${TARGET} ${TEST_SOURCES})
    target_link_libraries(${TARGET} nodec_rendering)
    target_include_directories(${TARGET}
        PRIVATE ${NODEC_DIR}/tests/common
    )

    add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction(add_basic_test)

add_basic_test("visivility_system" visivility_system.cpp)
