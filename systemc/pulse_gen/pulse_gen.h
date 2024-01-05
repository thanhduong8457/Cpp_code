#include "systemc.h"

#if !defined(_PULSE_GEN_H)
#define _PULSE_GEN_H

class PULSE_GEN : public sc_module
{
public:
    SC_HAS_PROCESS(PULSE_GEN);
    PULSE_GEN(sc_module_name name);
    ~PULSE_GEN(void);

    sc_clock CLK;
    sc_in< bool > ENABLE;
    sc_in< bool > PRESET;
    sc_in< bool > WR;
    sc_in< sc_dt::uint64 > N;
    sc_in< sc_dt::uint64 > Nx;

    sc_out<bool> PULSE_X_OUT;
    sc_out<bool> DIR_X;
    sc_out<bool> FLAG;
    sc_out<bool> CLK1;

    sc_event fulled;
    sc_event flag;

    void init(void);

    void count(void);
    void main_pulse(void);
    void main_process_thread(void);

    void handle_reset_method(void);
    void write_method(void);
    void catch_value(void);

private:
    unsigned int counter;
    unsigned int a;
    unsigned int buffer_x[10];
    unsigned int inN;
    unsigned int inNx;
    unsigned int acc_x;
    unsigned int temp_flag;
    bool clk1;
    bool pin_x_out;
    bool dir_x_out;
    bool enable_handler;
    bool preset_handler;
};

#endif  //_PULSE_GEN_H
