#include "systemc.h"

#if !defined(_PULSE_GEN_H)
#define _PULSE_GEN_H

class pulse_gen : public sc_module {
public:
    sc_in< sc_dt::uint64 > clk;
    sc_in< bool > enable;
    sc_in< bool > reset;
    sc_in< bool > write;
    sc_in< sc_dt::uint64 > N;
    sc_in< sc_dt::uint64 > Nx;

    sc_out<bool> pulse_out;

    SC_HAS_PROCESS(pulse_gen);
    pulse_gen(sc_module_name name);
    ~pulse_gen(void);

private:
    std::deque<unsigned int> buffer_x;
    unsigned int inN;
    unsigned int inNx;
    unsigned int acc_x;
    bool pin_x_out;

    bool isZeroClock;
    bool isReset;
    bool isNotInOperation;

    sc_uint<64> clockPeriod;
    double timeStartOperation;

    sc_event catchValueEvent;
    sc_event start_operation_event;

    sc_process_handle main_process_thread_handle ;

    void initialize(void);

    void main_process_thread(void);

    void handle_reset_method(void);
    void write_method(void);
    void catch_value(void);

    void monitorClockMethod(void);

    void cancelAllEvent(void);
    double nextPosEdge(void);
    void get_value_method(void);
};

#endif  //_PULSE_GEN_H
