
#include "include/Recorder.hpp"
#include "include/Utils.hpp"
#include <algorithm>
#include <iostream>
#include <array>

using namespace std;

int main(){
    
    constexpr size_t BUFFERING = 512;
    constexpr size_t SAMPLE_RATE = 48000;
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

    Recorder<BUFFERING> rec;
    size_t quantized = 0;

    while(true){

        throttle<SAMPLE_RATE/BUFFERING>(
            [&rec, &quantized, &RESOLUTION](){
                float mean = 0;
                for(size_t i = 0; i < BUFFERING; i++)
                    mean += rec.record();
                mean /= (float)BUFFERING;
                quantized = clamp((size_t)abs(mean * RESOLUTION),(size_t)0,RESOLUTION-1);
            }
        );
        
        throttle<60>(
            [&quantized,&levels](){
                cout << levels[quantized].data() << endl;
            }
        );
    }

    return 0;
}