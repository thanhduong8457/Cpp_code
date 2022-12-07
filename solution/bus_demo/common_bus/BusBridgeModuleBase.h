/*
* Copyright(c) 2012 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*/

#ifndef __BUSBRIDGEMODULEBASE_H__
#define __BUSBRIDGEMODULEBASE_H__

#include "systemc.h"
#include "BusBridgeCoreBase.h"

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH> class BusBridgeCoreBase;

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
class BusBridgeModuleBase : public sc_module
{
 public:
  sc_in<bool>            resetPort;  //reset port

  SC_HAS_PROCESS( BusBridgeModuleBase );

  /// <summary>constructor</summary>
  /// <param name="moduleName">module name</param>
  BusBridgeModuleBase( sc_module_name moduleName ) :
    sc_module( moduleName ),
    resetPort("resetPort"),
    mpCore( (BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH> *)0 )
  {
    SC_METHOD( resetProcess );
    sensitive << resetPort.neg();
    dont_initialize();

    SC_METHOD( deleteTransProcess );
    sensitive << meDeleteTransEvent;
    dont_initialize();
  }

  /// <summary>destructor</summary>
  ~BusBridgeModuleBase(void){}

  /// <summary>reset process</summary>
  void resetProcess( void )
  {
    mpCore->reset();
  }

  /// <summary>transaction deleting process</summary>
  void deleteTransProcess( void )
  {
    mpCore->deleteTrans();
  }

  /// <summary>transaction deleting function</summary>
  void deleteTrans( void )
  {
    meDeleteTransEvent.notify();
  }

  /// <summary>set bridge latency for forward path</summary>
  /// <param name="cycle">latency for forward path</param>
  void setBridgeFwLatency( uint64 latency )
  {
    mpCore->setBridgeFwLatency( latency );
  }

  /// <summary>set bridge latency for backward path</summary>
  /// <param name="cycle">latency for backward path</param>
  void setBridgeBwLatency( uint64 latency )
  {
    mpCore->setBridgeBwLatency( latency );
  }

  /// <summary>set TLM transaction type</summary>
  /// <param name="simMode">simulation mode (AT or LT)</param>
  void setTlmTransType( SIM_MODE_T simMode )
  {
    mpCore->setTlmTransType( simMode );
  }

  BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH> *mpCore; //bridge core function pointer
  sc_event meDeleteTransEvent; //transaction deleting event

};


#endif //__BUSBRIDGEMODULEBASE_H__
