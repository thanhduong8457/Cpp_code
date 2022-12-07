#include "SLAVE.h"

SLAVE::SLAVE( sc_module_name name, 
                unsigned int rILatency,
                unsigned int wILatency,
                unsigned int rFDLatency,
                unsigned int wFDLatency,
                unsigned int rNDLatency,
                unsigned int wNDLatency ):
    sc_module( name ),
    BusSlaveBase<64,1>()
{
    setSlaveResetPort64( &resetPort );
    setSlaveFreqPort64( &freqPortt);
    setTargetSocket64( "tsx" );

    tsx = tSocket64[0];

    mFunc = new MemoryFunction();
    mBusSlaveIf64[0]->setFuncModulePtr( mFunc );
    mBusSlaveIf64[0]->setReadInitLatency( rILatency );
    mBusSlaveIf64[0]->setWriteInitLatency( wILatency );
    mBusSlaveIf64[0]->setReadFirstDataLatency( rFDLatency );
    mBusSlaveIf64[0]->setWriteFirstDataLatency( wFDLatency );
    mBusSlaveIf64[0]->setReadNextDataLatency( rNDLatency );
    mBusSlaveIf64[0]->setWriteNextDataLatency( wNDLatency );
    mBusSlaveIf64[0]->setBusProtocol( BUS_APB );
    mBusSlaveIf64[0]->setTransNmbLmt( 1 );
}

SLAVE::~SLAVE(void)
{
    delete mFunc;
}
