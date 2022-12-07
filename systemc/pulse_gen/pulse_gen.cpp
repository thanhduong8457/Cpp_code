#include "PULSE_GEN.h"

PULSE_GEN::PULSE_GEN(sc_module_name name):sc_module(name)
    , CLK("CLK")
    , ENABLE("ENABLE")
    , WR("WR")
    , PRESET("PRESET")
    , PULSE_X_OUT("PULSE_X_OUT")
    , DIR_X("DIR_X")
{
    // Initialize variables

    counter = 0;
    a = 0;
    for (int i = 0; i < 10; i++) buffer_x[i] = 0;
    inN = 0;
    inNx = 0;
    acc_x = 0;
    temp_flag = 0;
    clk1 = 0;
    pin_x_out = 0;
    dir_x_out = 0;
    enable_handler = 0;
    preset_handler = 0;

    PULSE_X_OUT.initialize(false);
    DIR_X.initialize(false);

    SC_METHOD(handle_reset_method);
    dont_initialize();
    sensitive << PRESET.pos();

    SC_METHOD(write_method);
    dont_initialize();
    sensitive << WR.pos();

    SC_CTHREAD(main_process_thread, CLK);
    dont_initialize();
}

void PULSE_GEN::main_process_thread(void)
{
    count();
    main_pulse();
}

void PULSE_GEN::count(void)
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
        DIR_X.write(dir_x_out);
    }
    else
    {
        PULSE_X_OUT.write(0);
        DIR_X.write(0);
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

    if (acc_x >= inN)
    {
        acc_x -= inN;
        pin_x_out = 1;
    }
    else pin_x_out = 0;
}

void PULSE_GEN::handle_reset_method()
{
    counter = 0;
    temp_flag = 0;

    inNx = 0;
    acc_x = 0;

    clk1 = false;
    pin_x_out = false;
    dir_x_out = false;

    for (int i = 0; i < 10; i++) buffer_x[i] = 0;
}

void PULSE_GEN::write_method()
{
    a++;
    if (a > 10) a = 10;
    buffer_x[a - 1] = Nx.read();
}

void PULSE_GEN::catch_value()
{
    inN = N.read();
    inNx = buffer_x[0];

    if (inNx > 10) inNx = 10;

    for (int i = 0; i < a; i++) buffer_x[i] = buffer_x[i + 1];

    if (--a < 0) a = 0;

    buffer_x[a] = 0;
}

void PULSE_GEN::init()
{
    catch_value();
    acc_x = inNx;
}

PULSE_GEN::~PULSE_GEN(void)
{

}
