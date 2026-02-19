#include "../include/WBAS.hpp"

template<size_t N>
float WBAS<N>::energy(const std::array<float,N>& samples, size_t step){
    float energy = 0;
    for(size_t i = 0; i < samples.size(); i+= step)
        energy += samples[i] * samples[i];
    return energy;
}

template<size_t N>
void WBAS<N>::filter(const std::array<float,N>& samples, std::array<float,N>& filtered, size_t step){
    for(size_t i = 0; i < samples.size(); i += step){
        float result = firststage.execute(samples[i]);
        filtered[i] = secondstage.execute(result);
    }
    firststage.clear();
    secondstage.clear();
}

template<size_t N>
void WBAS<N>::upperband(const std::array<float,N>& wholeband, const std::array<float,N>& lowerband, std::array<float,N>& upperband, size_t step){
    float shift = 1;
    for(size_t i = 0; i < wholeband.size(); i += step, shift = -shift)
        upperband[i] = shift * (wholeband[i] - lowerband[i]);
}

template<size_t N>
float WBAS<N>::execute(const std::array<float,N>& samples){
    
    float low = 0;
    float up = SAMPLE_RATE/2.0f;
    std::array<std::complex<float>,N> auxiliars[2];
    std::array<std::complex<float>,N> * whole = &auxiliars[0];
    std::array<std::complex<float>,N> * lower = &auxiliars[1];
    
    //need to filter the negative frequencies here
    std::copy(samples.begin(),samples.end(),(*whole).begin());
    
    float wb_energy = energy(samples,1);
    for(size_t i = 1; i < samples.size(); i <<= 1){
        
        filter(*whole,*lower,i);
        float lb_energy = energy(*lower,i);
        float ub_energy = std::max(0.0f,wb_energy - lb_energy);
        
        if(lb_energy >= ub_energy){//lower band wins
            wb_energy = lb_energy/2;//estimate
            std::swap(lower,whole);
            up -= (up - low)/2.0f;
        }
        else{//upper band wins
            wb_energy = ub_energy/2;//estimate
            upperband(*whole, *lower, *whole, i);
            low += (up - low)/2.0f;
        }
    }

    return (low + up)/2.0f;
}