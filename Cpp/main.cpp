#include "include/Constants.hpp"
#include "include/Recorder.hpp"
#include "include/Goertzel.hpp"
#include "include/Utils.hpp"

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
    
    constexpr size_t BUFFERING = 512;
    constexpr size_t RESOLUTION = 100;
    
    constexpr array<array<char,RESOLUTION + 1>,RESOLUTION> levels = [](){
        array<array<char,RESOLUTION + 1>,RESOLUTION> levels;
        for(size_t i = 0; i < RESOLUTION; i++){
            for(size_t j = 0; j < i; j++)
                levels[i][j] = '=';
            levels[i][i] = '\0';
        }
        return levels;
    }();

    Goertzel analyzer(10e3);
    Recorder<BUFFERING> rec;
    array<float,BUFFERING> samples;
    double mag = 0;
    double max_mag = 0;
    size_t quantized = 0;

    while(true){

        throttle<SAMPLE_RATE/BUFFERING>(
            [&rec, &quantized, &RESOLUTION, &samples, &analyzer, &mag, &max_mag](){
                rec.record(samples);
                mag = analyzer.execute(samples);
                max_mag = mag > max_mag ? mag : max_mag;
                analyzer.clear();
                quantized = clamp((size_t)abs(mag/max_mag * RESOLUTION),(size_t)0,RESOLUTION-1);
            }
        );
        
        throttle<120>(
            [&quantized,&levels, &mag, &max_mag](){
                cout << levels[quantized].data() << " " << mag/max_mag << endl;
            }
        );
    }

    return 0;
}