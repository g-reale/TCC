cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

project(BAS)

find_path(PULSEAUDIO_INCLUDE_DIR
        NAMES pulse/pulseaudio.h
        DOC "The PulseAudio include directory"
)
find_library(PULSEAUDIO_LIBRARY
        NAMES pulse
        DOC "The PulseAudio library"
)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pulse-simple pulse)
target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -fsanitize=address)
target_include_directories(${PROJECT_NAME} PRIVATE include templates ${PULSEAUDIO_INCLUDE_DIRS})
