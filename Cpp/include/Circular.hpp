#ifndef CIRCULAR_HPP
#define CIRCULAR_HPP

#include <array>
#include <complex>

template<size_t N, typename datatype>
class Circular{
    private:
        std::array<datatype,N> memory;
        size_t index = 0;
    public:
        Circular() = default;
        inline void push(datatype sample);
        inline datatype operator[](size_t i) const;
        void clear();
};

#include "../templates/Circular.tpp"
#endif