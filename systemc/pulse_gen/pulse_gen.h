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
    sc_in<bool> ENABLE;
    sc_in<bool> PRESET;
    sc_in<bool> WR;

    sc_in< sc_dt::uint64 > N;
    sc_in< sc_dt::uint64 > Nx;
    sc_in< sc_dt::uint64 > Ny;
    sc_in< sc_dt::uint64 > Nz;

    sc_out<bool> PULSE_X_OUT;
    sc_out<bool> PULSE_Y_OUT;
    sc_out<bool> PULSE_Z_OUT;

    sc_out<bool> DIR_X;
    sc_out<bool> DIR_Y;
    sc_out<bool> DIR_Z;

    sc_out<bool> FLAG;
    sc_out<bool> CLK1;

    sc_event fulled;
    sc_event flag;

    void init();

    void count();
    void main_pulse();
    void preset_module();
    void write_value();
    void catch_value();

private:
    unsigned int counter;

    int a = 0;
    unsigned int buffer_x[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int buffer_y[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    unsigned int buffer_z[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    unsigned int inN = 0;

    unsigned int inNx = 0;
    unsigned int inNy = 0;
    unsigned int inNz = 0;

    unsigned int acc_x = 0;
    unsigned int acc_y = 0;
    unsigned int acc_z = 0;

    unsigned int temp_flag = 0;

    bool clk1 = false;

    bool pin_x_out = false;
    bool dir_x_out = false;

    bool pin_y_out = false;
    bool dir_y_out = false;

    bool pin_z_out = false;
    bool dir_z_out = false;

    bool enable_handler = false;
    bool preset_handler = false;
};

#endif  //_PULSE_GEN_H
