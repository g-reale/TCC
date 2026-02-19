#ifndef BAS_HPP
#define BAS_HPP

#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>

#include "Goertzel.hpp"
#include "Constants.hpp"

class BAS{
    private:
        float alpha;
        float beta;
        float power;
        float trust;
        size_t iterations;
        size_t slices;
        Goertzel analizer;
    public:
        BAS(float alpha, float beta, size_t iterations, float power, float trust);
        template<size_t N>
        float nthArea(const std::array<float,N> & samples, float alpha, float beta);
        template<size_t N>
        float execute(const std::array<float,N> & samples);
        void set(float alpha, float beta, size_t iterations, float power, float thrust);
};

#include "../templates/BAS.tpp"
#endif