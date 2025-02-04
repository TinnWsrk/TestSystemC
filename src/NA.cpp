#include"NA.h"
#include <systemc>
#include <systemc.h>
#include <tlm>
#include<iostream>
#include<tlm_utils/simple_target_socket.h>
#include <boost/multiprecision/cpp_int.hpp>
#include "KGC.h"

using uint128_t = boost::multiprecision::uint128_t;

void NA::b_transport_from_kgc(tlm::tlm_generic_payload& trans, sc_time& delay){
    auto cmd = trans.get_command();
    auto addr = trans.get_address();
    auto data_ptr = trans.get_data_ptr();

    if(cmd == tlm::TLM_WRITE_COMMAND){
        //Empfang Koeff und r_B
        uint128_t* received_coeffs = reinterpret_cast< uint128_t*>(trans.get_data_ptr()); //wandelt den Zeiger trans in uint* um
        coeff_a = received_coeffs[0]; // a
        coeff_b = received_coeffs[1]; //b*r_B
        coeff_c = received_coeffs[2];
        public_value = received_coeffs[3]; //r_B

        

        //Ausgabe der Daten
        std::cout << "NA:Emfangen Von Benuter-ID:" << trans.get_address() << 
                    ", a = " << coeff_a << ", coeff b =" << coeff_b << ", coeff c = " << ", r_B =" << public_value << std::endl;

    }
    else{
        std::cout << "Errorrrrrrrrrr" << std::endl;
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    
}

//verarbeiten Nachrichten von einen anderen NA
void NA::b_transport_from_na(tlm::tlm_generic_payload& trans, sc_time& delay){
    auto cmd = trans.get_command();
    auto addr = trans.get_address();
    auto data_ptr = trans.get_data_ptr();

    if(cmd == tlm::TLM_WRITE_COMMAND){
        
        uint128_t r_B = *reinterpret_cast<uint128_t*>(data_ptr); //Empfang r_B von anderen NA
        recieved_r_B = r_B;

        std::cout <<"NA: Emfang vom anderen NA -ID: - " <<addr<< ", empfangenes r_B =" << r_B <<std::endl;

    
    }else{
        std::cout <<"NA Errorrrrr von ID"<< id<<"hat Fehler"<< std::endl;
    }
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
}

void NA::start_key_exchange(NA* other_na){

    if(public_value==0){
        std::cerr << "NA: hat kein r_B von KGC" << std::endl;
        return;
    }


    
    tlm::tlm_generic_payload trans;
    sc_core::sc_time delay = sc_core :: SC_ZERO_TIME;

    //Array zur Speichern öff Wert und evtl etwas mehr
    uint128_t r_B_array[1] = {public_value};

    //Trans erstellen
    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(id);
    trans.set_data_ptr(reinterpret_cast<unsigned char*>(r_B_array));
    trans.set_data_length(sizeof(r_B_array));

    std::cout <<"NA:ID"<<id<<" Sende seine r_B an NA -ID " <<other_na->id<<" :++ " << std::endl;
    //Trans senden an other na
    iNANASocket->b_transport(trans, delay);

    if(trans.get_response_status()!= tlm::TLM_OK_RESPONSE){
        std::cerr <<"NA beim Senden an NA Errorrrrrr id " <<id << "hat Fehler mit NA id -"<<other_na->id<<  std::endl;
    }

}

void NA::calculate_key(){
   
   if(recieved_r_B==0){
    std::cerr <<"NA"<<id<<" hat kein r_B vom anderen NA empfangen"<< std::endl;
    return;

   }
   //Berechnen Key
   uint128_t key = (coeff_a*recieved_r_B*recieved_r_B+recieved_r_B*coeff_b+coeff_c*recieved_r_B) ;

   //Key ausgeben
   std::cout <<"NA-"<< id<<"---->>>> Schlüssel ist "<<key <<" +++++"<< std::endl;

   share_key = key;


}