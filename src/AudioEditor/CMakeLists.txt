# Add your executable or library
add_library(AudioEditor STATIC "AudioEditor.cpp" "AudioEditor.h" "ImguiWindow.cpp" "ImguiWindow.h" "AudioEditorSettings.h" "NodeRenderer.h"  "NodeRenderer.cpp" "GraphWindow.h" "GraphWindow.cpp" "EditorNodes/SignalGraphNode.h" "EditorNodes/SignalGraphNode.cpp" "EditorNodes/EditorNodeFactory.h" "EditorNodes/EditorNode.h" "EditorNodes/FileNode.cpp" "EditorNodes/FileNode.h")

target_link_libraries(AudioEditor PRIVATE glad)
target_link_libraries(AudioEditor PRIVATE glfw)
target_link_libraries(AudioEditor PRIVATE imgui)
target_link_libraries(AudioEditor PRIVATE imgui_node_editor)
target_link_libraries(AudioEditor PRIVATE implot)
target_link_libraries(AudioEditor PRIVATE AudioEngine) 
target_link_libraries(AudioEditor PRIVATE Common)
target_link_libraries(AudioEditor PRIVATE ImGuiFileDialog)


target_include_directories(AudioEditor PRIVATE ${CMAKE_SOURCE_DIR}/src/AudioEngine)
target_include_directories(AudioEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
