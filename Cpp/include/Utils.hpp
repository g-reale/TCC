#ifndef UTILS_HPP
#define UTILS_HPP

#include <array>
#include <chrono>

template<size_t N, typename T, typename ...types>
inline void throttle(T function, types... arguments){
    using clock = std::chrono::high_resolution_clock;
    using duration_t = std::chrono::duration<float>;

    static duration_t period(1/(float)N);
    static auto start = clock::now();

    if(clock::now() - start >= period){
        function(arguments...);
        start = clock::now();
    }
}

#endif