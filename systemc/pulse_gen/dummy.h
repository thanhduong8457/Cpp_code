#include "systemc.h"

class dummy: public sc_module
{
    sc_out<bool> dummy_enable;
    sc_out<bool> dummy_preset;
    sc_out<bool> dummy_wr;

    sc_out< sc_dt::uint64 > dummy_N;
    sc_out< sc_dt::uint64 > dummy_Nx;

    sc_in<bool> dummy_pulse_x_in;
    sc_in<bool> dummy_dir_x_in;
    sc_in<bool> dummy_flag_in;
    sc_in<bool> dummy_clk1_in;

    //pointting to tracefile in contructor
    sc_trace_file *pTraceFile;

    dummy(void);
    void simulation();

};