#include "MASTER.h"
#include "CoreFunction.h"

MASTER::MASTER( sc_module_name __name ) : sc_module( __name ),
                                          BusMasterBase<64, 1>(),
                                          resetPort( "resetPort" ),
                                          freqPort( "freqPort" )
{
    // BusMasterBase setting for 64bit bus socket
    setMasterResetPort64( &resetPort );
    setMasterFreqPort64( &freqPort );
    setInitiatorSocket64( "isx" );
    isx = iSocket64[0];

    // instantiation for Core function
    mpCore = new CoreFunction( mBusMasterIf64[0] );
    sc_assert(mpCore != NULL);
    mBusMasterIf64[0]->setFuncModulePtr( mpCore);
    mBusMasterIf64[0]->setBusProtocol( BUS_AXI );
    mBusMasterIf64[0]->setTransNmbLmt( 10 );

    SC_THREAD( process );

}

MASTER::~MASTER( void )
{
    delete mpCore;
}

void MASTER::process( void )
{
  while(1){
  
    mpCore->transportProcess();
  
  }
}