template<size_t N>
float BASS<N>::nthArea(const std::array<float,N> & samples, float alpha, float beta){
    
    float area = 0;
    float lower = alpha;
    float a_0 = analizer.execute(alpha,samples);
    float a_1 = 0;

    do{
        lower += trust;
        a_1 = analizer.execute(std::min(lower,beta), samples);
        area += std::pow(a_1 - a_0, 2 * power);
        a_0 = a_1;
    }while(lower < beta);
    return area;
}

template<size_t N>
float BASS<N>::execute(const std::array<float,N>& samples){
    
    std::array<float,N> y;
    for(float i = 1; i <= samples.size(); i++)
        y[i-1] = - samples[i-1] / i;
        
    float upper = beta;
    float lower = alpha;
    float a_0 = nthArea(y,lower,upper);
    float a_1 = 0;

    for(size_t i = 0; i < iterations; i++){
        a_1 = nthArea(y,lower,lower + (upper - lower)/2);
        
        if(a_0 - a_1 > a_1){
            lower += (upper - lower)/2;
            a_0 = a_0 - a_1;
        }
        else{
            upper -= (upper - lower)/2;
            a_0 = a_1;
        }

        if(upper < lower)
            break;
    }

    return (lower + upper)/2;
}

template<size_t N>
BASS<N>::BASS(float alpha, float beta, size_t iterations, float power, float trust) : analizer(0){
    set(alpha,beta,iterations,power,trust);
}

template<size_t N>
void BASS<N>::set(float alpha, float beta, size_t iterations, float power, float trust){
    this->alpha = alpha;
    this->beta = beta;
    this->iterations = iterations;
    this->power = power;
    this->trust = trust;
}