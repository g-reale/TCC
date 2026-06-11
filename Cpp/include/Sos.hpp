#ifndef SOS_HPP
#define SOS_HPP

#include <array>
#include <complex>
#include <assert.h>
#include <initializer_list>

#include "Filter.hpp"

template<size_t order, typename filtertype, typename sampletype, typename resulttype>
class Sos{
    private:
        std::array<Filter<3,2,filtertype,sampletype,resulttype>,order> biquads;
    public:
        using segment_t = std::pair<std::array<float,3>,std::array<float,2>>;
        Sos() = default;
        Sos(const std::initializer_list<segment_t>& coefficients);
        inline resulttype execute(sampletype sample);
        void clear();
};

#include "../templates/Sos.tpp"
#endif