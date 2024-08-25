#include "systemc.h"

class counter : sc_module {
public:
	sc_in<sc_uint<64> >	clk;
	sc_in<bool> enable;
	sc_in<bool> reset;
	sc_in<bool> pulse;
	sc_out< sc_uint<16> > count;

	SC_HAS_PROCESS(counter);
	counter(sc_module_name name);
	~counter();
private:
	sc_event updateOuputEvent;
	sc_event handleResetEvent;

	double clockPeriod;

	bool isZeroClock;
	bool isInReset;

	double timeStartOperation;

	uint16_t counterTemp;

	void initialize();
	void main_counter();
	void wait_ena();
	void show_data();

	void monitorClockMethod();
	void monitorResetMethod();
	void handleResetMethod();
	void MonitorPulseEdgeMethod();
	void updateOutputThread();
	double nextPosEdge();
};