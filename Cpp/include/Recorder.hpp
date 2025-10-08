#ifndef RECORDER_HPP
#define RECORDER_hPP

#include <array>
#include <mutex>
#include <thread>
#include <string>
#include <complex>
#include <algorithm>
#include <pulse/simple.h>
#include <condition_variable>

#include "Constants.hpp"

template<size_t N>
class Recorder{
    private:
        pa_simple * pulse_audio_handle = nullptr;
        std::array<float,N> frame;
        size_t index;
    public:
        ~Recorder();
        float record();
        Recorder(const std::string & source);
        void record(std::array<float,N> & frame);
        void clear();
        void reset(const std::string & source);
};

#include "../templates/Recorder.tpp"
#endif