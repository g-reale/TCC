cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

project(BAS)

option(PERFORMANCE OFF)

find_path(PULSEAUDIO_INCLUDE_DIR
        NAMES pulse/pulseaudio.h
        DOC "The PulseAudio include directory"
)
find_library(PULSEAUDIO_LIBRARY
        NAMES pulse
        DOC "The PulseAudio library"
)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

set(IIMGUI
        imgui
        imgui/backends
        imgui/misc
)

set(SRCIMGUI
        imgui/imgui.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_tables.cpp
        imgui/misc/cpp/imgui_stdlib.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)

set(IIMPLOT
        implot/
)

set(SRCIMPLOT
        implot/implot_demo.cpp
        implot/implot_items.cpp
        implot/implot.cpp
)

set(
        SRCGLAD
        glad/src/glad.c
)

add_executable(${PROJECT_NAME} main.cpp src/Goertzel.cpp src/BackEnd.cpp src/FrontEnd.cpp ${SRCIMGUI} ${SRCGLAD} ${SRCIMPLOT})
target_link_libraries(${PROJECT_NAME} PRIVATE pulse-simple pulse ${GLFW_LIBRARIES} OpenGL::GL)
target_include_directories(${PROJECT_NAME} PRIVATE include templates ${PULSEAUDIO_INCLUDE_DIRS} ${IIMGUI} ${GLFW_INCLUDE_DIRS} ${IIMPLOT})

if(PERFORMANCE)
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O3 -march=native)
else()
        target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -fsanitize=address)
endif()