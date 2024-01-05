#include "counter.h"

void counter::main_counter()
{
	if (enable) 
	{
		if (pulse) temp++;
		else
		{
			if (temp)
			{
				count = temp;
				temp = 0;
			}
		}
	}
	else
	{
		count = 0;
	}

}

void counter::show_data()
{
	cout << "@" << sc_time_stamp() << " ENABLE: " << enable << " PULSE: " << pulse << " COUNT: " << count << endl;
}