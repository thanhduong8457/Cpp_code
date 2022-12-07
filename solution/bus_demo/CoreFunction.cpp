#include "CoreFunction.h"

CoreFunction::CoreFunction( BusMasterIf<64> *busIf1 ): mBusIf1( busIf1 )
{
  //for( int i; i<(int)1024; i++ ) mData1[i] = 0;
  //mPayload1.set_extension( &mG3mExt1 );
  //mPayload2.set_extension( &mG3mExt2 );
  
}

CoreFunction::~CoreFunction( void ){}

void CoreFunction::transportProcess( void )
{
  //mPayload1.set_address(0x10000001); // set payload
 
  //mBusIf1->write( mPayload1 );         //write from socket1

  //mPayload2.set_address(0xFFFF0100); // set payload

  //mBufIf2->read( mPayload2);          //read from socket2

}

void CoreFunction::notifyTransSend(TlmTransactionType &trans, BusTime_t &t )
{

}

void CoreFunction::notifyTransComp(TlmTransactionType &trans, BusTime_t &t)
{

}

void CoreFunction::read(unsigned int offsetAddress, TlmBasicPayload &trans, BusTime_t *t, bool debug)
{

}

void CoreFunction::write(unsigned int offsetAddress, TlmBasicPayload &trans, BusTime_t *t, bool debug)
{

}
