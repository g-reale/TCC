inline void Goertzel::update(float sample){
    s_0 = a_exp_2 * sample + iir_1 * s_1 + a_exp_2 * s_2;
    s_2 = s_1;
    s_1 = s_0;
}

inline double Goertzel::execute(){
    double re = s_0 + fir_1 * s_1;
    double im = fir_2 * s_1;
    return re * re + im * im;
}

template<size_t N>
double Goertzel::execute(const std::array<float,N>& samples){
    for(double sample : samples)
        update(sample);
    return execute();
}
