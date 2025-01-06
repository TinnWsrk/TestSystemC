#ifndef INITIATOR_H
#define INITIATOR_H

#include <systemc>
#include <systemc.h>
#include <tlm>

#include <cstdint>
#include <tlm_utils/simple_initiator_socket.h>



SC_MODULE(Initiator) {
public:
    tlm_utils::simple_initiator_socket<Initiator> socket; // Initiator-Socket
    sc_signal<bool> transaction_status;// Signal für den Transaktionsstatus

    SC_CTOR(Initiator) : socket("socket") {
        SC_THREAD(process); // Prozess starten
    }

    void process(); // Prozess, der Transaktionen sendet
};

#endif // INITIATOR_H
