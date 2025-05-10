
#include "include/Recorder.hpp"
#include "include/Utils.hpp"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <array>

using namespace std;

int main(){
    
    // constexpr size_t BUFFERING = 512;
    // constexpr size_t SAMPLE_RATE = 48000;
    // constexpr size_t RESOLUTION = 100;
    
    // constexpr array<array<char,RESOLUTION + 1>,RESOLUTION> levels = [](){
    //     array<array<char,RESOLUTION + 1>,RESOLUTION> levels;
    //     for(size_t i = 0; i < RESOLUTION; i++){
    //         for(size_t j = 0; j < i; j++)
    //             levels[i][j] = '=';
    //         levels[i][i] = '\0';
    //     }
    //     return levels;
    // }();

    // Recorder<BUFFERING> rec;
    // size_t quantized = 0;

    // while(true){

    //     throttle<SAMPLE_RATE/BUFFERING>(
    //         [&rec, &quantized, &RESOLUTION](){
    //             float mean = 0;
    //             for(size_t i = 0; i < BUFFERING; i++)
    //                 mean += rec.record();
    //             mean /= (float)BUFFERING;
    //             quantized = clamp((size_t)abs(mean * RESOLUTION),(size_t)0,RESOLUTION-1);
    //         }
    //     );
        
    //     throttle<60>(
    //         [&quantized,&levels](){
    //             cout << levels[quantized].data() << endl;
    //         }
    //     );
    // }

    // Initialize GLFW
    if (!glfwInit())
        return -1;

    // Setup OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui Example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader (GLAD in this case)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core"); // Set shader version

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}