#include "include/BackEnd.hpp"

#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "implot/implot.h"
#include "imgui/imgui.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <array>

using namespace std;

int main(){
    
    BackEnd::initialize();
    for(auto source : BackEnd::querySources())
        cout << source << endl;
    
    BackEnd::createAnalyzer(10e3);
    BackEnd::createAnalyzer(11e3);

    // while (true){
    //     cout << BackEnd::queryFrequency(10e3) << " " << BackEnd::queryFrequency(11e3) << endl;
    // }
    
    BackEnd::cleanup();
    return 0;
}
