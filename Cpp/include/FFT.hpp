#ifndef FFT_HPP
#define FFT_HPP

#include <array>
#include <complex>
#include <liquid/liquid.h>
#include "Constants.hpp"

template<size_t N>
class FFT{
    private:
        fftplan fft;
        std::array<std::complex<float>,N> x;
        std::array<std::complex<float>,N> y;
    public:
        FFT();
        ~FFT();
        inline std::pair<float,float> execute(const std::array<float,N>& samples);
};

#include "../templates/FFT.tpp"
#endif