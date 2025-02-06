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
#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h> 



#include "KGC.h"

using uint128_t = boost::multiprecision::uint128_t;

using namespace CryptoPP;
using CryptoPP::byte;
using CryptoPP::AES;



void NA::b_transport_from_kgc(tlm::tlm_generic_payload& trans, sc_time& delay){
    auto cmd = trans.get_command();
    auto addr = trans.get_address();
    auto data_ptr = trans.get_data_ptr();
   

    if(cmd == tlm::TLM_WRITE_COMMAND){


      



        //Empfang Koeff und r_B
        uint128_t* received_coeffs = reinterpret_cast< uint128_t*>(trans.get_data_ptr()); //wandelt den Zeiger trans in uint* um
        coefficients.clear();
        for (int i = 0; i < 2; i++)
        {
            coefficients.push_back(received_coeffs[i]);
        }

        public_value = received_coeffs[2]; //r_B speichern
        prim =received_coeffs[3];
        
        std::cout<< "NA Empfangen von Benutzer ID:"<<trans.get_address()<<", Koeffizienten = [";
        for (size_t i = 0; i < 2; i++)
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
    unsigned int data_length = trans.get_data_length();

    if(data_length==16){
        if(cmd == tlm::TLM_WRITE_COMMAND){
        
        uint128_t r_B = *reinterpret_cast<uint128_t*>(data_ptr); //Empfang r_B von anderen NA
        recieved_r_B = r_B;

        std::cout <<"NA: Emfang vom anderen NA -ID: - " <<addr<< ", empfangenes r_B =" << r_B <<std::endl;
        
       }else{
        std::cout <<"NA Errorrrrr von ID"<< id<<"hat Fehler"<< std::endl;
       }

    }
    else{


    }



    
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
}

void NA::b_transport_cipher(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay){

    auto cmd = trans.get_command();
    auto addr = trans.get_address();
    auto data_ptr = trans.get_data_ptr();
    unsigned int data_length = trans.get_data_length();
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
   
     uint128_t result;
    //g_U(x) für d=2
    int index =0;
    
    
        
        

            if(index<coefficients.size()){

            //uint128_t term = (coefficients[index]*boost::multiprecision::powm(x,i,p)%p);
            boost::multiprecision::cpp_int term = (coefficients[0]+ coefficients[1]*x)%p;
            

           result=static_cast<uint128_t>(term);
            

            



            
            
            
            index++;
            }else{
                std::cerr << "Fehler:: Index überschreitet"<< std::endl;
                return 0;
            }
        
        
   
    return result;
    
}

void NA::calculate_key(){
   
   if(recieved_r_B==0){
    std::cerr <<"NA"<<id<<" hat kein r_B vom anderen NA empfangen"<< std::endl;
    return;

   }
   //Berechnen Key
   share_key = calculate_user_polynomial(coefficients,recieved_r_B,prim);

   //Key ausgeben
   std::cout <<"NA-"<< id<<"---->>>> Schlüssel ist "<<std::hex <<share_key <<" +++++"<< std::endl;

   for(int i = 0;i<16;++i){

    share_key_array[15-i]=static_cast<uint8_t>(share_key>>(i*8));;
    
   }
   for(int i = 0;i<16;++i){

    std::cout<<" Key array in Byte --"<< share_key_array[i]<< std::endl;

    
   }

   /*

  
    encrypt_session_key();
    decryptMessage(sessionKey);
   */
   generate_session_key();
   
  hash_sha256_to_64bit(sessionKey);
  encrypt_test_data();
  decrypt_test_data(ciphertext);
  hash_sha256_to_64bit(decrypted_text);
    


   

   KeyExchange=true;

}

std::string NA::generate_session_key() {
    sessionKey.resize(16);  // 16 Bytes für 128-Bit-Schlüssel

    CryptoPP::AutoSeededRandomPool rng;

    for (size_t i = 0; i < sessionKey.size(); ++i) {
        byte random_byte;
        do {
            rng.GenerateBlock(&random_byte, 1);
        } while (random_byte == 0);  // Erneut generieren, wenn das Byte 0 ist (Null-Byte)
        
        sessionKey[i] = static_cast<char>(random_byte);
    }

    // Ausgabe des Session Keys in Klartext (kann Binärdaten enthalten)
    std::cout << "Random Session Key is - " << sessionKey << std::endl;

    // Ausgabe des Session Keys in Hex-Format zur besseren Lesbarkeit
    std::string encoded;
    CryptoPP::StringSource(sessionKey, true,
        new CryptoPP::HexEncoder(
            new CryptoPP::StringSink(encoded)
        )
    );

    std::cout << "Random Session Key (Hex): " << encoded << std::endl;
    std::cout << "Session Key Länge: " << sessionKey.size() << std::endl;

    return sessionKey;
}



const std::array<uint8_t, 16> NA::iv = { 
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
};

std::string NA::encrypt_session_key() {
   
    std::cout << "Text vor der Verschlüsselung (Hex): ";
    for (unsigned char ch : sessionKey) {
        std::cout << std::hex << static_cast<int>(ch) << " ";
    }
    std::cout << std::endl;

    CBC_Mode<AES>::Encryption encryptor;
    encryptor.SetKeyWithIV(share_key_array.data(), 16, iv.data());

    if (sessionKey.size() != 16) {
        std::cerr << "Fehler: Session Key ist nicht 16 Bytes lang!" << std::endl;
        return "";
    }

    CryptoPP::StringSource(sessionKey, true,
        new CryptoPP::StreamTransformationFilter(encryptor,
            new CryptoPP::StringSink(ciphertext),
            CryptoPP::StreamTransformationFilter::NO_PADDING
        )
    );

    // Hex-Ausgabe der verschlüsselten Nachricht
    std::string encoded;
    CryptoPP::StringSource(ciphertext, true,
        new CryptoPP::HexEncoder(
            new CryptoPP::StringSink(encoded)
        )
    );

    std::cout << "Verschlüsselte Nachricht (Hex): " << encoded << std::endl;

    return ciphertext;
}


std::string NA::decryptMessage(const std::string& ciphertext) {
    using namespace CryptoPP;

    std::string decryptedtext;

    CBC_Mode<AES>::Decryption decryptor;
    decryptor.SetKeyWithIV(share_key_array.data(), 16, iv.data());

    if (ciphertext.size() % AES::BLOCKSIZE != 0) {
        std::cerr << "Fehler: Ciphertext ist kein Vielfaches der Blockgröße!" << std::endl;
        return "";
    }

    try {
        StringSource(ciphertext, true,
            new StreamTransformationFilter(decryptor,
                new StringSink(decryptedtext),
                StreamTransformationFilter::NO_PADDING
            )
        );

        // Hex-Ausgabe der entschlüsselten Nachricht
        std::string decrypted_hex;
        CryptoPP::StringSource(decryptedtext, true,
            new HexEncoder(
                new CryptoPP::StringSink(decrypted_hex)
            )
        );

        std::cout << "Entschlüsselte Nachricht (Hex): " << decrypted_hex << std::endl;
        std::cout << "Entschlüsselte Nachricht (Klartext): " << decryptedtext << std::endl;

    } catch (const CryptoPP::Exception& e) {
        std::cerr << "Fehler bei der Entschlüsselung: " << e.what() << std::endl;
    }

    return decryptedtext;
}


uint64_t NA::hash_sha256_to_64bit(const std::string& input) {
    using namespace CryptoPP;

    // SHA-256-Hash (256 Bit) berechnen
    byte sha_digest[SHA256::DIGESTSIZE];  // SHA256::DIGESTSIZE = 32 Bytes = 256 Bit

    SHA256 hash;
    hash.Update(reinterpret_cast<const byte*>(input.data()), input.size());
    hash.Final(sha_digest);

    // Ausgabe des SHA-256-Hash (256 Bit) im Hex-Format
    std::string sha256_hex;
    StringSource(sha_digest, sizeof(sha_digest), true,
        new HexEncoder(
            new StringSink(sha256_hex)
        )
    );

    std::cout << "Original SHA-256 Hash (Hex): " << sha256_hex << std::endl;

    // 256-Bit-Hash auf 64-Bit reduzieren (XOR der vier 64-Bit-Blöcke)
    uint64_t* hash_parts = reinterpret_cast<uint64_t*>(sha_digest);
    uint64_t hash64 = hash_parts[0] ^ hash_parts[1] ^ hash_parts[2] ^ hash_parts[3];

    // Ausgabe des 64-Bit-Hashs in Dezimal- und Hex-Format
    std::cout << "Reduzierter 64-Bit Hash (Dezimal): " << hash64 << std::endl;
    std::cout << "Reduzierter 64-Bit Hash (Hex): " << std::hex << hash64 << std::endl;

    return hash64;
}

std::string NA::encrypt_test_data() {
    

    // Testdaten (16 Bytes)
    const std::string test_data = "HalloWorldffffff";  // 16 Zeichen

    std::cout << "Testdaten vor der Verschlüsselung (Klartext): " << sessionKey << std::endl;
    std::string sessionKeyHex;
    CryptoPP::StringSource(sessionKey, true,
    new CryptoPP::HexEncoder(
        new CryptoPP::StringSink(sessionKeyHex)
    )
    );

std::cout << "Testdaten vor der Verschlüsselung (Hex): " << sessionKeyHex << std::endl;

    CBC_Mode<AES>::Encryption encryptor;
    encryptor.SetKeyWithIV(share_key_array.data(), 16, iv.data());

    // Sicherstellen, dass die Testdaten 16 Bytes lang sind
    if (sessionKey.size() != 16) {
        std::cerr << "Fehler: Testdaten sind nicht 16 Bytes lang!" << std::endl;
        return "";
    }

    // Verschlüsselung ohne Padding
    CryptoPP::StringSource(sessionKey, true,
        new CryptoPP::StreamTransformationFilter(encryptor,
            new CryptoPP::StringSink(ciphertext),
            CryptoPP::StreamTransformationFilter::NO_PADDING
        )
    );

    // Hex-Ausgabe der verschlüsselten Nachricht
    std::string encoded;
    CryptoPP::StringSource(ciphertext, true,
        new CryptoPP::HexEncoder(
            new CryptoPP::StringSink(encoded)
        )
    );

    std::cout << "Verschlüsselte Testdaten (Hex): " << encoded << std::endl;

    return ciphertext;
}
std::string NA::decrypt_test_data(const std::string& ciphertext) {
    using namespace CryptoPP;

    

    CBC_Mode<AES>::Decryption decryptor;
    decryptor.SetKeyWithIV(share_key_array.data(), 16, iv.data());

    // Sicherstellen, dass der Ciphertext ein Vielfaches von 16 Bytes ist
    if (ciphertext.size() % AES::BLOCKSIZE != 0) {
        std::cerr << "Fehler: Ciphertext ist kein Vielfaches der Blockgröße!" << std::endl;
        return "";
    }

    try {
        // Entschlüsselung ohne Padding
        CryptoPP::StringSource(ciphertext, true,
            new CryptoPP::StreamTransformationFilter(decryptor,
                new CryptoPP::StringSink(decrypted_text),
                CryptoPP::StreamTransformationFilter::NO_PADDING
            )
        );

        // Hex-Ausgabe der entschlüsselten Nachricht
        std::string decrypted_hex;
        CryptoPP::StringSource(decrypted_text, true,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(decrypted_hex)
            )
        );

        std::cout << "Entschlüsselte Testdaten (Hex): " << decrypted_hex << std::endl;
        std::cout << "Entschlüsselte Testdaten (Klartext): " << decrypted_text << std::endl;

    } catch (const CryptoPP::Exception& e) {
        std::cerr << "Fehler bei der Entschlüsselung: " << e.what() << std::endl;
    }

    return decrypted_text;
}


