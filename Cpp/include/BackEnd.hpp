#ifndef BACKEND_CPP
#define BACKEND_CPP

#include "Constants.hpp"
#include "Goertzel.hpp"
#include "Recorder.hpp"
#include "Utils.hpp"
#include "WBAS.hpp"
#include "BAS.hpp"

#include <mutex>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <utility>
#include <unordered_map>
#include <condition_variable>
#include <pulse/pulseaudio.h>

class BackEnd{
    private:
        static float normalization;
        static constexpr float decay = 0.99;
        static Recorder<BUFFER_SIZE> recorder;
        static std::array<float,BUFFER_SIZE> frame;
        static std::unordered_map<float,Goertzel> analyzers;
        static BAS bas;
        static WBAS<BUFFER_SIZE> wbas;
        
        static std::atomic<bool> read_names;
        static pa_mainloop * main_loop;
        static pa_context * context;
        static std::vector<std::string> sources;
        static void contextCallBack(pa_context * context, void * userdata);
        static void sourceInfoCallBack(pa_context * context, const pa_source_info * info, int eol, void * userdata);
    public:
        
        static void setSource(const std::string& source);
        static std::vector<std::string> querySources();

        static std::pair<float,float> maximum();
        static float queryFrequency(float frequency);
        static void update();
        static void createAnalyzer(float frequency);
        static void destroyAnalyzer(float frequency);
        
        static void initialize();
        static void cleanup();
};
#endif