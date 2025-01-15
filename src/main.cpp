#include <systemc>
#include<tlm>


#include "TopLevel.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    
   

    TopLevel top("TopLevel");
    
    top.kgc.set_num_user(2);

    top.start_simulation();
    top.start_key_exchange_process();

    std::cout << "Simulation beendet."<< std::endl;

    return 0;
}
