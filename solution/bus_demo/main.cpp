#include "MASTER.h"
#include "SLAVE.h"

#include "main.h"

#include <stdlib.h>  // for strtol, strtoul, strtod
#include <string.h>  // for strcpy, strtok, strcmp,
#include <stdio.h>   // for sprintf
#include <signal.h>
#include <string>
#include <time.h>

#include "systemc.h"

extern int           token_to_int(char*, const char*, const char*);
extern unsigned int  token_to_uint(char*, const char*, const char*);
extern unsigned long token_to_ulong(char*, const char*, const char*);
extern unsigned long long token_to_ulonglong(char*, const char*, const char*);
extern double        token_to_double(char*, const char*, const char*);
extern unsigned int  token_to_uint_default(char *, unsigned int, const char *, char *);
extern unsigned long long token_to_ulonglong_default(char*, unsigned long long, const char*, const char*);

#define FREQ_DEFAULT 10000000 // default value of frequency

int           scstopDoneFlag; // sc_stop done flag
sc_dt::uint64 glb_freq = FREQ_DEFAULT; // global simulation frequency
double        glb_resolution_period; // global resolution period
sc_time_unit  glb_resolution_unit; // global resolution unit

char          *glb_python_scr_file; // python script file 
sc_signal<sc_dt::uint64, sc_core::SC_UNCHECKED_WRITERS> *glb_pe_clk_sig; // global clock signal
ofstream      bus_monitor_ofs;         // ofstream for bus monitor

#define HEAP_RELEASE_STRING "E1.00.00" // SC-HEAP release version
static const std::string smid = HEAP_RELEASE_STRING " [" __DATE__"]";

using namespace CmErrMsg; // for WinMsg

static const std::string version();

static const std::string version()
{
    return (smid);
}

const std::string(*scheapVersion)() = version;

int sc_main(int argc, char **argv)
{
    MASTER *master;
    SLAVE *slave;
    APISS *apiss;

    const char  *config_file = 0;
    char         seps[] = " ";
    char         config_seps[] = " ,=();\t\n\r";
    std::string  time_resolution = "";

    glb_python_scr_file = 0;
    glb_pe_clk_sig = new sc_signal<sc_dt::uint64, sc_core::SC_UNCHECKED_WRITERS>("glb_pe_clk_sig");

    if (argc > 1) {
        for (int count = 1; count < argc; count++) {
            if (strcmp(argv[count], "-config") == 0) {
                config_file = argv[count + 1];
            }
            else if (strcmp(argv[count], "-py_scr") == 0) {
                glb_python_scr_file = argv[count + 1];
            }
            else if (strcmp(argv[count], "-sctime") == 0) {
                time_resolution = argv[count + 1];
            }
            else if (strcmp(argv[count], "-v") == 0) {
                cout << "SC-HEAP G4 model version is "
                    << version()
                    << endl;
            }
            else if (strcmp(argv[count], "-help") == 0) {
                cout << "Usage : " << argv[0]
                    << " -config config_file"
                    << " -py_scr python_script_file"
                    << " [-v]"
                    << " [-help]" << endl;
                exit(1);
            }
        }
    }

    if (!config_file) {
        cout << "Error: cannot open config file\n"
            << endl;
        exit(1);
    }

    scstopDoneFlag = 0;

    CmErrMsgInitSet(config_file); // for WinMsg

    ifstream configFile_(config_file);

    if (!configFile_) {
        // for WinMsg
        char error_message_buf[1024];
        sprintf(error_message_buf,
            "Error: cannot open config file %s\n",
            config_file);
        printErrorMessage("main", "main", error_message_buf);
        exit(1);
    }

    glb_resolution_period = 1;
    glb_resolution_unit = SC_PS;
    if (time_resolution == "") glb_resolution_unit = SC_PS;
    else if (time_resolution == "SC_FS") glb_resolution_unit = SC_FS;
    else if (time_resolution == "SC_PS") glb_resolution_unit = SC_PS;
    else if (time_resolution == "SC_NS") glb_resolution_unit = SC_NS;
    else if (time_resolution == "SC_US") glb_resolution_unit = SC_US;
    else if (time_resolution == "SC_MS") glb_resolution_unit = SC_MS;
    else if (time_resolution == "SC_SEC") glb_resolution_unit = SC_SEC;
    else glb_resolution_unit = SC_PS;
    sc_set_time_resolution(glb_resolution_period, glb_resolution_unit);

    master = new MASTER("master");
    slave = new SLAVE("slave", 0, 0, 0, 0, 0, 0);
    apiss = new APISS("P_BUS", config_file);

    master->isx(apiss->ts);
    apiss->is(slave->tsx);

    return 0;
}