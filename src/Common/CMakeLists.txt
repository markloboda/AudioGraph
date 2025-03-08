# Add your executable or library
add_library(Common STATIC "Utils.cpp" "Utils.h")

target_include_directories(Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})