#ifndef WBAS_HPP
#define WBAS_HPP

#include <array>
#include <cmath>
#include <complex>
#include <cstdint>
#include <iostream>

#include "Filter.hpp"
#include "Constants.hpp"

// octave:56> [b, a] = ellip(4, 0.5, 20, 0.5);
// octave:57> sos = tf2sos(b, a)
// sos = 0.241295   0.320591   0.241295   1.000000   0.045016   0.867640
//       1.000000   0.318595   1.000000   1.000000  -0.229655   0.260995

template<size_t N>
class WBAS {
    private:
        static float energy(const std::array<float,N>& samples, size_t step);
        void filter(const std::array<float,N>& samples, std::array<float,N>& filtered, size_t step);
        static void upperband(const std::array<float,N>& wholeband, const std::array<float,N>& lowerband, std::array<float,N>& upperband, size_t step);
        Filter<3,2,float,float,float> firststage;
        Filter<3,2,float,float,float> secondstage;
    public:
        WBAS() : firststage({0.241295, 0.320591, 0.241295},{0.045016, 0.867640}), secondstage({1.000000, 0.318595, 1.000000},{-0.229655, 0.260995}) {}; //ugly and i know it
        float execute(const std::array<float,N> & samples);
};

#include "../templates/WBAS.tpp"
#endif