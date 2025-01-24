#include"NA.h"
#include <systemc>
#include <systemc.h>
#include <tlm>
#include<iostream>
#include<tlm_utils/simple_target_socket.h>
#include <boost/multiprecision/cpp_int.hpp>


#include <cryptopp/osrng.h>
#include <cryptopp/aes.h>
#include<cryptopp/modes.h>
#include <cryptopp/filters.h>


#include "KGC.h"

using uint128_t = boost::multiprecision::uint128_t;
using namespace CryptoPP;

void NA::b_transport_from_kgc(tlm::tlm_generic_payload& trans, sc_time& delay){
    auto cmd = trans.get_command();
    auto addr = trans.get_address();
    auto data_ptr = trans.get_data_ptr();

    if(cmd == tlm::TLM_WRITE_COMMAND){
        //Empfang Koeff und r_B
        uint128_t* received_coeffs = reinterpret_cast< uint128_t*>(trans.get_data_ptr()); //wandelt den Zeiger trans in uint* um
        coefficients.clear();
        for (int i = 0; i < 9; i++)
        {
            coefficients.push_back(received_coeffs[i]);
        }

        public_value = received_coeffs[9]; //r_B speichern
        prim =received_coeffs[10];
        
        std::cout<< "NA Empfangen von Benutzer ID:"<<trans.get_address()<<", Koeffizienten = [";
        for (size_t i = 0; i < 9; i++)
        {
            std::cout << coefficients[i];
            if(i<coefficients.size()-1) {

                std::cout << ", ";
            } 
        }
        std::cout << "--und, r_B= " << public_value << ", p = "<< prim << std::endl;
        
    

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
    std::cout <<"Benutzer-ID -"<< id<<"start key-exchange"<< std::endl;

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

uint128_t  NA::calculate_user_polynomial(std::vector<boost::multiprecision::uint128_t>&coefficients,uint128_t x, uint128_t p){
    boost::multiprecision::cpp_int result =0;
    uint128_t result_128 =0;
    //g_U(x) für d=2
    int index =0;
    for (int i = 0; i <= 2; i++)
    {
        for (int j = 0; j <= 2; j++)
        {

            if(index<coefficients.size()){
            boost::multiprecision::cpp_int term = coefficients[index]*boost::multiprecision::pow(x,i);
            


            
            result = (result+term)%prim; // mod p
            /*
            std::cout<<"p =" <<prim <<" und r_B = "<< r_B<<std::endl; 


            std::cout<<coefficients[index]<<" * "<< x << "^ "<<i<< "*  "<<r_B<< "^ "<<j<<" = "<<result<< std::endl;
            */
            index++;
            }else{
                std::cerr << "Fehler:: Index überschreitet"<< std::endl;
                return 0;
            }
        }
        
    }

    result_128 = static_cast<uint128_t>(result%p);
    return result_128;
    
}

void NA::calculate_key(){
   
   if(recieved_r_B==0){
    std::cerr <<"NA"<<id<<" hat kein r_B vom anderen NA empfangen"<< std::endl;
    return;

   }
   //Berechnen Key
   share_key = calculate_user_polynomial(coefficients,recieved_r_B,prim);

   //Key ausgeben
   std::cout <<"NA-"<< id<<"---->>>> Schlüssel ist "<<share_key <<" +++++"<< std::endl;

}

std::string generate_session_key(){
    return "";

}

std::string encrypt_session_key(const std::string& session_key, const std::string& share_key){
    return "";
}