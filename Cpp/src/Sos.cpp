#include "../include/Sos.hpp"

using namespace std;

template<size_t order, typename filtertype, typename sampletype, typename resulttype>
Sos<order,filtertype,sampletype,resulttype>::Sos(const initializer_list<segment_t>& coefficients){    
    
    assert(coefficients.size() == order);
    size_t i = 0;
    for(const auto& segment : coefficients){
        const auto& [fir,iir] = segment;
        biquads[i] = Filter<3,2,filtertype,sampletype,resulttype>(fir,iir);
        i++;
    }
}

template<size_t order, typename filtertype, typename sampletype, typename resulttype>
void Sos<order,filtertype,sampletype,resulttype>::clear(){
    for(auto& section : biquads)
        section.clear();
}
