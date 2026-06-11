#include "../include/BackEnd.hpp"

using namespace std;

Recorder<BUFFER_SIZE> BackEnd::recorder("");
array<float,BUFFER_SIZE> BackEnd::frame;
unordered_map<float,Goertzel> BackEnd::analyzers;
float BackEnd::normalization;
BASS<BUFFER_SIZE> BackEnd::bass(0,5e3,10,1,10);
WBASS<BUFFER_SIZE> BackEnd::wbass;
FFT<BUFFER_SIZE> BackEnd::fft;

atomic<bool> BackEnd::read_names = false;
pa_mainloop *BackEnd::main_loop = nullptr;
pa_context *BackEnd::context = nullptr;
vector<string> BackEnd::sources;


void BackEnd::sourceInfoCallBack(pa_context * context, const pa_source_info *info, int eol, void *userdata) {
    if (eol){
        read_names = true;
        return;
    }
    sources.push_back(info->name);
}

void BackEnd::contextCallBack(pa_context *context, void * userdata) {
    pa_operation * operation = nullptr;
    switch (pa_context_get_state(context)) {
        case PA_CONTEXT_READY:
            operation = pa_context_get_source_info_list(context, sourceInfoCallBack, nullptr);
            break;
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
            pa_mainloop_quit(main_loop, 1);
            break;
        default:
            break;
    }
    if(operation) pa_operation_unref(operation);
}

vector<string> BackEnd::querySources() {
    sources.clear();
    read_names = false;
    pa_operation * operation = nullptr;
    pa_context_state_t state = pa_context_get_state(context);
    
    if (state == PA_CONTEXT_READY)
        operation = pa_context_get_source_info_list(context, sourceInfoCallBack, nullptr);
    else 
        pa_context_set_state_callback(context, contextCallBack, nullptr);

    int returned;
    while (0 <= pa_mainloop_iterate(main_loop, 1, &returned) && !read_names);
    if(operation) pa_operation_unref(operation);
    return sources;
}


void BackEnd::setSource(const string & source){
    bool in_sources = false;
    for(auto known_source : sources)
    in_sources |= source == known_source;
    
    //guarantee that the source is valid
    if(!in_sources)
        return;
    
    recorder.reset(source);
}

void BackEnd::initialize() {
    if (main_loop != nullptr || context != nullptr)
        return;

    normalization = 1;
    sources.clear();
    main_loop = pa_mainloop_new();
    pa_mainloop_api *api = pa_mainloop_get_api(main_loop);
    context = pa_context_new(api, "ListSources");
    pa_context_connect(context, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
}

void BackEnd::cleanup() {

    //clear pulse audio objects
    if (context != nullptr) {
        pa_context_disconnect(context);
        pa_context_unref(context);
        context = nullptr;
    }
    if (main_loop != nullptr) {
        pa_mainloop_free(main_loop);
        main_loop = nullptr;
    }
}

void BackEnd::createAnalyzer(float frequency){
    analyzers.try_emplace(frequency,frequency);
}

void BackEnd::destroyAnalyzer(float frequency){
    auto match = analyzers.find(frequency);
    if(match != analyzers.end()){
        analyzers.erase(match);
    }
}

void BackEnd::update(){
    recorder.record(frame);
}

float BackEnd::queryFrequency(float frequency){
    
    auto analyzer = analyzers.find(frequency);
    if (analyzer != analyzers.end()){ 
        float magnitude = (analyzer->second).execute(frame); 
        normalization = normalization > magnitude ? normalization * decay : magnitude;
        return magnitude / normalization;
    }
    else 
        return -1;
}

template<BackEnd::strategy_t strategy>
pair<float,float> BackEnd::maximum(){
    static Goertzel analizer(0.0f);
    
    static float smoothing = 0;
    static float smoothing1 = 0;
    // static float factor = 0.8;
    // static float factor1 = 0.8;
    static float factor = 0.0;
    static float factor1 = 0.0;

    float frequency;
    float magnitude;

    if constexpr (strategy == strategy_t::WBASS){
        frequency = wbass.execute(frame);
        magnitude = analizer.execute(frequency,frame);
    }
    else if constexpr (strategy == strategy_t::BASS){
        frequency = bass.execute(frame);
        magnitude = analizer.execute(frequency,frame);
    }
    if constexpr (strategy == strategy_t::FFT) {
        tie(frequency, magnitude) = fft.execute(frame);
    }

    normalization = normalization > magnitude ? normalization * decay : magnitude;
    magnitude = magnitude / normalization;
    smoothing = smoothing * factor + magnitude * (1-factor);
    smoothing1 = smoothing1 * factor1 + frequency * (1-factor1);
    return pair<float,float>(smoothing1,smoothing);
}

template std::pair<float,float> BackEnd::maximum<BackEnd::strategy_t::WBASS>();
template std::pair<float,float> BackEnd::maximum<BackEnd::strategy_t::BASS>();
template std::pair<float,float> BackEnd::maximum<BackEnd::strategy_t::FFT>();