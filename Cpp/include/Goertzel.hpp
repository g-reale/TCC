#ifndef GOERTZEL_HPP
#define GOERTZEL_HPP

#include <iostream>
#include <array>
#include <cmath>

#include "Constants.hpp"

class Goertzel{
    private:
        static constexpr float a = 0.6;
        static constexpr float a_exp_2 = a * a;

        float iir_1;
        float fir_1;
        float fir_2;

        float s_0;
        float s_1;
        float s_2;

    public:
        Goertzel(float frequency);
        void clear();
        void set(float frequency);
        inline void update(float sample);
        inline float execute();
        template<size_t N>
        float execute(const std::array<float,N>& samples);
};

#include "../templates/Goertzel.tpp"
#endif