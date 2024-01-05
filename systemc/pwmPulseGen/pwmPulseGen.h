#include "systemc.h"

#if !defined(_PWM_PULSE_GEN_H)
#define _PWM_PULSE_GEN_H

class pwmPulseGen : public sc_mudule {
public:
   SC_HAS_PROCESS(pwmPulseGen);
   pwmPulseGen(sc_module_name name);
   ~pwmPulseGen(void);

   sc_in<bool>          clk;
   sc_in<bool>          reset;
   sc_in<sc_dt::uint8>  mValue;
   sc_in<sc_dt::uint8>  nValue;
   sc_in<bool>          wr;
   sc_out<bool>         pwmOut;
   sc_out<bool>         flag;

   sc_event e1;

    void init(void);

    void count(void);
    void main_pulse(void);
    void main_process_thread(void);

    void handle_reset_method(void);
    void write_method(void);
    void catch_value(void);


private:
    unsigned int inNvalue;
    unsigned int inMvalue;

};
#endif  //_PWM_PULSE_GEN_H