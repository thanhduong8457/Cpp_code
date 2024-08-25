#include <iostream>
#include "counter.h"

int sc_main(int argc, char* argv[]) {
	sc_signal<sc_uint<64> > clk;
	sc_signal<bool> enable;
	sc_signal<bool> reset;
	sc_signal<bool> pulse;
	sc_signal< sc_uint<16> > count;

	sc_trace_file *pTraceFile;

	counter *my_counter = NULL;

	my_counter = new counter("my_counter");

	//defining and identifying signals
	my_counter->clk(clk);
	my_counter->reset(reset);
	my_counter->enable(enable);
	my_counter->pulse(pulse);
	my_counter->count(count);

	//trace file
	pTraceFile = sc_create_vcd_trace_file("counter_sim");

	sc_trace(pTraceFile, clk, "clk");
	sc_trace(pTraceFile, reset, "reset");
	sc_trace(pTraceFile, enable, "enable");
	sc_trace(pTraceFile, pulse, "PULSE");
	sc_trace(pTraceFile, count, "count");

	sc_set_time_resolution(1, SC_PS);

	// Start simulation
	sc_start(1, SC_NS);// run indefinitely

	clk.write(10000000);
	reset.write(false);
	enable.write(true);

	sc_start(1, SC_NS);// run indefinitely

	pulse.write(true);
	sc_start(0.2, SC_NS);// run indefinitely
	pulse.write(false);
	sc_start(1, SC_NS);// run indefinitely

	pulse.write(true);
	sc_start(3, SC_NS);// run indefinitely
	pulse.write(false);
	sc_start(0.1, SC_NS);// run indefinitely

	reset.write(true);
	sc_start(1, SC_NS);// run indefinitely
	reset.write(false);
	sc_start(1, SC_NS);// run indefinitely

	pulse.write(true);
	sc_start(0.2, SC_NS);// run indefinitely
	pulse.write(false);
	sc_start(1, SC_NS);// run indefinitely

	pulse.write(true);
	sc_start(1, SC_NS);// run indefinitely
	pulse.write(false);
	sc_start(3, SC_NS);// run indefinitely

	enable.write(false);

	pulse.write(true);
	sc_start(1, SC_NS);// run indefinitely
	pulse.write(false);
	sc_start(3, SC_NS);// run indefinitely

	pulse.write(true);
	sc_start(1, SC_NS);// run indefinitely
	pulse.write(false);
	sc_start(3, SC_NS);// run indefinitely

	enable.write(true);

	pulse.write(true);
	sc_start(1, SC_NS);// run indefinitely
	pulse.write(false);
	sc_start(3, SC_NS);// run indefinitely

	pulse.write(true);
	sc_start(1, SC_NS);// run indefinitely
	pulse.write(false);
	sc_start(3, SC_NS);// run indefinitely


	sc_close_vcd_trace_file(pTraceFile);
	return 0;
}// end sc_main()
