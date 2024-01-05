#include "pwmPulseGen.h"

pwmPulseGen::pwmPulseGen(sc_module_name name):sc_module(name)
    , clk("clk")
    , wr("wr")
    , reset("reset")
    , nValue("nValue")
    , mValue("mValue")
    , pwmOut("pwmOut")
    , flag("flag")
{
    // Initialize variables
    nValue = 0;
    mValue = 0;
    wr = false;
    reset = false;
    temp_flag = 0;
    pwmOut = false;
    flag = false;

    pwmOut.initialize(false);
    flag.initialize(false);

    SC_METHOD(handleResetMethod);
    dont_initialize();
    sensitive << reset;

    SC_METHOD(write_method);
    dont_initialize();
    sensitive << wr;

    SC_THREAD(mainProcessThread);
    dont_initialize();
    sensitive << clk.pos();

    SC_THREAD(flagTheard);
    dont_initialize();
    sensitive << clk.pos();
}

void pwmPulseGen::mainProcessThread(void) {
    while (true) {
        if (a < inNvalue) {
            pwmOut.write(1);
        }
        else if (a < inMvalue) {
            pwmOut.write(0);
        }
        else {
            count = 0;
        }
        wait();
    }
}

void pwmPulseGen::flagTheard(void) {
    

}

void pwmPulseGen::count(void) {

}

void pwmPulseGen::main_pulse() {

}

void pwmPulseGen::handleResetMethod() {
    inNvalue = 0;
    inMvalue = 0;

    clk1 = false;
    pin_x_out = false;
    dir_x_out = false;
}

void pwmPulseGen::write_method() {
    inNvalue = nValue.read();
    inMvalue = mValue.read();
    writeEent.notify();
}

void pwmPulseGen::init() {

}

pwmPulseGen::~pwmPulseGen(void) {

}
