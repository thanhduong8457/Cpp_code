#include "systemc.h"
#include "pulse_gen.h"

SC_MODULE(dummy)
{
    sc_signal<bool> dummy_enable;
    sc_signal<bool> dummy_preset;
    sc_signal<bool> dummy_wr;

    sc_signal< sc_dt::uint64 > dummy_N;
    sc_signal< sc_dt::uint64 > dummy_Nx;
    sc_signal< sc_dt::uint64 > dummy_Ny;
    sc_signal< sc_dt::uint64 > dummy_Nz;

    sc_signal<bool> dummy_pulse_x_in;
    sc_signal<bool> dummy_pulse_y_in;
    sc_signal<bool> dummy_pulse_z_in;

    sc_signal<bool> dummy_dir_x_in;
    sc_signal<bool> dummy_dir_y_in;
    sc_signal<bool> dummy_dir_z_in;

    sc_signal<bool> dummy_flag_in;
    sc_signal<bool> dummy_clk1_in;

    //pointting to tracefile in contructor
    sc_trace_file *pTraceFile;

    PULSE_GEN PULSE_GEN_1;

    // calling simulation
    void simulation();

    SC_CTOR(dummy) :
        PULSE_GEN_1("PULSE_GEN_1"),
        dummy_enable("dummy_enable"),
        dummy_preset("dummy_preset"),
        dummy_wr("dummy_wr"),
        dummy_N("dummy_N"),
        dummy_Nx("dummy_Nx"),
        dummy_Ny("dummy_Ny"),
        dummy_Nz("dummy_Nz"),
        dummy_pulse_x_in("dummy_pulse_x_in"),
        dummy_pulse_y_in("dummy_pulse_y_in"),
        dummy_pulse_z_in("dummy_pulse_z_in"),
        dummy_dir_x_in("dummy_dir_x_in"),
        dummy_dir_y_in("dummy_dir_y_in"),
        dummy_dir_z_in("dummy_dir_z_in"),
        dummy_flag_in("dummy_flag_in"),
        dummy_clk1_in("dummy_clk1_in")
    {
        //defining and identifying signals
        PULSE_GEN_1.ENABLE(dummy_enable);
        PULSE_GEN_1.PRESET(dummy_preset);
        PULSE_GEN_1.WR(dummy_wr);
        PULSE_GEN_1.N(dummy_N);

        PULSE_GEN_1.Nx(dummy_Nx);
        PULSE_GEN_1.Ny(dummy_Ny);
        PULSE_GEN_1.Nz(dummy_Nz);

        PULSE_GEN_1.PULSE_X_OUT(dummy_pulse_x_in);
        PULSE_GEN_1.PULSE_Y_OUT(dummy_pulse_y_in);
        PULSE_GEN_1.PULSE_Z_OUT(dummy_pulse_z_in);

        PULSE_GEN_1.DIR_X(dummy_dir_x_in);
        PULSE_GEN_1.DIR_Y(dummy_dir_y_in);
        PULSE_GEN_1.DIR_Z(dummy_dir_z_in);

        PULSE_GEN_1.FLAG(dummy_flag_in);
        PULSE_GEN_1.CLK1(dummy_clk1_in);

        //trace file
        pTraceFile = sc_create_vcd_trace_file("PULSE_SIMULATION");

        sc_trace(pTraceFile, dummy_enable, "enable");
        sc_trace(pTraceFile, dummy_preset, "preset");
        sc_trace(pTraceFile, dummy_wr, "wr");
        sc_trace(pTraceFile, dummy_N, "N");
        sc_trace(pTraceFile, dummy_Nx, "Nx");
        sc_trace(pTraceFile, dummy_Ny, "Ny");
        sc_trace(pTraceFile, dummy_Nz, "Nz");
        sc_trace(pTraceFile, dummy_pulse_x_in, "pulse_x");
        sc_trace(pTraceFile, dummy_pulse_y_in, "pulse_y");
        sc_trace(pTraceFile, dummy_pulse_z_in, "pulse_z");
        sc_trace(pTraceFile, dummy_dir_x_in, "dir_x");
        sc_trace(pTraceFile, dummy_dir_y_in, "dir_y");
        sc_trace(pTraceFile, dummy_dir_z_in, "dir_z");
        sc_trace(pTraceFile, dummy_flag_in, "flag");
        sc_trace(pTraceFile, dummy_clk1_in, "clk1");

        SC_THREAD(simulation);
    }

    ~dummy()
    {
        sc_close_vcd_trace_file(pTraceFile);
    }
};