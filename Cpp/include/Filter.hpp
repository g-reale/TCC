#ifndef FILTER_HPP
#define FILTER_HPP

#include <array>
#include <vector>
#include <algorithm>

template<size_t N, typename datatype>
class Circular{
    private:
        std::array<datatype,N> memory;
        size_t index = 0;
    public:

        Circular() = default;

        void push(datatype sample){
            memory[index] = sample;
            index = (index + 1) % memory.size();
        }

        inline float operator[](size_t i) const {
            return memory[(memory.size() + index - i - 1) % memory.size()];
        }

        void clear(){
            std::fill(memory.begin(),memory.end(),0);
        }
};

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
        void clear();
        resulttype execute(sampletype sample);
};

template<size_t order, typename filtertype, typename sampletype, typename resulttype>
class Sos{
    private:
        std::array<Filter<3,2,filtertype,sampletype,resulttype>,order> biquads;
    public:
        Sos(std::array<std::pair<std::array<filtertype,3>,std::array<filtertype,2>>,order> coefficients);
        void clear();
        resulttype execute(sampletype sample);
};

#include "../templates/Filter.tpp"
#endif