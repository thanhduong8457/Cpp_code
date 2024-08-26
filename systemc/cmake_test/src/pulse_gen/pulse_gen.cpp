#include "pulse_gen.h"

pulse_gen::pulse_gen(sc_module_name name):sc_module(name)
    , clk("clk")
    , enable("enable")
    , write("write")
    , reset("reset")
    , pulse_out("pulse_out")
{
    // Initialize variables

    while (buffer_x.size() != 0) {
        buffer_x.pop_back();
    }

    inN = 0;
    inNx = 0;
    acc_x = 0;
    pin_x_out = 0;

    pulse_out.initialize(false);

    SC_METHOD(handle_reset_method);
    dont_initialize();
    sensitive << reset;

    SC_METHOD(write_method);
    dont_initialize();
    sensitive << write;

    SC_METHOD(get_value_method);
    dont_initialize();
    sensitive << catchValueEvent;

    SC_METHOD(monitorClockMethod);
	dont_initialize();
	sensitive << clk;	

    SC_THREAD(main_process_thread);
    main_process_thread_handle = sc_get_current_process_handle();
}

/// @brief 
/// @param  
pulse_gen::~pulse_gen(void) {
    //
}

/// @brief 
/// @param  
void pulse_gen::main_process_thread(void) {
    unsigned temp_a = 0;

    while (true) {
        isNotInOperation = true;
        wait(start_operation_event);

        if ((false == isZeroClock) && (false == isReset) && (true == enable.read())) {
            isNotInOperation = false;
            catch_value();
            acc_x = inNx;

            for (unsigned int i = 0; i < inN; i++) {
                acc_x += inNx;

                if (acc_x >= inN) {
                    acc_x -= inN;
                    pulse_out.write(true);
                }
                else {
                    pulse_out.write(false);
                }
                wait(nextPosEdge()/2, SC_NS);
                pulse_out.write(false);

                start_operation_event.notify(nextPosEdge()/2, SC_NS);
                wait(start_operation_event);
            }

            if (0 != buffer_x.size()) {
                start_operation_event.notify(nextPosEdge(), SC_NS);
            } 
        }
    }
}

/// @brief 
/// @param  
void pulse_gen::handle_reset_method(void) {
    if (false == reset.read()) {
        isReset = true;
        std::cout << "reset assert" << std::endl;
    } 
    else {
        initialize();
        isReset = false;
        cancelAllEvent();
        std::cout << "reset deassert" << std::endl;
    }
}

void pulse_gen::cancelAllEvent(void) {

}

/// @brief 
/// @param  
void pulse_gen::monitorClockMethod(void) {
	unsigned int period = clk.read();
	
	if (0x0 == period) {
		isZeroClock = true;
		clockPeriod = 0x0;
        main_process_thread_handle.suspend();
	} 
	else {
		isZeroClock = false;
		clockPeriod = period;
		cout << " clock is " << clockPeriod << endl;
		timeStartOperation = sc_time_stamp().to_double();
		cout << " start time of module is " << timeStartOperation << endl;

        main_process_thread_handle.resume();
	}
}

/// @brief 
/// @param  
/// @return 
double pulse_gen::nextPosEdge(void) {
	double return_value = 0x0;
	double current_time = sc_time_stamp().to_double();
	double period = (double)1/clockPeriod * 10e+9;
	return_value = period - (unsigned)(current_time - timeStartOperation) % (unsigned)period;
	return return_value;
}

/// @brief 
/// @param  
void pulse_gen::write_method(void) {
    if (true == write.read()) {
        double next_pos_edge = nextPosEdge();
        catchValueEvent.notify(next_pos_edge, SC_NS);
        cout << "write_method trigger" << endl;
    }
}

/// @brief 
/// @param  
void pulse_gen::get_value_method(void) {
    sc_uint<64> temp_val = Nx.read();
    if ((false == isZeroClock) && (false == isReset) && (true == enable.read())) {
        buffer_x.push_back(temp_val); // check
        start_operation_event.notify(SC_ZERO_TIME);
        std::cout << "get value" << std::endl;
    }
}

/// @brief 
/// @param  
void pulse_gen::catch_value(void) {
    inN = N.read();

    if (0 != buffer_x.size()) {
        inNx = buffer_x[0];
        buffer_x.pop_front();
        if (inNx > inN) {
            inNx = inN;
        }
    }
}

/// @brief 
/// @param  
void pulse_gen::initialize(void) {
    inNx = 0;
    acc_x = 0;
    isNotInOperation = false;

    pin_x_out = false;

    while (buffer_x.size() != 0) {
        buffer_x.pop_back();
    }
}
