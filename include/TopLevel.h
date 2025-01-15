#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <systemc>
#include "KGC.h"
#include "NA.h"

class TopLevel : public sc_module {
public:

    KGC kgc;
    NA na1;
    NA na2;

   sc_event kgc_done_event;

    SC_CTOR(TopLevel)
        : kgc("KGC"), na1("NA1",1), na2("NA2", 2)
    {
        kgc.iKGCSocket1.bind(na1.tNAKGCSocket);
        kgc.iKGCSocket2.bind(na2.tNAKGCSocket);

        //NA1 und NA2 miteinander verbinden
        na1.iNANASocket.bind(na2.tNANASocket);
        na2.iNANASocket.bind(na1.tNANASocket);

        //SC_THREAD(start_key_exchange_process);

        //SC_METHOD(start_key_exchange_process);
        //dont_initialize();
        //sensitive << kgc_done_event;
    
    }


    void start_simulation();
    void start_key_exchange_process();

};
#endif // TOPLEVEL