#ifndef NA_H
#define NA_H


#include <systemc>
#include <systemc.h>
#include <tlm>
#include<iostream>
#include <boost/multiprecision/cpp_int.hpp>

using uint128_t = boost::multiprecision::uint128_t;

class NA : public sc_module {
public:

    //Target-Socket für NA von KGC
    tlm_utils::simple_target_socket <NA> tNASocket;

    //Konstruktor

    SC_CTOR(NA) : tNASocket("tNASocket"){

        tNASocket.register_b_transport(this, &NA::b_transport);
    }

    void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);

private:



};

#endif //NA_H