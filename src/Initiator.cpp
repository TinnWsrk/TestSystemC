

#include "Initiator.h"
#include <cstdint>
#include <iostream>
#include <tlm>





void Initiator::process() {
    tlm::tlm_generic_payload trans; // Transaktionsobjekt
    sc_core::sc_time delay = sc_core::SC_ZERO_TIME; // Verzögerung

    uint32_t data = 0xABBA; // Beispiel-Daten
    trans.set_command(tlm::TLM_WRITE_COMMAND); // Schreibe-Transaktion
    trans.set_address(0x1000); // Adresse
    trans.set_data_ptr(reinterpret_cast<unsigned char*>(&data)); // Datenzeiger
    trans.set_data_length(4); // Datenlänge in Bytes

    std::cout << "Initiator: Sende Transaktion mit Daten: 0x" << std::hex << data << std::endl;

    // Transaktion senden
    socket->b_transport(trans, delay);

    // Antwortstatus prüfen
    if (trans.get_response_status() == tlm::TLM_OK_RESPONSE) {
        std::cout << "Initiator: Transaktion erfolgreich abgeschlossen!" << std::endl;
    }
}
