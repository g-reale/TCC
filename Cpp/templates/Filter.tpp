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