template<size_t N>
float WBASS<N>::energy(const std::array<std::complex<float>,N>& samples, size_t decimation){
    float energy = 0;
    for(size_t i = 0; i < samples.size(); i+= decimation)
        energy += std::norm(samples[i]);
    return energy;
}

template<size_t N>
void WBASS<N>::shift(const std::array<std::complex<float>,N>& samples, std::array<std::complex<float>,N>& shifted, size_t decimation){
    std::complex<float> frequency = std::polar<float>(1.0f,2*M_PI/4.0f);
    std::complex<float> phase = 1.0f;

    for(size_t i = 0; i < N; i+=decimation, phase *= frequency)
        shifted[i] = samples[i] * phase;
}

template<size_t N>
float WBASS<N>::execute(const std::array<float,N>& samples){
    
    std::array<std::complex<float>,N> memory[3];
    std::array<std::complex<float>,N> * whole = &memory[0];
    std::array<std::complex<float>,N> * lower = &memory[1];
    std::array<std::complex<float>,N> * upper = &memory[2];
    
    analytic.clear();
    analytic.execute(samples,*whole);
    
    size_t decimation = 1;
    float low = 0;
    float up = SAMPLE_RATE/2.0f;
    while(decimation < N){
        lpf.execute(*whole,*lower,decimation);
        hpf.execute(*whole,*upper,decimation);
        float lb_energy = energy(*lower,decimation);
        float ub_energy = energy(*upper,decimation);
        
        if(lb_energy >= ub_energy){
            std::swap(whole,lower);
            up = (up+low)/2;
        }
        else{
            shift(*upper,*whole,decimation);
            low = (up+low)/2;
        }
        
        decimation <<= 1;
        hpf.clear();
        lpf.clear();
    }

    
    return (low + up)/2.0f;
}