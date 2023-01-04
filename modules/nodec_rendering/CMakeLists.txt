cmake_minimum_required(VERSION 3.0)

project(nodec_rendering LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME}
    INTERFACE include
)

target_link_libraries(${PROJECT_NAME}
    INTERFACE nodec nodec_scene
)

# https://stackoverflow.com/questions/71299716/how-to-compile-hlsl-shaders-during-build-with-cmake
add_custom_target(nodec_rendering_shaders)

if(MSVC)
    function(add_vertex_shader LIST FILE)
        set(${LIST} ${${LIST}} ${FILE} PARENT_SCOPE)
        set_source_files_properties(${FILE}
            PROPERTIES
            SHADER_TYPE "vs"
            SHADER_MODEL "4_0"
            ENTRY_POINT_NAME "VSMain"
        )
    endfunction()

    function(add_pixel_shader LIST FILE)
        set(${LIST} ${${LIST}} ${FILE} PARENT_SCOPE)
        set_source_files_properties(${FILE}
            PROPERTIES
            SHADER_TYPE "ps"
            SHADER_MODEL "4_0"
            ENTRY_POINT_NAME "PSMain"
        )
    endfunction()

    set(HLSL_SHADER_FILES "")

    add_vertex_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/image/vertex.hlsl")
    add_vertex_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/pbr/vertex.hlsl")
    add_vertex_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/pbr-defer/geometry_vs.hlsl")
    add_vertex_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/pbr-defer/lighting_vs.hlsl")
    add_vertex_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/bloom/blend_vs.hlsl")
    add_vertex_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/bloom/blur_h_vs.hlsl")
    add_vertex_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/bloom/blur_v_vs.hlsl")
    add_vertex_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/bloom/brightness_vs.hlsl")
    add_vertex_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/hdr/hdr_vs.hlsl")
    add_vertex_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/ssao/occlusion_vs.hlsl")

    add_pixel_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/image/pixel.hlsl")
    add_pixel_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/pbr/pixel.hlsl")
    add_pixel_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/pbr-defer/geometry_ps.hlsl")
    add_pixel_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/pbr-defer/lighting_ps.hlsl")
    add_pixel_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/bloom/blend_ps.hlsl")
    add_pixel_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/bloom/blur_h_ps.hlsl")
    add_pixel_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/bloom/blur_v_ps.hlsl")
    add_pixel_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/bloom/brightness_ps.hlsl")
    add_pixel_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/hdr/hdr_ps.hlsl")
    add_pixel_shader(HLSL_SHADER_FILES
        "resources/org.nodec-rendering.essentials/shaders/post-processings/ssao/occlusion_ps.hlsl")

    # set(HLSL_SHADER_FILES
    # resources/org.nodec-rendering.essentials/shaders/image/pixel.hlsl
    # resources/org.nodec-rendering.essentials/shaders/image/vertex.hlsl
    # )

    # set_source_files_properties(
    # resources/org.nodec-rendering.essentials/shaders/image/vertex.hlsl
    # PROPERTIES ShaderType "vs")

    # set_source_files_properties(
    # resources/org.nodec-rendering.essentials/shaders/image/pixel.hlsl
    # PROPERTIES ShaderType "ps")

    # set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES ShaderModel "4_0")
    foreach(FILE ${HLSL_SHADER_FILES})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        get_filename_component(DIRECTORY ${FILE} DIRECTORY)
        get_source_file_property(SHADER_TYPE ${FILE} SHADER_TYPE)
        get_source_file_property(SHADER_MODEL ${FILE} SHADER_MODEL)
        get_source_file_property(ENTRY_POINT_NAME ${FILE} ENTRY_POINT_NAME)
        add_custom_command(TARGET nodec_rendering_shaders
            COMMAND fxc.exe /nologo /E${ENTRY_POINT_NAME} /T${SHADER_TYPE}_${SHADER_MODEL} $<IF:$<CONFIG:DEBUG>,/Od,/O1> /Zi /Fo ${DIRECTORY}/${FILE_WE}.cso /Fd ${DIRECTORY}/${FILE_WE}.pdb ${FILE}
            MAIN_DEPENDENCY ${FILE}
            COMMENT "HLSL ${FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM)
    endforeach(FILE)
endif()

add_dependencies(${PROJECT_NAME} nodec_rendering_shaders)

# Tests
option(NODEC_RENDERING_BUILD_TESTS "Enable building tests." OFF)

if(NODEC_RENDERING_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
