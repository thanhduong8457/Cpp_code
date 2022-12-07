#include "systemc.h"

#include "APISS.h"

//---- global variable
extern int           scstopDoneFlag;
extern sc_dt::uint64 glb_freq;
extern double        glb_resolution_period;
extern sc_time_unit  glb_resolution_unit;
extern int           glb_cycle_number;
extern char          *glb_python_scr_file;
extern sc_signal<sc_dt::uint64, sc_core::SC_UNCHECKED_WRITERS> *glb_pe_clk_sig;

extern void SetPyExtCmd(void);