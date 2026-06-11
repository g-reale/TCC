#ifndef WBAS_HPP
#define WBAS_HPP

#include <array>
#include <complex>
#include <iostream>

#include "Filter.hpp"
#include "Constants.hpp"

template<size_t N>
class WBASS {
    private:
        static constexpr size_t FILTER_SIZE = 15;
        size_t decimation;
        Filter<FILTER_SIZE,0,float,std::complex<float>,std::complex<float>> lpf;
        Filter<FILTER_SIZE,0,float,std::complex<float>,std::complex<float>> hpf;
        Filter<FILTER_SIZE,0,std::complex<float>,float,std::complex<float>> analytic;
        inline float energy(const std::array<std::complex<float>,N>& samples,size_t decimation);
        inline void shift(const std::array<std::complex<float>,N>& samples, std::array<std::complex<float>,N>& shifted, size_t decimation);

    public:
        WBASS() : lpf("../filters/lpf.txt"), hpf("../filters/hpf.txt"), analytic("../filters/analytic.txt"){};
        inline float execute(const std::array<float,N>& samples);
};

#include "../templates/WBASS.tpp"
#endif