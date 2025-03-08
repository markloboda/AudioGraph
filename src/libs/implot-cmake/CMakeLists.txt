project(implot)

set(root ${CMAKE_CURRENT_SOURCE_DIR}/implot)

set(implot_sources
    ${root}/implot.cpp
    ${root}/implot.h
    ${root}/implot_internal.h
    ${root}/implot_items.cpp
    ${root}/implot_demo.cpp
)

add_library(${PROJECT_NAME} STATIC ${implot_sources})

target_include_directories(implot PUBLIC ${root})
target_link_libraries(implot PRIVATE imgui)