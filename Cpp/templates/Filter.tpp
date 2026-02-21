template<size_t N,size_t M,typename filtertype, typename sampletype, typename resulttype>
resulttype Filter<N,M,filtertype,sampletype,resulttype>::execute(sampletype sample){
    resulttype result = 0;
    firmem.push(sample);

    for(size_t i = 0; i < fir.size(); i++)
        result += fir[i] * firmem[i];

    for(size_t i = 0; i < iir.size(); i++)
        result -= iir[i] * iirmem[i];

    iirmem.push(result);
    return result;
}

template<size_t N,size_t M,typename filtertype, typename sampletype, typename resulttype>
void Filter<N,M,filtertype,sampletype,resulttype>::clear(){
    firmem.clear();
    iirmem.clear();
}

template<size_t order, typename filtertype, typename sampletype, typename resulttype>
Sos<order,filtertype,sampletype,resulttype>::Sos(std::array<std::pair<std::array<filtertype,3>,std::array<filtertype,2>>,order> coefficients){    
    for(size_t i = 0; i < order; i++){
        auto& [fir,iir] = coefficients[i];
        biquads[i] = Filter<3,2,filtertype,sampletype,resulttype>(fir,iir);
    }
}

template<size_t order, typename filtertype, typename sampletype, typename resulttype>
resulttype Sos<order,filtertype,sampletype,resulttype>::execute(sampletype sample){
    for(auto& section : biquads){
        resulttype result = section.execute(sample);
        sample = result;
    }

    return sample;
}

template<size_t order, typename filtertype, typename sampletype, typename resulttype>
void Sos<order,filtertype,sampletype,resulttype>::clear(){
    for(auto& section : biquads)
        section.clear();
}
