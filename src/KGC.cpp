#include "KGC.h"
#include <boost/random.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <iostream>

KGC::KGC() : iKGCSocket("iKGCSocket"){
    SC_THREAD(generate_and_send_key); //SC thread schlüsselerzeugung
}

//Generiernung p
__uint128_t KGC :: generate_random_prime(){
    boost::random::mt19937 gen(static_cast<unsigned int>(time(0)));
    boost:: random::uniform_int_distribution<__uint128_t> dist(1e15,1e16);

    while(true){
        __uint128_t candidate = dist(gen) | 1; //ungerade Zahl generieren
    }
}