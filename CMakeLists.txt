cmake_minimum_required(VERSION 4.0)
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

file(GLOB SRC_FILES 
    src/*.c
    src/*.cpp
    src/runtime/function/*.cpp
)

file(GLOB IMGUI_FILES
    thirdparty/imgui/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_widgets.cpp
)

add_executable(${PROJECT_NAME} 
    ${SRC_FILES}
    ${IMGUI_FILES}
)

target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/glfw3.lib)
#target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/imgui.lib)
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/OpenGL32.lib)
target_link_libraries(${PROJECT_NAME} 
    debug ${PROJECT_SOURCE_DIR}/lib/assimp-vc143-mtd.lib 
    optimized ${PROJECT_SOURCE_DIR}/lib/assimp-vc143-mt.lib)

# ========== 自动复制 DLL 到输出目录 ==========
if(WIN32)
    # 定义 DLL 源目录（假设 bin/Debug/ 和 bin/Release/ 存放不同版本的 DLL）
    set(DLL_SRC_DIR_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug)
    set(DLL_SRC_DIR_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release)

    # 根据当前构建类型选择 DLL 源目录
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(DLL_SRC_DIR ${DLL_SRC_DIR_RELEASE})
    else()
        set(DLL_SRC_DIR ${DLL_SRC_DIR_DEBUG})
    endif()

    # 使用 POST_BUILD 自动复制 DLL
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${DLL_SRC_DIR}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying DLLs from ${DLL_SRC_DIR} to output directory..."
    )
endif()