#include "BusMasterIf.h"
#include "BusMasterFuncIf.h"

class CoreFunction : public BusMasterFuncIf
{
    public:
        CoreFunction( BusMasterIf<64>* );
        ~CoreFunction(void);

        //For slave - virtual functions in BusSlaveFuncIf
        void read(unsigned int offsetAddress, TlmBasicPayload &trans, BusTime_t *t, bool debug);
        void write(unsigned int offsetAddress, TlmBasicPayload &trans, BusTime_t *t, bool debug);

        void transportProcess( void );
        void notifyTransSend( TlmTransactionType &trans, BusTime_t &t );
        void notifyTransComp( TlmTransactionType &trans, BusTime_t &t );
    private:
        BusMasterIf<64> *mBusIf1;
        unsigned char mData1[1024];
        TlmBasicPayload mPayload1;
        TlmG3mExtension mG3mExt1;
};
