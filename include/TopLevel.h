#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <systemc>
#include "KGC.h"
#include "NA.h"

class TopLevel : public sc_module {
public:

    KGC kgc;
    NA na;

    SC_CTOR(TopLevel)
        : kgc("KGC"), na("NA")
    {
        kgc.iKGCSocket.bind(na.tNASocket);
    }
    


};
#endif // TOPLEVEL