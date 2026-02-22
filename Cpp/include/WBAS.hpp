#ifndef WBAS_HPP
#define WBAS_HPP

#include <array>
#include <cmath>
#include <complex>
#include <cstdint>
#include <iostream>

#include "Filter.hpp"
#include "Constants.hpp"

// [b,a] = ellip(4, 0.5, 20, 0.5);
// sos   = tf2sos(b,a)
// sos =

//    0.241295   0.320591   0.241295   1.000000   0.045016   0.867640
//    1.000000   0.318595   1.000000   1.000000  -0.229655   0.260995

template<size_t N>
class WBAS {
    private:
        static float energy(const std::array<float,N>& samples, size_t step);
        void filter(const std::array<float,N>& samples, std::array<float,N>& filtered, size_t step);
        void upperband(const std::array<float,N>& wholeband, const std::array<float,N>& lowerband, std::array<float,N>& upperband, size_t step);
        Sos<2,float,float,float> lpf;
    public:
        // WBAS() : lpf({
        //     {{0.120960, -0.019523, 0.120960},{-1.152464, 0.462928}},
        //     {{1.000000, -1.234695, 1.000000},{-1.322280, 0.902983}}
        // }){};
        
        //create initializer list constructor later
        WBAS() : lpf(
            std::array<
                std::pair<
                    std::array<float,3>,
                    std::array<float,2>
                >,
            2>{{
                {
                    {{0.241295f, 0.320591f, 0.241295f}},
                    {{0.045016f, 0.867640f}}
                },
                {
                    {{1.000000f, 0.318595f, 1.000000f}},
                    {{-0.229655f, 0.260995f}}
                }
            }}
        ){};
        float execute(const std::array<float,N> & samples);
};

#include "../templates/WBAS.tpp"
#endif