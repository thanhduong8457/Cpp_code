#include "counter_test.h"

void counter_test::simulation()
{
	enable.write(1);
	pulse.write(0);

	for (int i = 0; i < 300; i++)
	{
		clk.write(1);
		wait(500, SC_NS);
		clk.write(0);
		wait(500, SC_NS);

		if ((i > 20 && i <= 50)||(i > 80 && i <= 120) || (i > 150 && i <= 200) || (i > 230 && i <= 250))
			pulse.write(1);
		else 
			pulse.write(0);


		if (i > 70 && i <= 130)
			enable.write(0);
		else enable.write(1);
	};

	enable.write(0);

	sc_stop();
}