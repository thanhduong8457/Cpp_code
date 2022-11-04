#include "dummy.h"

void dummy::simulation()
{
    dummy_N.write(10);

    for (int i = 1; i <= 10; i++)
    {
        dummy_Nx.write(2);
        dummy_Ny.write(5);
        dummy_Nz.write(8);
        dummy_wr.write(0);
        wait(1, SC_NS);
        dummy_wr.write(1);
        wait(1, SC_NS);
    }

    dummy_enable.write(1);

    sc_stop();
}