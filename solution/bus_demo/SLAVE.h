#include "BusSlaveBase.h"
#include "MemoryFunction.h"

class SLAVE : public sc_module, public BusSlaveBase<64,1>
{
    public:
        SLAVE(sc_module_name name,
            unsigned int         rILatency,
            unsigned int         wILatency,
            unsigned int         rFDLatency,
            unsigned int         wFDLatency,
            unsigned int         rNDLatency,
            unsigned int         wNDLatency);

        SLAVE( sc_module_name name, 
                    unsigned int         rLatency,
                    unsigned int         wLatency );

        ~SLAVE(void);

        TlmTargetSocket<64> tsx;

        sc_in<bool>    resetPort;
        sc_in<uint64> freqPortt;
        MemoryFunction *mFunc;

};
