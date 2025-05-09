template<size_t N>
Recorder<N>::Recorder(){
    
    pa_sample_spec sample_spec = {
        .format = PA_SAMPLE_FLOAT32,
        .rate = 48000,
        .channels = 1
    };

    pa_buffer_attr attr = {
        .maxlength = (uint32_t)-1,
        .tlength = 0,
        .prebuf = 0,
        .minreq = sizeof(float) * N,
        .fragsize = sizeof(float) * N,
    };
    
    pulse_audio_handle = pa_simple_new(
        nullptr,
        "BAS",
        PA_STREAM_RECORD,
        nullptr,
        "BAS input",
        &sample_spec,
        nullptr,
        &attr,
        nullptr
    );

    for(size_t i = 0; i < frames.size(); i++)
        pa_simple_read(
            pulse_audio_handle,
            frames[i].data(),
            N * sizeof(float),
            nullptr
        );

    ready = false;
    finished = false;
    current_frame = 0;
    index = 0;

    worker = std::thread(
        [this](){
            while(true){
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this] { return ready;});
                    ready = false;
                    if (finished) return;
                }
                
                pa_simple_read(
                    pulse_audio_handle,
                    frames[!current_frame].data(),
                    N * sizeof(float),
                    nullptr
                );
            };
            return;
        }
    );
}

template<size_t N>
Recorder<N>::~Recorder(){
    {
        std::lock_guard<std::mutex> lock(mtx);
        finished = true;
        ready = true;
    }
    cv.notify_one();
    if(worker.joinable())
        worker.join();
    pa_simple_free(pulse_audio_handle);
}

template<size_t N>
inline float Recorder<N>::record(){
    if(index == N){
        index = 0;
        current_frame = !current_frame;
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
        cv.notify_one();
    }

    float sample = frames[current_frame][index];
    index++;
    return sample;
}

template<size_t N>
inline void Recorder<N>::record(std::array<float,N> & frame){
    
    std::copy(
        frames[current_frame].begin(),
        frames[current_frame].end(),
        frame.begin()
    );

    index = 0;
    current_frame = !current_frame;
    std::lock_guard<std::mutex> lock(mtx);
    ready = true;
    cv.notify_one();
}