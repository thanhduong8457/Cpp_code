#include "systemc.h"
#include "counter.h"

SC_MODULE(counter_test)
{
	sc_signal<bool> clk;
	sc_signal<bool> enable;
	sc_signal<bool> pulse;
	sc_signal< sc_uint<16> > count;

	//pointting to tracefile in contructor
	sc_trace_file *pTraceFile;

	counter counter1;

	// calling simulation
	void simulation();

	SC_CTOR(counter_test):
		counter1("counter1"),
		clk("clk"),
		enable("enable"),
		pulse("pulse"),
		count("count")
	{
		//defining and identifying signals
		counter1.clk(clk);
		counter1.enable(enable);
		counter1.pulse(pulse);
		counter1.count(count);

		//trace file
		pTraceFile = sc_create_vcd_trace_file("counter_sim");

		sc_trace(pTraceFile, clk, "clk");
		sc_trace(pTraceFile, enable, "enable");
		sc_trace(pTraceFile, pulse, "PULSE");
		sc_trace(pTraceFile, count, "count");

		SC_THREAD(simulation);
	}

	~counter_test()
	{
		sc_close_vcd_trace_file(pTraceFile);
	}
};