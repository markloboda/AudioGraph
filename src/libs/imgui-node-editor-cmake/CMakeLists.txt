project(imgui_node_editor)

set(root ${CMAKE_CURRENT_SOURCE_DIR}/imgui-node-editor)

set(_imgui_node_editor_sources
    ${root}/crude_json.cpp
    ${root}/crude_json.h
    ${root}/imgui_bezier_math.h
    ${root}/imgui_bezier_math.inl
    ${root}/imgui_canvas.cpp
    ${root}/imgui_canvas.h
    ${root}/imgui_extra_math.h
    ${root}/imgui_extra_math.inl
    ${root}/imgui_node_editor.cpp
    ${root}/imgui_node_editor.h
    ${root}/imgui_node_editor_api.cpp
    ${root}/imgui_node_editor_internal.h
    ${root}/imgui_node_editor_internal.inl
)

add_library(${PROJECT_NAME} STATIC ${_imgui_node_editor_sources})

target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)

target_include_directories(imgui_node_editor PUBLIC ${root})
target_link_libraries(imgui_node_editor PRIVATE imgui)