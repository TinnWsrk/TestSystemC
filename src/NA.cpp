#include "Target.h"
#include <cstdint>
#include <iostream>



/* 
void Target::b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay) {
    auto cmd = trans.get_command(); // Befehl (READ/WRITE)
   /auto addr = trans.get_address(); // Zieladresse
    auto data_ptr = trans.get_data_ptr(); // Zeiger auf die übertragenen Daten, zeigt auf einen Speicherbereich, der die zu lesenden oder zu schreibenden Daten hat
    

    if (cmd == tlm::TLM_WRITE_COMMAND) {
        uint32_t data = *reinterpret_cast<uint32_t*>(data_ptr);
        std::cout << "Target: Schreibe Daten 0x" << std::hex << data
                 << " an Adresse 0x" << addr << std::endl;
    } else if (cmd == tlm::TLM_READ_COMMAND) {
        uint32_t response = 0xDEADBEEF;
        memcpy(data_ptr, &response, sizeof(response));
        std::cout << "Target: Lese-Daten: 0x" << std::hex << response
                 << " von Adresse 0x" << addr << std::endl;
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE); // Erfolgreiche Verarbeitung
    transaction_status.write(true);
} */

/*void Target:: b_transport_thread (){
    while(true){
        tlm::tlm_generic_payload trans;
        sc_core::sc_time delay = SC_ZERO_TIME;

        b_transport(trans, delay); //Transaktion verarbeiten, b_transport aufrufen
        wait(10,SC_NS); //warten auf den nächsten Zyklus
    }
}*/

void Target::b_transport(tlm::tlm_generic_payload& trans, sc_core ::sc_time& delay){
    auto cmd = trans.get_command();
    auto addr = trans.get_address();
    auto data_ptr = trans.get_data_ptr();

    if(cmd == tlm:: TLM_WRITE_COMMAND){
        uint32_t response = 0xDEADBEEF;
        memcpy(data_ptr, &response, sizeof(response));
        std::cout << "Target:Lesen-Daten:0x" << std::hex <<response
                  << "von Adresse 0x" << addr << std::endl;
    }
    trans.set_response_status(tlm::TLM_OK_RESPONSE); //Antwort setzen
    transaction_status.write(true); //status neu
}



