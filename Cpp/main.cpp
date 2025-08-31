#include "include/FrontEnd.hpp"
#include "include/BackEnd.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "implot/implot.h"
#include "imgui/imgui.h"

#include <iostream>

// Callback para tratamento de erros do GLFW
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(){
    // Configurar callback de erro do GLFW
    glfwSetErrorCallback(glfw_error_callback);

    // Inicializar GLFW e criar janela
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    // GLSL version
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Goertzel Analyzer", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Inicializar o backend
    BackEnd::initialize();

    // Inicializar o frontend
    FrontEnd::Application::initialize(window);

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Renderizar o frontend
        FrontEnd::Application::render(window);

        glfwSwapBuffers(window);
    }

    // Limpeza do frontend
    FrontEnd::Application::cleanup();

    // Limpeza do backend
    BackEnd::cleanup();

    // Terminar GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


