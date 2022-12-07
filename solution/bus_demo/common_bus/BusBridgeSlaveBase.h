/*
* Copyright(c) 2012 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*/

#ifndef __BUSBRIDGESLAVEBASE_H__
#define __BUSBRIDGESLAVEBASE_H__

#include "BusBridgeSlaveIf.h"
#include "BusSlaveIf.h"
#include "CmErrMsg.h"  // for error message
#include <stdarg.h>

template<unsigned int BUSWIDTH, unsigned int S_NUM>
class BusBridgeSlaveBase
{
 private:
  sc_in<bool>               *mSlaveResetPort[S_NUM]; // pointer of slave reset port
  sc_in<uint64>             *mSlaveFreqPort[S_NUM]; // pointer of slave frequency port
 public:
  TlmTargetSocket<BUSWIDTH> *tSocket[S_NUM]; // target socket pointer
  BusBridgeSlaveIf<BUSWIDTH>      *mBusSlaveIf[S_NUM]; // bus slave I/F pointer

  /// <summary>constructor</summary>
  BusBridgeSlaveBase(void){}

  /// <summary>destructor</summary>
  ~BusBridgeSlaveBase(void){
    for( unsigned char i=0; i<S_NUM; i++ ){
      delete tSocket[i];
      delete mBusSlaveIf[i];
    }
  }

  /// <summary>setting function for slave reset port</summary>
  /// <param name="resetPort">reset port pointer</param>
  void setSlaveResetPort( sc_in<bool> *resetPort, ... )
  {
    mSlaveResetPort[0] = resetPort;
    va_list list;
    va_start( list, resetPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mSlaveResetPort[i] = va_arg( list, sc_in<bool> * );
    }
  }

  /// <summary>setting function for slave frequency port</summary>
  /// <param name="freqPort">frequency port pointer</param>
  void setSlaveFreqPort( sc_in<uint64> *freqPort, ... )
  {
    mSlaveFreqPort[0] = freqPort;
    va_list list;
    va_start( list, freqPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mSlaveFreqPort[i] = va_arg( list, sc_in<uint64> * );
    }
  }

  /// <summary>setting function for target socket</summary>
  /// <param name="socketName">target socket name</param>
  void setTargetSocket( char *socketName, ... )
  {
    char name[1024];

    sprintf( name, socketName );
    tSocket[0] = new TlmTargetSocket<BUSWIDTH>( name );
    strcat( name, "_if" );
    mBusSlaveIf[0] = new BusBridgeSlaveIf<BUSWIDTH>( name,
					       *mSlaveResetPort[0],
					       *mSlaveFreqPort[0],
					       *tSocket[0] );
    (*tSocket[0])(*mBusSlaveIf[0]);  // bind between socket & I/F
    tSocket[0]->setFuncPtrBaseAddressSize((BusSlaveIf<BUSWIDTH> *)mBusSlaveIf[0], 
					  &BusSlaveIf<BUSWIDTH>::setfunc);
    va_list list;
    va_start( list, socketName );
    for( unsigned char i=1; i<S_NUM; i++ ){
      sprintf( name, va_arg( list, char* ) );
      tSocket[i] = new TlmTargetSocket<BUSWIDTH>( name );
      strcat( name, "_if" );
      mBusSlaveIf[i] = new BusBridgeSlaveIf<BUSWIDTH>( name,
						 *mSlaveResetPort[i],
						 *mSlaveFreqPort[i],
						 *tSocket[i] );
      (*tSocket[i])(*mBusSlaveIf[i]);  // bind between socket & I/F
      tSocket[i]->setFuncPtrBaseAddressSize((BusSlaveIf<BUSWIDTH> *)mBusSlaveIf[i], 
					    &BusSlaveIf<BUSWIDTH>::setfunc);
    }
    va_end( list );
  }

};


#endif //__BUSBRIDGESLAVEBASE_H__
