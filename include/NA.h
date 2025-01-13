#ifndef TARGET_H
#define TARGET_H

#include <systemc>
#include <systemc.h>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

SC_MODULE(Target) {
public:
    tlm_utils::simple_target_socket<Target> socket; // Target-Socket
    sc_signal<bool> transaction_status;

    SC_CTOR(Target) : socket("socket") {
        //SC_METHOD(b_transport_thread); //Überwache b_transport
       // sensitive << socket;

       socket.register_b_transport(this, &Target::b_transport);

    
}
//void b_transport_thread(); //Wrapper-Methode ohne Parameter

void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay); // Verarbeite Transaktion


};
#endif // TARGET_H
