#ifndef GOERTZEL_HPP
#define GOERTZEL_HPP

#include <iostream>
#include <array>
#include <cmath>

#include "Constants.hpp"

class Goertzel{
    private:
        static constexpr double a = 0.98;
        static constexpr double a_exp_2 = a * a;

        double iir_1;
        double fir_1;
        double fir_2;

        double s_0;
        double s_1;
        double s_2;

    public:
        Goertzel(float frequency);
        void clear();
        void set(float frequency);
        inline void update(float sample);
        inline double execute();
        template<size_t N>
        double execute(const std::array<float,N>& samples);
};

#include "../templates/Goertzel.tpp"
#endif