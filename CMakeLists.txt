cmake_minimum_required(VERSION 2.8)
project(Renderer_Dev)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

add_definitions(-w)
# SET(CMAKE_BUILD_TYPE "Debug")  # 定义编译类型
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb") # 定义Debug编译参数
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") # 定义Release编译参数

include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src
                                    ${PROJECT_SOURCE_DIR}/thirdparty)
link_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME}
    src/glad.c 
    src/main.cpp 
    src/scene.cpp
    src/model.cpp
    src/shader.cpp
    src/texture.cpp
    src/material.cpp
    src/attributes.cpp
    src/file_system.cpp
    src/renderer_ui.cpp
    src/postprocess.cpp
    src/scene_object.cpp
    src/render_texture.cpp
    src/editor_content.cpp
    src/render_pipeline.cpp
    src/editor_settings.cpp
    src/renderer_window.cpp 
    src/input_management.cpp
    thirdparty/imgui/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_widgets.cpp
                                )

target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/glfw3.lib)
#target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/imgui.lib)
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/OpenGL32.lib)
target_link_libraries(${PROJECT_NAME} 
    debug ${PROJECT_SOURCE_DIR}/lib/assimp-vc143-mtd.lib 
    optimized ${PROJECT_SOURCE_DIR}/lib/assimp-vc143-mt.lib)

