project(nodec_tests)

function(add_basic_test TARGET TEST_SOURCES)
    add_executable(${TARGET} ${TEST_SOURCES})
    target_link_libraries(${TARGET} nodec)
    target_include_directories(${TARGET} PRIVATE common)
    # set_target_properties(${TARGET} PROPERTIES
    #     CXX_STANDARD_REQUIRED ON
    #     CXX_STANDARD 14)

    add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction(add_basic_test)

add_basic_test("optional__basic" optional/basic.cpp)
add_basic_test("entitites__registry" entities/registry.cpp)
add_basic_test("entitites__storage" entities/storage.cpp)
add_basic_test("math" math/math.cpp)
add_basic_test("math_gfx" math/gfx.cpp)
add_basic_test("type_info" type_info/type_info.cpp)
add_basic_test("formatter" formatter/formatter.cpp)
add_basic_test("utility" utility/utility.cpp)
add_basic_test("matrix4x4" matrix4x4/matrix4x4.cpp)
add_basic_test("signals__signal" signals/signal.cpp)
add_basic_test("containers__sparse_table" containers/sparse_table.cpp)
add_basic_test("random" random/random.cpp)