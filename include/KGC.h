#ifndef KGC_H
#define KGC_H

#include <systemc>
#include <systemc.h>
#include <tlm>
#include <boost/random.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <iostream>

#include <cstdint>
#include <tlm_utils/simple_initiator_socket.h>

using namespace boost::multiprecision; //für große Zahlen
using boost::random;

//Ganzezahlen mit 128 Bit
//using uint128_t = boost::multiprecision::uint128_t;

using uint128_t= boost::multiprecision::uint128_t;


class KGC : public sc_module {

public:
    
    tlm_util::simple_initiator_socket<KGC> iKGCSocket;

    KGC(sc_module_name name);

    
    // zur Generierung zufällig p
    uint128_t generate_random_prime();

    //Generierung Elemnte aus GF(p)
    uint128_t random_element_gf(uint128_t p);


    //Berechnung des symmetrischen Polynoms
    uint128_t symmetric_polynomial(uint128_t x, uint128_t y, uint128_t a, uint128_t b, uint128_t c, uint128_t p);
    

    //gen public Value
    void generate_unique_public_values(uint128_t p, int num_users);

    std::map<int, std::function < uint128_t a, uint128_t b, uint128_t c, uint128_t p);

    void generate_and_send_key(int num_users);
   

private:
    std::map<int, uint128_t> public_values;


};

#endif // KGC_H
