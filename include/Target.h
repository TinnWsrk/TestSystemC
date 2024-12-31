#ifndef TARGET_H
#define TARGET_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

SC_MODULE(Target) {
    tlm_utils::simple_target_socket<Target> socket; // Target-Socket

    SC_CTOR(Target) : socket("socket") {
        socket.register_b_transport(this, &Target::b_transport); // b_transport registrieren
    }

    void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay); // Verarbeite Transaktion
};

#endif // TARGET_H
