#include "../include/BackEnd.hpp"

using namespace std;

atomic<bool> BackEnd::finished = false;
atomic<bool> BackEnd::stopped = false;
mutex BackEnd::critical;
mutex BackEnd::processing;
condition_variable BackEnd::condition;
thread BackEnd::worker;
Recorder<BUFFER_SIZE> BackEnd::recorder("");
unordered_map<float,float> BackEnd::frequencies;
unordered_map<float,Goertzel> BackEnd::analyzers;

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
    
    stop();
    recorder.reset(source);
    start();
}

void BackEnd::initialize() {
    if (main_loop != nullptr || context != nullptr)
        return;

    sources.clear();
    main_loop = pa_mainloop_new();
    pa_mainloop_api *api = pa_mainloop_get_api(main_loop);
    context = pa_context_new(api, "ListSources");
    pa_context_connect(context, nullptr, PA_CONTEXT_NOFLAGS, nullptr);

    worker = thread(
    []{
        array<float,BUFFER_SIZE> frame;
        while(!finished){
            throttle<SAMPLE_RATE/BUFFER_SIZE>(
                [&frame]{
                    unique_lock<mutex> lock(critical);
                    condition.wait(lock,[]{return !stopped || finished;});

                    processing.lock();
                    recorder.record(frame);
                    for (auto& [frequency, analyzer] : analyzers){
                        frequencies[frequency] = analyzer.execute<BUFFER_SIZE>(frame);
                        analyzer.clear();
                    }
                    processing.unlock();
                }
            );
        }
    });
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

    //delete the worker thread
    finished = true;
    condition.notify_all();
    if(worker.joinable())
        worker.join();
    
}

void BackEnd::createAnalyzer(float frequency){
    stop();
    analyzers.try_emplace(frequency,frequency);
    frequencies.try_emplace(frequency,0);
    start();
}

void BackEnd::destroyAnalyzer(float frequency){
    stop();
    auto match = analyzers.find(frequency);
    if(match != analyzers.end()){
        analyzers.erase(match);
        frequencies.erase(frequencies.find(frequency));
    }
    start();
}

void BackEnd::start(){
    stopped = false;
    condition.notify_all();
}

void BackEnd::stop(){
    stopped = true;
    processing.lock();
    processing.unlock();
}

float BackEnd::queryFrequency(float frequency){
    return frequencies[frequency];
}