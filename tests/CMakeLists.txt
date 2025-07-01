project(nodec_tests)

function(add_basic_test TARGET TEST_SOURCES)
    add_executable(${TARGET} ${TEST_SOURCES})
    target_link_libraries(${TARGET} nodec)
    target_include_directories(${TARGET} PRIVATE common)

    add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction(add_basic_test)

add_basic_test("nodec__array_view" array_view/array_view.cpp)
add_basic_test("nodec__containers__sparse_table" containers/sparse_table.cpp)
add_basic_test("nodec__entitites__registry" entities/registry.cpp)
add_basic_test("nodec__entitites__storage" entities/storage.cpp)
add_basic_test("nodec__asyncio__event_loop" asyncio/event_loop.cpp)
add_basic_test("nodec__asyncio__event_promise" asyncio/event_promise.cpp)
add_basic_test("nodec__flags" flags/flags.cpp)
add_basic_test("nodec__formatter" formatter/formatter.cpp)
add_basic_test("nodec__formatter_benchmark" formatter/formatter_benchmark.cpp)
add_basic_test("nodec__formatter_detailed_benchmark" formatter/formatter_detailed_benchmark.cpp)
add_basic_test("nodec__formatter_vector3_benchmark" formatter/formatter_vector3_benchmark.cpp)
add_basic_test("nodec__formatter_ostringstream_compat" formatter/formatter_ostringstream_compat.cpp)
add_basic_test("nodec__ostringstream_verification" formatter/ostringstream_verification.cpp)
add_basic_test("nodec__hex_debug" formatter/hex_debug.cpp)
add_basic_test("nodec__complex_format_debug" formatter/complex_format_debug.cpp)
add_basic_test("nodec__optimized_ostringstream_test" formatter/optimized_ostringstream_test.cpp)
add_basic_test("nodec__gfx" gfx/gfx.cpp)
add_basic_test("nodec__logging__logging" logging/logging.cpp)
add_basic_test("nodec__logging_bench" logging/bench.cpp)
add_basic_test("nodec__math" math/math.cpp)
add_basic_test("nodec__matrix4x4" matrix4x4/matrix4x4.cpp)
add_basic_test("nodec__observers" observers/observers.cpp)
add_basic_test("nodec__optional__basic" optional/basic.cpp)
add_basic_test("nodec__quaternion" quaternion/quaternion.cpp)
add_basic_test("nodec__random" random/random.cpp)
add_basic_test("nodec__ranges" ranges/ranges.cpp)
add_basic_test("nodec__resource_management__resource_registry" resource_management/resource_registry.cpp)
add_basic_test("nodec__signals__signal" signals/signal.cpp)
add_basic_test("nodec__stopwatch" stopwatch/stopwatch.cpp)
add_basic_test("nodec__type_info" type_info/type_info.cpp)
add_basic_test("nodec__unicode" unicode/unicode.cpp)
add_basic_test("nodec__utility" utility/utility.cpp)
add_basic_test("nodec__vector" vector/vector.cpp)

# Debug test for optimized ostringstream
add_basic_test("nodec__debug_test" formatter/debug_test.cpp)

# Constructor debug test  
add_basic_test("nodec__constructor_debug" formatter/constructor_debug.cpp)

# OpenMode examples test  
add_basic_test("nodec__openmode_examples" formatter/openmode_examples.cpp)

# Vector3 debug test  
add_basic_test("nodec__vector3_debug" formatter/vector3_debug.cpp)

target_compile_definitions(
    nodec__unicode PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/unicode/data"
)