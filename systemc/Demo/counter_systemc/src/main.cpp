#include "systemc.h"
#include "counter_test.h"

counter_test *top = NULL;

int sc_main(int argc, char* argv[])
{
	top = new counter_test("top");

	// Start simulation
	sc_start();// run indefinitely

	return 0;
}// end sc_main()
