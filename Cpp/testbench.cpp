#include "include/WBASS.hpp"
#include "include/BASS.hpp"
#include "include/FFT.hpp"

#include <tuple>
#include <chrono>
#include <fstream>
#include <iostream>

using namespace std;

template<typename T>
inline void sink(const T& value){
    asm volatile("" : : "g"(&value) : "memory");
}

template<template<size_t> class obj_t, size_t N, size_t iterations, size_t index = 1, typename...args_t>
auto benchmark(std::ofstream & stream, args_t&&... args){
    if constexpr(N < index)
        return tuple{};
    else{

        obj_t<index> obj(std::forward<args_t>(args)...);
        static array<float,index> input;
        size_t repetitions = N/index * iterations;
        auto result = obj.execute(input);
        auto start = chrono::high_resolution_clock::now();

        for(size_t i = 0; i < repetitions; i++)
            result = obj.execute(input);

        auto end = chrono::high_resolution_clock::now();
        float elapsed = chrono::duration<float, std::micro>(end - start).count() / (float)repetitions;
        sink(result);

        stream << index << ";" << elapsed << endl;

        return tuple_cat(
            make_tuple(elapsed),
            benchmark<obj_t,N,iterations,2*index>(stream,std::forward<args_t>(args)...)
        );
    }
}

int main(){
    constexpr size_t POWER = 1<<17;
    constexpr size_t ITERATIONS = 100;

    ofstream wbass("../results/wbass.csv");
    ofstream bass("../results/bass.csv");
    ofstream fft("../results/fft.csv");

    benchmark<WBASS,POWER,ITERATIONS,16>(wbass);
    benchmark<BASS,POWER,ITERATIONS,16>(bass,0,21e3,10,1,100);
    benchmark<FFT,POWER,ITERATIONS,16>(fft);

    wbass.close();
    bass.close();
    fft.close();
}