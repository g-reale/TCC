#include "Goertzel.hpp"

#include <cmath>
#include <stdexcept> 

using namespace std;

Goertzel::Goertzel(float frequency){
    try{
        set(frequency);
    }
    catch(const std::invalid_argument& e){
        throw invalid_argument("Goertzel.constructor:\n" + string(e.what()));
    }
    clear();
}

void Goertzel::clear(){
    s_0 = 0;
    s_1 = 0;
    s_2 = 0;
}

void Goertzel::set(float frequency){
    if(frequency > SAMPLE_RATE/2)
        throw(invalid_argument("Goertzel.set: frequency must smaller than half of the sample rate: " + to_string(SAMPLE_RATE) + " sps"));

    float radians = 2.0f * M_PI * frequency / (float)(SAMPLE_RATE);
    iir_1 = 2.0f * cos(radians);
    fir_1 = cos(radians);
    fir_2 = sin(radians);
}
