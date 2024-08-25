#include "counter.h"

counter::counter(sc_module_name name) : sc_module(name) {
	initialize();

	SC_THREAD(updateOutputThread);
	sensitive << updateOuputEvent;

	SC_METHOD(MonitorPulseEdgeMethod);
	dont_initialize();
	sensitive << pulse;	

	SC_METHOD(monitorClockMethod);
	dont_initialize();
	sensitive << clk;	

	SC_METHOD(monitorResetMethod);
	dont_initialize();
	sensitive << reset;	

	SC_METHOD(handleResetMethod);
	dont_initialize();
	sensitive << handleResetEvent;
}

counter::~counter() {
	//
}

void counter::initialize() {
	// clockPeriod = 0x0;
	// isZeroClock = true;
	// isInReset = true;

	counterTemp = 0x0;
	timeStartOperation = 0x0;
}

void counter::show_data() {
	cout << "@" << sc_time_stamp() << " ENABLE: " << enable << " PULSE: " << pulse << " COUNT: " << count << endl;
}

void counter::monitorClockMethod() {
	unsigned int period = clk.read();
	
	if (0x0 == period) {
		isZeroClock = true;
		clockPeriod = 0x0;
	} 
	else {
		isZeroClock = false;
		clockPeriod = period;
		cout << " clock is " << clockPeriod << endl;
		timeStartOperation = sc_time_stamp().to_double();
		cout << " start time of module is " << timeStartOperation << endl;
	}
}

void counter::monitorResetMethod() {
	if (true == reset.read()) {
		isInReset = true;
		updateOuputEvent.cancel();
		handleResetEvent.notify(SC_ZERO_TIME);
		cout << " reset is triggered" << endl;
	}
	else {
		isInReset = false;
		cout << " reset is release" << endl;
	}
}

void counter::handleResetMethod() {
	initialize();
	updateOuputEvent.notify(SC_ZERO_TIME);
}

void counter::MonitorPulseEdgeMethod() {
	if (false == pulse.read()) {
		cout << "pulse down" << endl;
		if ((false == isZeroClock) && (false == isInReset) && (true == enable.read())) {
			counterTemp += 1;
			updateOuputEvent.notify(SC_ZERO_TIME);
		}
	}
}

void counter::updateOutputThread() {
	while (true) {
		wait();
		wait(nextPosEdge(), SC_PS);
		count.write(counterTemp);
		show_data();
	}
}

double counter::nextPosEdge() {
	double return_value = 0x0;
	double current_time = sc_time_stamp().to_double();
	double period = 1/clockPeriod * 10e+9;
	return_value = period - (unsigned)(current_time - timeStartOperation) % (unsigned)period;
	return return_value;
}
