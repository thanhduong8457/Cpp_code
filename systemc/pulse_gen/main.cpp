#include <systemc>
#include "dummy.h"

using namespace sc_core;

dummy *top = NULL;

int sc_main(int , char* [])
{
    top = new dummy("top");

    // Start simulation
    sc_start();// run indefinitely

    return 0;
}// end sc_main()
