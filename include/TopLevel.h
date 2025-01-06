#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <systemc>
#include "Initiator.h"
#include "Target.h"

SC_MODULE(TopLevel) {
    Initiator* initiator;
    Target* target;

    sc_signal<bool> transaction_status_signal; //Signal zur Übertragung von trans
    //sc_export<sc_signal<bool> exported_signal; //exportiertes Signal

    SC_CTOR(TopLevel) {
        initiator = new Initiator("Initiator");
        target = new Target("Target");

        // Verbindungen herstellen
        initiator->socket.bind(target->socket);

        initiator -> transaction_status.write(transaction_status_signal.read()); //Initiator gibt den Status an
        target ->transaction_status.write(transaction_status_signal.read()); //Target kriegt dan Status

        SC_THREAD(monitor_transaction_status); //monitor für das Signal
    }

    void monitor_transaction_status(){
        while(true){
            wait(transaction_status_signal.posedge()); //warten auf Statusänderung
            if(transaction_status_signal.read()){
                std::cout << "Transaktion erfolgreich abgeschlossen!" << std::endl;

            }else{
                std::cout << "Fehler bei der Transaktion!!" << std::endl;
            }
            
        }
    }

    ~TopLevel() {
        delete initiator;
        delete target;
    }
};

#endif // TOPLEVEL_H
