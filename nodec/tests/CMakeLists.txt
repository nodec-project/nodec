project(nodec_tests)

function(add_basic_test TARGET TEST_SOURCES)
    add_executable(${TARGET} ${TEST_SOURCES})
    target_link_libraries(${TARGET} nodec)
    target_include_directories(${TARGET} PRIVATE common)

    add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction(add_basic_test)

add_basic_test("optional_basic" optional/basic.cpp)
add_basic_test("entitites_registry" entities/registry.cpp)
