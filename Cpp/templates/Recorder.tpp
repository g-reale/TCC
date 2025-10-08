template<size_t N>
Recorder<N>::Recorder(const std::string & source){
    reset(source);
}

template<size_t N>
Recorder<N>::~Recorder(){
    clear();
}

template<size_t N>
void Recorder<N>::clear(){
    if(pulse_audio_handle)
        pa_simple_free(pulse_audio_handle);
    index = 0;
}

template<size_t N>
void Recorder<N>::reset(const std::string & source){
    
    clear();
    pa_sample_spec sample_spec = {
        .format = PA_SAMPLE_FLOAT32,
        .rate = SAMPLE_RATE,
        .channels = 1
    };

    pa_buffer_attr attr = {
        .maxlength = sizeof(float) * N * 3,
        .tlength = 0,
        .prebuf = 0,
        .minreq = sizeof(float) * N,
        .fragsize = sizeof(float) * N,
    };
    
    pulse_audio_handle = pa_simple_new(
        nullptr,
        "BAS",
        PA_STREAM_RECORD,
        source.empty() ? nullptr : source.data(),
        "BAS input",
        &sample_spec,
        nullptr,
        &attr,
        nullptr
    );

    pa_simple_read(
        pulse_audio_handle,
        frame.data(),
        N * sizeof(float),
        nullptr
    );
}

template<size_t N>
inline float Recorder<N>::record(){
    if(index == N){
        pa_simple_read(
            pulse_audio_handle,
            frame.data(),
            N * sizeof(float),
            nullptr
        );
        index = 0;
    }
    

    float sample = frame[index];
    index++;
    return sample;
}

template<size_t N>
inline void Recorder<N>::record(std::array<float,N> & frame){
    
    std::copy(
        this->frame.begin(),
        this->frame.end(),
        frame.begin()
    );

    pa_simple_read(
        pulse_audio_handle,
        this->frame.data(),
        N * sizeof(float),
        nullptr
    );

    index = 0;
}