#include "../include/BAS.hpp"

BAS::BAS(float alpha, float beta, size_t iterations, float power, float trust) : analizer(0){
    set(alpha,beta,iterations,power,trust);
}

void BAS::set(float alpha, float beta, size_t iterations, float power, float trust){
    this->alpha = alpha;
    this->beta = beta;
    this->iterations = iterations;
    this->power = power;
    this->trust = trust;
}