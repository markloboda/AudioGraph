cmake_minimum_required(VERSION 2.8.12)

find_package(OpenGL REQUIRED)

project(imgui)

set(inst_src_dir include/imgui_src)

set(root ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(phdrs ${root}/imgui.h)
set(hdrs ${root}/stb_rect_pack.h ${root}/stb_textedit.h ${root}/stb_truetype.h)
set(srcs ${root}/imgui.cpp
   ${root}/imgui_draw.cpp
   ${root}/imgui_tables.cpp
   ${root}/imgui_widgets.cpp
   ${root}/imgui_demo.cpp)
set(inst_srcs ${inst_src_dir}/imgui.cpp)

set(backends
    ${root}/backends/imgui_impl_glfw.cpp
    ${root}/backends/imgui_impl_opengl3.cpp
)

set(backends_headers
    ${root}/backends/imgui_impl_glfw.h
    ${root}/backends/imgui_impl_opengl3.h
)

set(IMGUI_SOURCES ${srcs} ${backends})

add_library(imgui INTERFACE)

if(CMAKE_VERSION VERSION_LESS 3.3)
    message(WARNING "The config-module created by your CMake version (< 3.3) "
        "can't attach the imgui source files to the exported library. Please see the "
        "documentation in `README.md` for further instructions.")
else()
    target_sources(imgui INTERFACE
        $<BUILD_INTERFACE:${srcs}>
        $<BUILD_INTERFACE:${backends}>
        $<INSTALL_INTERFACE:${inst_srcs}>)
endif()

install(FILES ${srcs} ${hdrs} DESTINATION ${inst_src_dir})
install(FILES ${backends} ${backends_headers} DESTINATION ${inst_src_dir}/backends)

target_include_directories(imgui INTERFACE
    $<BUILD_INTERFACE:${root}>
    $<BUILD_INTERFACE:${root}/backends>
    $<INSTALL_INTERFACE:include>)

target_compile_definitions(imgui INTERFACE IMGUI_DISABLE_INCLUDE_IMCONFIG_H)
target_compile_definitions(imgui INTERFACE IMGUI_DISABLE_OBSOLETE_FUNCTIONS)

target_link_libraries(imgui INTERFACE glfw OpenGL::GL)

install(TARGETS imgui EXPORT imgui-targets DESTINATION lib)

install(EXPORT imgui-targets
    DESTINATION lib/cmake/imgui)

install(FILES ${phdrs} DESTINATION include)
install(FILES ${root}/imconfig.h DESTINATION include RENAME imconfig-sample.h)

# create config-module

set(cm ${CMAKE_CURRENT_BINARY_DIR}/hide/imgui-config.cmake)

file(WRITE ${cm} "include(\${CMAKE_CURRENT_LIST_DIR}/imgui-targets.cmake)\n")

if(CMAKE_VERSION VERSION_LESS 3.3)
    file(APPEND ${cm}
        "get_filename_component(IMGUI_SOURCES \"\${CMAKE_CURRENT_LIST_DIR}/../../../include/imgui_src/imgui.cpp\" ABSOLUTE)\n")
endif()

install(FILES ${cm} DESTINATION lib/cmake/imgui)

