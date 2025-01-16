
#include <systemc>
#include "TopLevel.h"

#include "NA.h"

void TopLevel::start_simulation(){

    std::cout <<"Simulation Starttttttt ....." << std::endl;
    sc_start(10,SC_MS);
    std::cout <<"ist die start Simulation abgeschlossen?"<< std::endl;

    kgc_done_event.notify();
}

    




void TopLevel::start_key_exchange_process(){
    std::cout <<"Toplevel warten auf kgc_done_event ...."<<std::endl;
    //wait(kgc_done_event);

   
    std::cout <<"KGC Done, NA start Schlüsselaustausch ...."<<std::endl;

    na1.start_key_exchange(&na2);

    na2.start_key_exchange(&na1);
    
    //sc_start(100,SC_NS);
    

    na1.calculate_key();
    na2.calculate_key();

}