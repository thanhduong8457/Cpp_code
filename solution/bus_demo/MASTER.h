#include "BusMasterBase.h"
#include "BusSlaveBase.h"

class CoreFunction;
class MASTER : public sc_module, public BusMasterBase<64,1>
{
public:
    sc_in<bool>    resetPort;
    sc_in<uint64> freqPort;

    //Socket declaration
    TlmInitiatorSocket<64>          isx;

    CoreFunction                    *mpCore;

    SC_HAS_PROCESS(MASTER);
    MASTER(sc_module_name __name = "MASTER");
    ~MASTER(void);

    void process(void);
};