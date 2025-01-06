#include <systemc>
#include<tlm>

#include "Initiator.h"
#include "Target.h"
#include "TopLevel.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    
    // SystemC Initialisierung
    sc_clock clk("clk", 10, SC_NS);

    // Instanziere Initiator und Target
    Initiator initiator("Initiator");
    Target target("Target");

    // Verbindungen erstellen
    initiator.socket.bind(target.socket);

    // Simulation starten
    sc_start(100, SC_NS);

    return 0;
}
