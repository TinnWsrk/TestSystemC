#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <systemc>
#include "Initiator.h"
#include "Target.h"

SC_MODULE(TopLevel) {
    Initiator* initiator;
    Target* target;

    SC_CTOR(TopLevel) {
        initiator = new Initiator("Initiator");
        target = new Target("Target");

        // Verbindungen herstellen
        initiator->socket.bind(target->socket);
    }

    ~TopLevel() {
        delete initiator;
        delete target;
    }
};

#endif // TOPLEVEL_H
