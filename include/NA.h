#ifndef NA_H
#define NA_H


#include <systemc>
#include <systemc.h>
#include <tlm>
#include<iostream>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <cryptopp/osrng.h>
#include <cryptopp/aes.h>
#include<cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/aes.h>


using uint128_t = boost::multiprecision::uint128_t;
using namespace CryptoPP;
using CryptoPP::byte;

class NA : public sc_module {
public:

    //Target-Socket für NA von KGC
    tlm_utils::simple_target_socket <NA> tNAKGCSocket;

    //Target Socket für Empfang von anderen NAs
    tlm_utils::simple_target_socket<NA> tNANASocket;

    //iSock fürs Senden an anderen NAs
    tlm_utils::simple_initiator_socket<NA> iNANASocket;

    tlm_utils::simple_initiator_socket<NA> iSockCipher;
    tlm_utils::simple_target_socket<NA>tSockCipher;

    
    uint128_t public_value; //Öffentlicher Param

    uint128_t recieved_r_B;
    
    
    //ID des aktuellen NA
    int id=0;

    //Konstruktor
    SC_HAS_PROCESS(NA);
    NA(sc_module_name name, int na_id)
        :sc_module(name), id(na_id), tNAKGCSocket("tNAKGCSocket"),
         tNANASocket("tNASocket"), iNANASocket("iNASocket"),iSockCipher("iSockCipher"), tSockCipher("tSockCipher") 
    {

        tNAKGCSocket.register_b_transport(this, &NA::b_transport_from_kgc); //register für KGC-NA
        tNANASocket.register_b_transport(this, &NA::b_transport_from_na); //register für NA-NA
        tSockCipher.register_b_transport(this,&NA::b_transport_cipher);
    }


    void b_transport_from_kgc(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);
    void b_transport_from_na(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);
    void b_transport_cipher(tlm::tlm_generic_payload& trans,sc_core::sc_time& delay);
    //Schlüsselaustausch mit anderen NA
    void start_key_exchange(NA* other_na);

    std::string generate_session_key();
    std::string encrypt_session_key();
    std::string decryptMessage(const std::string& ciphertext);

    std::string decrypt_test_data(const std::string& ciphertext);
    std::string encrypt_test_data();
    uint64_t hash_sha256_to_64bit(const std::string& input);


    void calculate_key();
    uint128_t calculate_user_polynomial(std::vector<boost::multiprecision::uint128_t>&coefficients,uint128_t x, uint128_t p);

private:

//verarbeitung empfangende Nachrichtn
    
    void init();
    uint128_t share_key; // gemeinsamer Schlüssel
    std::vector<uint128_t> coefficients; //Dynamisch für Koeff gon g(x)

    


    bool KeyExchange = false;
    std::string sessionKey;

    std::string decryptedtext;
    uint128_t prim;

    //iv
    static const std::array<uint8_t, 16> iv;
    std::array<uint8_t,16> share_key_array;

    const std::array<uint8_t, 16> test_key = {
    0x00, 0x00, 0x03, 0x04, 
    0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 
    0x0D, 0x0E, 0x0F, 0x10
};

 std::string ciphertext;
 std::string decrypted_text;


    
    
    



};

#endif //NA_H