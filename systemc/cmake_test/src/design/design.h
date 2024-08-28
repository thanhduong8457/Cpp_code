// A 4 bit up-counter with synchronous active high reset
// and with active high enable signal
#include "systemc.h"

SC_MODULE(first_counter)
{
    sc_in_clk     clock;      // Clock input of the design
    sc_in<bool>   reset;      // active high, synchronous Reset input
    sc_in<bool>   enable;      // Active high enable signal for counter
    sc_out<sc_uint<4> > counter_out; // 4 bit vector output of the counter

    //------------Local Variables Here---------------------
    sc_uint<4>	count;
    //------------Code Starts Here-------------------------
    // Below function implements actual counter logic
    void incr_count();

    // Constructor for the counter
    // Since this counter is a positive edge trigged one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state
    SC_CTOR(first_counter)
    {
        cout << "Executing new" << endl;
        SC_METHOD(incr_count);
        sensitive << reset;
        sensitive << clock.pos();
    } // End of Constructor

}; // End of Module counter
