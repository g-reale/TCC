template<size_t N, typename datatype>
void Circular<N,datatype>::push(datatype sample){
    memory[index] = sample;
    index = (index + 1) % memory.size();
}

template<size_t N, typename datatype>
datatype Circular<N,datatype>::operator[](size_t i) const{
    return memory[(memory.size() + index - i - 1) % memory.size()];
}

template<size_t N, typename datatype>
void Circular<N,datatype>::clear(){
    std::fill(memory.begin(),memory.end(),0);
}