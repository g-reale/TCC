cmake_minimum_required(VERSION 3.10)
project(GoertzelGUI)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find Packages ---
find_package(OpenGL REQUIRED)

# Usar pkg-config para encontrar glfw3 corretamente no Debian
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

# --- Executável principal ---
add_executable(goertzel_gui
    main.cpp

    # ImGui
    ${CMAKE_SOURCE_DIR}/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/imgui/imgui_widgets.cpp

    # ImGui Backends
    ${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp

    # ImPlot
    ${CMAKE_SOURCE_DIR}/implot/implot.cpp
    ${CMAKE_SOURCE_DIR}/implot/implot_items.cpp

    # Você pode incluir o demo se quiser:
    # ${CMAKE_SOURCE_DIR}/implot/implot_demo.cpp
)

# --- Include Paths ---
target_include_directories(goertzel_gui PRIVATE
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}

    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui/backends
    ${CMAKE_SOURCE_DIR}/implot
)

# --- Linkagem ---
target_link_libraries(goertzel_gui
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${CMAKE_DL_LIBS}  # Necessário para GLAD ou OpenGL dinâmico
)
