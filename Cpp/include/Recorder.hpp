#ifndef RECORDER_HPP
#define RECORDER_hPP

#include <array>
#include <mutex>
#include <thread>
#include <complex>
#include <algorithm>
#include <pulse/simple.h>
#include <condition_variable>

#include "Constants.hpp"

template<size_t N>
class Recorder{
    private:
        pa_simple * pulse_audio_handle;
        std::array<std::array<float,N>,2> frames;
        
        std::thread worker;
        std::condition_variable cv;
        std::mutex mtx;
        bool finished;
        bool ready;
        
        size_t index;
        bool current_frame;
    public:
        ~Recorder();
        Recorder();
        float record();
        void record(std::array<float,N> & frame);
};

#include "../templates/Recorder.tpp"
#endif