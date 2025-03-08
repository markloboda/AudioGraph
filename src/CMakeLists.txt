# CMakeList.txt : CMake project for AudioGraph, include source and define
# project specific logic here.
#

add_subdirectory(libs)
add_subdirectory(AudioEngine)
add_subdirectory(AudioEditor)
add_subdirectory(Common)

# Add source to this project's executable.
add_executable (AudioGraph "main.cpp" "Common/Utils.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AudioGraph PROPERTY CXX_STANDARD 20)
endif()

# Link AudioEngine to AudioGraph
target_link_libraries(AudioGraph PRIVATE AudioEngine)
target_link_libraries(AudioGraph PRIVATE AudioEditor)
target_link_libraries(AudioGraph PRIVATE Common)