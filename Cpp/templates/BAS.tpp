template<size_t N>
float BAS::nthArea(const std::array<float,N> & samples, float alpha, float beta){
    
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
float BAS::execute(const std::array<float,N>& samples){
    
    std::array<float,N> y;
    for(float i = 1; i <= samples.size(); i++)
        y[i-1] = - samples[i-1] / i;                           //don´t forget the j implications
        
    float upper = beta;
    float lower = alpha;
    float a_0 = nthArea(y,lower,upper);
    float a_1 = 0;

    for(size_t i = 0; i < iterations; i++){
        a_1 = nthArea(y,lower,lower + (upper - lower)/2);
    
        // std::cout << i << ": whole(" << lower << "," << upper << "): " << a_0 << " left(" << lower << "," << upper/2 << "): " << a_1 << " rigth(" << upper/2 << "," << upper <<"): " << a_0 - a_1 << std::endl;

        if(a_0 - a_1 > a_1){
            // std::cout << "\trigth wins" << std::endl;
            lower += (upper - lower)/2;
            a_0 = a_0 - a_1;
        }
        else{
            // std::cout << "\tleft wins" << std::endl;
            upper -= (upper - lower)/2;
            a_0 = a_1;
        }

        if(upper < lower)
            break;
    }

    // std::cout << "end(" << lower << "," << upper << ")" << std::endl;
    return (lower + upper)/2;
}