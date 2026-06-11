template<size_t order, typename filtertype, typename sampletype, typename resulttype>
resulttype Sos<order,filtertype,sampletype,resulttype>::execute(sampletype sample){
    for(auto& section : biquads){
        resulttype result = section.execute(sample);
        sample = result;
    }
    return sample;
}