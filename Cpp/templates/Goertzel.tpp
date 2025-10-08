inline void Goertzel::update(float sample){
    s_0 = sample + iir_1 * s_1 - s_2;
    s_2 = s_1;
    s_1 = s_0;
}

inline float Goertzel::execute(){
    float re = s_1 - fir_1 * s_2;
    float im = fir_2 * s_2;
    return re * re + im * im;
}

template<size_t N>
float Goertzel::execute(const std::array<float,N>& samples){
    clear();
    for(float sample : samples)
        update(sample);
    return execute();
}
