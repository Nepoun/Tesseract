cmake_minimum_required(VERSION 3.20)
project(TextureTool)

set(CMAKE_CXX_STANDARD 20)
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

    src/libs/ImNodes/imnodes_internal.h
    src/libs/ImNodes/imnodes.cpp
    src/libs/ImNodes/imnodes.h

    src/nodes/NodeBase.h
    src/nodes/PresetNode.h
    src/nodes/ConversorNode.h
    src/nodes/PathInputNode.h
    src/nodes/StringNode.h
    src/nodes/ViewDataNode.h
    
    src/registry/NodeRegistry.h
    src/registry/RuntimeNodeRegistry.h

    src/pins/NodePin.h
    src/pins/Link.h

    src/types/Preset.h
    src/util/IdUtil.h
    src/util/SaveLoad.h
)

#target_compile_definitions(TextureTool PRIVATE _GLIBCXX_HAVE_AT_QUICK_EXIT=0)
target_include_directories(TextureTool PRIVATE
    src/
    src/nodes/
    src/pins/
    src/libs/imgui/
    src/libs/imgui/backends/
    src/libs/ImNodes/
    src/include/
)

add_custom_command(TARGET TextureTool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/ucrt64/bin/libstdc++-6.dll"
        "C:/msys64/ucrt64/bin/libgcc_s_seh-1.dll"
        "C:/msys64/ucrt64/bin/libwinpthread-1.dll"
        $<TARGET_FILE_DIR:TextureTool>
)

file(COPY ${CMAKE_SOURCE_DIR}/tools/slk
     DESTINATION ${CMAKE_BINARY_DIR})

find_package(SDL2 REQUIRED)
target_link_options(TextureTool PRIVATE -static-libgcc -static-libstdc++)
target_link_libraries(TextureTool PRIVATE SDL2::SDL2main SDL2::SDL2 opengl32)
message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}")