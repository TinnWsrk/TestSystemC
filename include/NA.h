#ifndef NA_H
#define NA_H


#include <systemc>
#include <systemc.h>
#include <tlm>
#include<iostream>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <boost/multiprecision/cpp_int.hpp>

using uint128_t = boost::multiprecision::uint128_t;

class NA : public sc_module {
public:

    //Target-Socket für NA von KGC
    tlm_utils::simple_target_socket <NA> tNAKGCSocket;

    //Target Socket für Empfang von anderen NAs
    tlm_utils::simple_target_socket<NA> tNANASocket;

    //iSock fürs Senden an anderen NAs
    tlm_utils::simple_initiator_socket<NA> iNANASocket;

    
    uint128_t public_value; //Öffentlicher Param

    uint128_t recieved_r_B;
    uint128_t coeff_a;
    uint128_t coeff_b;
    uint128_t coeff_c;
    int id=0;

    //Konstruktor
    SC_HAS_PROCESS(NA);
    NA(sc_module_name name, int na_id)
        :sc_module(name), id(na_id), tNAKGCSocket("tNAKGCSocket"),
         tNANASocket("tNASocket"), iNANASocket("iNASocket") 
    {

        tNAKGCSocket.register_b_transport(this, &NA::b_transport_from_kgc); //register für KGC-NA
        tNANASocket.register_b_transport(this, &NA::b_transport_from_na); //register für NA-NA
    }


    void b_transport_from_kgc(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);
    void b_transport_from_na(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);
    //Schlüsselaustausch mit anderen NA
    void start_key_exchange(NA* other_na);


    void calculate_key();

private:

//verarbeitung empfangende Nachrichtn
    
    void init();
    uint128_t share_key; // gemeinsamer Schlüssel



};

#endif //NA_H