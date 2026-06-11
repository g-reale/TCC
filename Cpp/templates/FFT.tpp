template<size_t N>
std::pair<float,float> FFT<N>::execute(const std::array<float,N>& samples){
    
    for(size_t i = 0; i < N; i++)
        x[i] = samples[i];
    fft_execute(fft);
    
    float magnitude = 0;
    size_t index = 0;

    for(size_t i = 0; i < N/2; i++){
        if(magnitude < abs(y[i])){
            magnitude = abs(y[i]);
            index = i;
        }
    }

    float maximum = (index/(float)(N/2)) * (SAMPLE_RATE/2.0f);
    return std::pair<float,float>(maximum,magnitude);
}

template<size_t N>
FFT<N>::FFT(){
    fft = fft_create_plan(N,x.data(),y.data(),LIQUID_FFT_FORWARD,0);
}

template<size_t N>
FFT<N>::~FFT(){
    fft_destroy_plan(fft);
}