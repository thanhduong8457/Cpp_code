#include "PULSE_GEN.h"

PULSE_GEN::PULSE_GEN(sc_module_name name):sc_module(name)
    , CLK("CLK")
    , ENABLE("ENABLE")
    , WR("WR")
    , PRESET("PRESET")
    , PULSE_X_OUT("PULSE_X_OUT")
    , PULSE_Y_OUT("PULSE_Y_OUT")
    , PULSE_Z_OUT("PULSE_Z_OUT")
    , DIR_X("DIR_X")
    , DIR_Y("DIR_Y")
    , DIR_Z("DIR_Z")
{
    // Initialize variables 
    PULSE_X_OUT.initialize(false);
    PULSE_Y_OUT.initialize(false);
    PULSE_Z_OUT.initialize(false);
    DIR_X.initialize(false);
    DIR_Y.initialize(false);
    DIR_Z.initialize(false);

    SC_METHOD(preset_module);
    dont_initialize();
    sensitive << PRESET.pos();

    SC_METHOD(write_value);
    dont_initialize();
    sensitive << WR.pos();

    SC_CTHREAD(count, CLK);
    dont_initialize();

    SC_CTHREAD(main_pulse, CLK);
    dont_initialize();
}

void PULSE_GEN::count()
{
    enable_handler = ENABLE.read();

    if (PRESET.read()) preset_handler = false;
    else preset_handler = true;

    if (enable_handler&&preset_handler)
    {
        counter++;

        if (counter > 100)
        {
            counter = 0;
            fulled.notify(SC_ZERO_TIME);
        }

        if (counter <= 50) clk1 = 1;
        else clk1 = 0;

        if (counter == 50) CLK1.write(1);
        else if (counter == 100) CLK1.write(0);

        PULSE_X_OUT.write(~clk1&pin_x_out);
        PULSE_Y_OUT.write(~clk1&pin_y_out);
        PULSE_Z_OUT.write(~clk1&pin_z_out);

        DIR_X.write(dir_x_out);
        DIR_Y.write(dir_x_out);
        DIR_Z.write(dir_z_out);
    }
    else
    {
        PULSE_X_OUT.write(0);
        PULSE_Y_OUT.write(0);
        PULSE_Z_OUT.write(0);
        
        DIR_X.write(0);
        DIR_Y.write(0);
        DIR_Z.write(0);

        FLAG.write(0);
        CLK1.write(0);
    }
}

void PULSE_GEN::main_pulse()
{
    temp_flag++;

    if (temp_flag <= inN) FLAG.write(1);
    else FLAG.write(0);

    if (temp_flag == inN || temp_flag == 2 * inN) catch_value();

    if (temp_flag >= 2 * inN) temp_flag = 0;

    acc_x += inNx;
    acc_y += inNy;
    acc_z += inNz;

    if (acc_x >= inN)
    {
        acc_x -= inN;
        pin_x_out = 1;
    }
    else
    {
        pin_x_out = 0;
    }

    if (acc_y >= inN)
    {
        acc_y -= inN;
        pin_y_out = 1;
    }
    else
    {
        pin_y_out = 0;
    }

    if (acc_z >= inN)
    {
        acc_z -= inN;
        pin_z_out = 1;
    }
    else
    {
        pin_z_out = 0;
    }
}

void PULSE_GEN::preset_module()
{
    counter = 0;
    temp_flag = 0;

    inNx = 0;
    inNx = 0;
    inNx = 0;

    acc_x = 0;
    acc_x = 0;
    acc_x = 0;

    clk1 = false;

    pin_x_out = false;
    pin_y_out = false;
    pin_z_out = false;

    dir_x_out = false;
    dir_y_out = false;
    dir_z_out = false;

    for (int i = 0; i < 10; i++)
    {
        buffer_x[i] = 0;
        buffer_y[i] = 0;
        buffer_z[i] = 0;
    }
}

void PULSE_GEN::write_value()
{
    a++;
    if (a > 10)
    {
        a = 10;
    }
    else
    {
        buffer_x[a - 1] = Nx.read();
        buffer_y[a - 1] = Ny.read();
        buffer_z[a - 1] = Nz.read();
    }
}

void PULSE_GEN::catch_value()
{
    inN = N.read();

    inNx = buffer_x[0];
    if (inNx > 10) inNx = 10;

    inNy = buffer_y[0];
    if (inNy > 10) inNy = 10;

    inNz = buffer_z[0];
    if (inNz > 10) inNz = 10;

    for (int i = 0; i < a; i++)
    {
        buffer_x[i] = buffer_x[i + 1];
        buffer_y[i] = buffer_y[i + 1];
        buffer_z[i] = buffer_z[i + 1];
    }

    if (--a < 0)
    {
        a = 0;
    }

    buffer_x[a] = 0;
    buffer_y[a] = 0;
    buffer_z[a] = 0;
}

void PULSE_GEN::init()
{
    catch_value();

    acc_x = inNx;
    acc_y = inNy;
    acc_z = inNz;
}

PULSE_GEN::~PULSE_GEN(void)
{

}
