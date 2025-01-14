#ifndef KGC_H
#define KGC_H

#include <systemc>
#include <systemc.h>
#include <tlm>
#include <boost/random.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <iostream>
#include <map>
#include <cstdint>
#include <tlm_utils/simple_initiator_socket.h>



//Ganzezahlen mit 128 Bit
//using uint128_t = boost::multiprecision::uint128_t;

using uint128_t= boost::multiprecision::uint128_t;


SC_MODULE(KGC){

public:
    //2 mal iSock für 2 NA, kann nur so lösen, sonst kann man iSock nicht 2 mal binden :/
    tlm_utils::simple_initiator_socket<KGC> iKGCSocket1;
    tlm_utils::simple_initiator_socket<KGC> iKGCSocket2;
    //std::map<int,uint128_t> public_values;
    
    SC_CTOR(KGC){
        SC_THREAD(generate_and_send_key);
    }

    void set_num_user(int num_users);


    // zur Generierung zufällig p
    uint128_t generate_random_prime();

    //Generierung Elemnte aus GF(p)
    uint128_t random_element_gf(uint128_t p);


    //Berechnung des symmetrischen Polynoms
    uint128_t symmetric_polynomial(uint128_t x, uint128_t y, uint128_t a, uint128_t b, uint128_t c, uint128_t p);
    

    //gen public Value
    void generate_unique_public_values(uint128_t p, int num_users);

    std::map<int, std::function<uint128_t(uint128_t)>>  generate_individual_polynomials (uint128_t a, uint128_t b, uint128_t c, uint128_t p);

    
   

private:
    void generate_and_send_key();

    int num_users;
    std::map<int, uint128_t> public_values;
    
    std::map<int, std::function<uint128_t(uint128_t) >> individual_polynomials;


    //Mem Variable Zufallsgenerator
    boost::random::mt19937 gen;


};

#endif // KGC_H
