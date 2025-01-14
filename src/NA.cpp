#include"NA.h"
#include <systemc>
#include <systemc.h>
#include <tlm>
#include<iostream>
#include<tlm_utils/simple_target_socket.h>
#include <boost/multiprecision/cpp_int.hpp>

using uint128_t = boost::multiprecision::uint128_t;

void NA::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay){
    auto cmd = trans.get_command();
    auto addr = trans.get_address();
    auto data_ptr = trans.get_data_ptr();

    if(cmd == tlm::TLM_WRITE_COMMAND){
        //Empfang den wert in 128 bit
        uint128_t received_value = *reinterpret_cast< uint128_t*>(data_ptr);


        //Ausgabe der Daten
        std::cout << "NA:Emfangen Von Benuter-ID:" << addr << ", Wert 0x" << std::hex << received_value << std::endl;

    }
    else{
        std::cout << "Errorrrrrrrrrr" << std::endl;
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    
}