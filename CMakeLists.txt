cmake_minimum_required(VERSION 3.20)
project(TextureTool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TextureTool
    src/main.cpp

    src/libs/imgui/imgui.cpp
    src/libs/imgui/imgui_demo.cpp
    src/libs/imgui/imgui_draw.cpp
    src/libs/imgui/imgui_tables.cpp
    src/libs/imgui/imgui_widgets.cpp

    src/libs/imgui/backends/imgui_impl_sdl2.cpp
    src/libs/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(TextureTool PRIVATE
    src/libs/imgui/
    src/libs/imgui/backends/
)

find_package(SDL2 REQUIRED)
target_link_libraries(TextureTool PRIVATE SDL2::SDL2main SDL2::SDL2 opengl32)