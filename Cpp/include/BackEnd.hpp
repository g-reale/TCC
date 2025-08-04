#ifndef BACKEND_CPP
#define BACKEND_CPP

#include "Constants.hpp"
#include "Goertzel.hpp"
#include "Recorder.hpp"
#include "Utils.hpp"

#include <mutex>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <condition_variable>
#include <pulse/pulseaudio.h>

class BackEnd{
    private:
        static std::atomic<bool> finished;
        static std::atomic<bool> stopped;
        static std::mutex critical;
        static std::mutex processing;
        static std::condition_variable condition;
        static std::thread worker;
        static Recorder<BUFFER_SIZE> recorder;
        static std::unordered_map<float,float> frequencies;
        static std::unordered_map<float,Goertzel> analyzers;
        
        static std::atomic<bool> read_names;
        static pa_mainloop * main_loop;
        static pa_context * context;
        static std::vector<std::string> sources;
        static void contextCallBack(pa_context * context, void * userdata);
        static void sourceInfoCallBack(pa_context * context, const pa_source_info * info, int eol, void * userdata);
    public:
        
        static void setSource(const std::string& source);
        static std::vector<std::string> querySources();

        static float queryFrequency(float frequency);
        static void createAnalyzer(float frequency);
        static void destroyAnalyzer(float frequency);
        
        static void initialize();
        static void cleanup();
        static void start();
        static void stop();
};
#endif