#include <systemc>
#include<tlm>


#include "TopLevel.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    
    int num_users=1;

    TopLevel top("TopLevel");
    
    top.kgc.set_num_user(1);

    sc_start(1000, SC_NS);

    std::cout << "Simulation beendet."<< std::endl;

    return 0;
}
