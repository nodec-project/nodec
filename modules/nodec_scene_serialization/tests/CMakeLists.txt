set(NODEC_DIR ../../../nodec)
set(MODULE_DIR ../..)

add_subdirectory(${NODEC_DIR} nodec)
add_subdirectory(${MODULE_DIR}/nodec_serialization nodec_serialization)
add_subdirectory(${MODULE_DIR}/nodec_scene nodec_scene)

function(add_basic_test TARGET TEST_SOURCES)
    add_executable(${TARGET} ${TEST_SOURCES})
    target_link_libraries(${TARGET} nodec_scene_serialization)
    target_include_directories(${TARGET}
        PRIVATE ${NODEC_DIR}/tests/common
    )

    add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction(add_basic_test)

add_basic_test("scene_serialization" scene_serialization.cpp)
add_basic_test("entity_serializer" entity_serializer.cpp)
add_basic_test("entity_emplacer" entity_emplacer.cpp)
add_basic_test("entity_loader" entity_loader.cpp)