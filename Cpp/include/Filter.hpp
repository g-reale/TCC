#ifndef FILTER_HPP
#define FILTER_HPP

#include <array>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <algorithm>
#include <initializer_list>

#include "Circular.hpp"

template<size_t N, size_t M, typename filtertype, typename sampletype, typename resulttype>
class Filter{
    private:
        std::array<filtertype,N> fir;
        std::array<filtertype,M> iir;
        Circular<N,sampletype> firmem;
        Circular<M,resulttype> iirmem;
    public:
        Filter() = default;
        Filter(std::array<filtertype,N> fir, std::array<filtertype,M> iir) : fir(fir), iir(iir) {};
        Filter(const std::string& path);
        void clear();
        inline resulttype execute(sampletype sample);
        template<size_t K>
        inline void execute(const std::array<sampletype,K>& samples, std::array<resulttype,K>& filtered, size_t decimation = 1);
};

#include "../templates/Filter.tpp"
#endif