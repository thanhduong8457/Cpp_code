#include "design.h"
void first_counter::incr_count()
{
    // At every rising edge of clock we check if reset is active
    // If active, we load the counter output with 4'b0000
    if (reset.read() == 1)
    {
        count = 0;
        counter_out.write(count);
        // If enable is active, then we increment the counter
    }
    else if (enable.read() == 1)
    {
        count = count + 1;
        counter_out.write(count);
        cout << "@" << sc_time_stamp() << " :: Incremented Counter "
            << counter_out.read() << endl;
    }
} // End of function incr_count