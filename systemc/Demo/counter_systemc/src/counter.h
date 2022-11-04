#include "systemc.h"

SC_MODULE(counter)
{
	sc_in<bool>	clk;
	sc_in<bool> enable;
	sc_in<bool> pulse;
	sc_out< sc_uint<16> > count;

	sc_event e1;

	uint16_t temp;
	void main_counter();
	void wait_ena();
	void show_data();

	SC_CTOR(counter)
	{
		temp = 0;
		SC_METHOD(main_counter);
		sensitive << clk.pos();

		SC_METHOD(show_data);
		sensitive << clk.pos();	
	}
};