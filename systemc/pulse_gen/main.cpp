#include <iostream>
#include "pulse_gen.h"

int sc_main(int argc, char* argv[]) {
	sc_signal<sc_dt::uint64> clk;
	sc_signal<bool> enable;
	sc_signal<bool> reset;
	sc_signal<bool> write;
	sc_signal<bool> pulse;

	sc_signal<sc_dt::uint64> n;
	sc_signal<sc_dt::uint64> nx;

	sc_trace_file *pTraceFile;

	pulse_gen *my_pulse_gen = NULL;
	my_pulse_gen = new pulse_gen("my_pulse_gen");

	//defining and identifying signals
	my_pulse_gen->clk(clk);
	my_pulse_gen->reset(reset);
	my_pulse_gen->enable(enable);
	my_pulse_gen->write(write);
	my_pulse_gen->N(n);
	my_pulse_gen->Nx(nx);
	my_pulse_gen->pulse_out(pulse);

	//trace file
	pTraceFile = sc_create_vcd_trace_file("counter_sim");

	sc_trace(pTraceFile, clk, "clk");
	sc_trace(pTraceFile, reset, "reset");
	sc_trace(pTraceFile, enable, "enable");
	sc_trace(pTraceFile, write, "write");
	sc_trace(pTraceFile, n, "n");
	sc_trace(pTraceFile, nx, "nx");
	sc_trace(pTraceFile, pulse, "pulse");

	sc_set_time_resolution(1, SC_PS);

	sc_start(1, SC_PS);// run indefinitely

	clk.write(1000000000);
    n.write(10);
	sc_start(1.5, SC_NS);

	reset.write(false);
	sc_start(1, SC_NS);
	reset.write(true);
	sc_start(1, SC_NS);
    
	enable.write(true);
	sc_start(1, SC_NS);

    nx.write(3);
    sc_start(1, SC_NS);
    write.write(true);
    sc_start(1, SC_NS);
    write.write(false);
    sc_start(10, SC_NS);

    nx.write(8);
    sc_start(1, SC_NS);
    write.write(true);
    sc_start(1, SC_NS);
    write.write(false);
    sc_start(10, SC_NS);

    nx.write(10);
	sc_start(1, SC_NS);
    write.write(true);
	sc_start(1, SC_NS);
    write.write(false);
	sc_start(10, SC_NS);

    nx.write(9);
	sc_start(1, SC_NS);
    write.write(true);
	sc_start(1, SC_NS);
    write.write(false);
	sc_start(10, SC_NS);

    nx.write(2);
	sc_start(1, SC_NS);
    write.write(true);
	sc_start(1, SC_NS);
    write.write(false);
	sc_start(10, SC_NS);

    nx.write(6);
	sc_start(1, SC_NS);
    write.write(true);
	sc_start(1, SC_NS);
    write.write(false);
	sc_start(10, SC_NS);

    clk.write(0);
	sc_start(50, SC_NS);

    clk.write(1000000000);
	sc_start(1, SC_NS);
	sc_start(1000, SC_NS);

	sc_close_vcd_trace_file(pTraceFile);
	return 0;
}// end sc_main()
