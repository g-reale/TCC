template<size_t N,size_t M,typename filtertype, typename sampletype, typename resulttype>
resulttype Filter<N,M,filtertype,sampletype,resulttype>::execute(sampletype sample){
    resulttype result = 0;
    
    if constexpr (N){
        firmem.push(sample);
        for(size_t i = 0; i < fir.size(); i++)
            result += fir[i] * firmem[i];
    }
    
    if constexpr (M){
        for(size_t i = 0; i < iir.size(); i++)
            result -= iir[i] * iirmem[i];
        iirmem.push(result);
    }

    return result;
}

template<size_t N,size_t M,typename filtertype, typename sampletype, typename resulttype>
template<size_t K>
void Filter<N,M,filtertype,sampletype,resulttype>::execute(const std::array<sampletype,K>& samples, std::array<resulttype,K>& filtered, size_t decimation){
    for(size_t i = 0; i < K; i+=decimation)
        filtered[i] = execute(samples[i]);
}

template<size_t N,size_t M,typename filtertype, typename sampletype, typename resulttype>
Filter<N,M,filtertype,sampletype,resulttype>::Filter(const std::string& path){
    std::fstream file(path,std::fstream::in);
    if(!file)
        throw std::runtime_error("cannot open file");

    std::string firline;
    std::string iirline;
    getline(file,firline);
    getline(file,iirline);
    
    size_t i = 0;
    std::stringstream stream;
    stream = std::stringstream(firline);
    while(i < N && stream >> fir[i++]);

    i = 0;
    stream = std::stringstream(iirline);
    while(i < M && stream >> iir[i++]);
    file.close();
}

template<size_t N,size_t M,typename filtertype, typename sampletype, typename resulttype>
void Filter<N,M,filtertype,sampletype,resulttype>::clear(){
    firmem.clear();
    iirmem.clear();
}