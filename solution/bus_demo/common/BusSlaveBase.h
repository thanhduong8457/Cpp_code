/*
* Copyright(c) 2011 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*/

#ifndef __BUSSLAVEBASE_H__
#define __BUSSLAVEBASE_H__

#include "BusSlaveIf.h"
#include "CmErrMsg.h"  // for error message
#include <stdarg.h>

#define tSocketFlsh  tSocket128
#define tSocketVci   tSocket64
#define tSocketVci32 tSocket32
#define tSocketAxi   tSocket64
#define tSocketAhb   tSocket32
#define tSocketApb   tSocket32
#define tSocketVpi   tSocket32
#define tSocketGram  tSocket64

#define mBusSlaveIfFlsh  mBusSlaveIf128
#define mBusSlaveIfVci   mBusSlaveIf64
#define mBusSlaveIfVci32 mBusSlaveIf32
#define mBusSlaveIfAxi   mBusSlaveIf64
#define mBusSlaveIfAhb   mBusSlaveIf32
#define mBusSlaveIfApb   mBusSlaveIf32
#define mBusSlaveIfVpi   mBusSlaveIf32
#define mBusSlaveIfGram  mBusSlaveIf64

#define setSlaveResetPortFlsh  setSlaveResetPort128
#define setSlaveResetPortVci   setSlaveResetPort64
#define setSlaveResetPortVci32 setSlaveResetPort32
#define setSlaveResetPortAxi   setSlaveResetPort64
#define setSlaveResetPortAhb   setSlaveResetPort32
#define setSlaveResetPortApb   setSlaveResetPort32
#define setSlaveResetPortVpi   setSlaveResetPort32
#define setSlaveResetPortGram  setSlaveResetPort64

#define setSlaveFreqPortFlsh  setSlaveFreqPort128
#define setSlaveFreqPortVci   setSlaveFreqPort64
#define setSlaveFreqPortVci32 setSlaveFreqPort32
#define setSlaveFreqPortAxi   setSlaveFreqPort64
#define setSlaveFreqPortAhb   setSlaveFreqPort32
#define setSlaveFreqPortApb   setSlaveFreqPort32
#define setSlaveFreqPortVpi   setSlaveFreqPort32
#define setSlaveFreqPortGram  setSlaveFreqPort64

#define setTargetSocketFlsh  setTargetSocket128
#define setTargetSocketVci   setTargetSocket64
#define setTargetSocketVci32 setTargetSocket32
#define setTargetSocketAxi   setTargetSocket64
#define setTargetSocketAhb   setTargetSocket32
#define setTargetSocketApb   setTargetSocket32
#define setTargetSocketVpi   setTargetSocket32
#define setTargetSocketGram  setTargetSocket64

template<unsigned int BUSWIDTH, unsigned int S_NUM>
class BusSlaveBase
{
 private:
  sc_in<bool>               *mSlaveResetPort[S_NUM]; //reset port
  sc_in<uint64>             *mSlaveFreqPort[S_NUM]; //frequency port
 public:
  TlmTargetSocket<BUSWIDTH> *tSocket[S_NUM]; //initiator port
  BusSlaveIf<BUSWIDTH>      *mBusSlaveIf[S_NUM]; //bus slave I/F

  /// <summary>constructer</summary>
  BusSlaveBase(void){}

  /// <summary>destructer</summary>
  ~BusSlaveBase(void){
    for( unsigned char i=0; i<S_NUM; i++ ){
      delete tSocket[i];
      delete mBusSlaveIf[i];
    }
  }

  /// <summary>set reset port</summary>
  /// <param name="resetPort">resest port</param>
  void setSlaveResetPort( sc_in<bool> *resetPort, ... )
  {
    mSlaveResetPort[0] = resetPort;
    va_list list;
    va_start( list, resetPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mSlaveResetPort[i] = va_arg( list, sc_in<bool> * );
    }
  }

  /// <summary>set frequency port</summary>
  /// <param name="freqPort">frequency port</param>
  void setSlaveFreqPort( sc_in<uint64> *freqPort, ... )
  {
    mSlaveFreqPort[0] = freqPort;
    va_list list;
    va_start( list, freqPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mSlaveFreqPort[i] = va_arg( list, sc_in<uint64> * );
    }
  }

  /// <summary>set target socket</summary>
  /// <param name="socketName">target socket</param>
  void setTargetSocket( char *socketName, ... )
  {
    char name[1024];

    sprintf( name, socketName );
    tSocket[0] = new TlmTargetSocket<BUSWIDTH>( name );
    strcat( name, "_if" );
    mBusSlaveIf[0] = new BusSlaveIf<BUSWIDTH>( name,
					       *mSlaveResetPort[0],
					       *mSlaveFreqPort[0],
					       *tSocket[0] );
    (*tSocket[0])(*mBusSlaveIf[0]);  // bind between socket & I/F
    tSocket[0]->setFuncPtrBaseAddressSize(mBusSlaveIf[0], 
					   &BusSlaveIf<BUSWIDTH>::setfunc);

    va_list list;
    va_start( list, socketName );
    for( unsigned char i=1; i<S_NUM; i++ ){
      sprintf( name, va_arg( list, char* ) );
      tSocket[i] = new TlmTargetSocket<BUSWIDTH>( name );
      strcat( name, "_if" );
      mBusSlaveIf[i] = new BusSlaveIf<BUSWIDTH>( name,
						 *mSlaveResetPort[i],
						 *mSlaveFreqPort[i],
						 *tSocket[i] );
      (*tSocket[i])(*mBusSlaveIf[i]);  // bind between socket & I/F
      tSocket[i]->setFuncPtrBaseAddressSize(mBusSlaveIf[i], 
					     &BusSlaveIf<BUSWIDTH>::setfunc);
    }
    va_end( list );
  }

};


template<unsigned int S_NUM>
class BusSlaveBase<32,S_NUM>
{
 private:
  sc_in<bool>         *mSlaveResetPort32[S_NUM]; //reset port
  sc_in<uint64>       *mSlaveFreqPort32[S_NUM]; //frequency port
 public:
  TlmTargetSocket<32> *tSocket32[S_NUM]; //initiator socket
  BusSlaveIf<32>      *mBusSlaveIf32[S_NUM]; //bus master I/F

  /// <summary>constructer</summary>
  BusSlaveBase(void){}

  /// <summary>destructer</summary>
  ~BusSlaveBase(void){
    for( unsigned char i=0; i<S_NUM; i++ ){
      delete tSocket32[i];
      delete mBusSlaveIf32[i];
    }
  }

  /// <summary>set reset port</summary>
  /// <param name="resetPort">resest port</param>
  void setSlaveResetPort32( sc_in<bool> *resetPort, ... )
  {
    mSlaveResetPort32[0] = resetPort;
    va_list list;
    va_start( list, resetPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mSlaveResetPort32[i] = va_arg( list, sc_in<bool> * );
    }
  }

  /// <summary>set frequency port</summary>
  /// <param name="freqPort">frequency port</param>
  void setSlaveFreqPort32( sc_in<uint64> *freqPort, ... )
  {
    mSlaveFreqPort32[0] = freqPort;
    va_list list;
    va_start( list, freqPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mSlaveFreqPort32[i] = va_arg( list, sc_in<uint64> * );
    }
  }

  /// <summary>set target socket</summary>
  /// <param name="socketName">target socket</param>
  void setTargetSocket32( char *socketName, ... )
  {
    char name[1024];

    sprintf( name, socketName );
    tSocket32[0] = new TlmTargetSocket<32>( name );
    strcat( name, "_if" );
    mBusSlaveIf32[0] = new BusSlaveIf<32>( name,
					   *mSlaveResetPort32[0],
					   *mSlaveFreqPort32[0], 
					   *tSocket32[0] );
    (*tSocket32[0])(*mBusSlaveIf32[0]);  // bind between socket & I/F
    tSocket32[0]->setFuncPtrBaseAddressSize(mBusSlaveIf32[0], 
					     &BusSlaveIf<32>::setfunc);

    va_list list;
    va_start( list, socketName );
    for( unsigned char i=1; i<S_NUM; i++ ){
      sprintf( name, va_arg( list, char* ) );
      tSocket32[i] = new TlmTargetSocket<32>( name );
      strcat( name, "_if" );
      mBusSlaveIf32[i] = new BusSlaveIf<32>( name,
					     *mSlaveResetPort32[i],
					     *mSlaveFreqPort32[i], 
					     *tSocket32[i] );
      (*tSocket32[i])(*mBusSlaveIf32[i]);  // bind between socket & I/F
      tSocket32[i]->setFuncPtrBaseAddressSize(mBusSlaveIf32[i], 
					       &BusSlaveIf<32>::setfunc);
    }
    va_end( list );
  }

};


template<unsigned int S_NUM>
class BusSlaveBase<64,S_NUM>
{
 private:
  sc_in<bool>         *mSlaveResetPort64[S_NUM]; //reset port
  sc_in<uint64>       *mSlaveFreqPort64[S_NUM]; //frequency port
 public:
  TlmTargetSocket<64> *tSocket64[S_NUM]; //initiator socket
  BusSlaveIf<64>      *mBusSlaveIf64[S_NUM]; //bus master I/F

  /// <summary>constructer</summary>
  BusSlaveBase(void){}

  /// <summary>destructer</summary>
  ~BusSlaveBase(void){
    for( unsigned char i=0; i<S_NUM; i++ ){
      delete tSocket64[i];
      delete mBusSlaveIf64[i];
    }
  }

  /// <summary>set reset port</summary>
  /// <param name="resetPort">resest port</param>
  void setSlaveResetPort64( sc_in<bool> *resetPort, ... )
  {
    mSlaveResetPort64[0] = resetPort;
    va_list list;
    va_start( list, resetPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mSlaveResetPort64[i] = va_arg( list, sc_in<bool> * );
    }
  }

  /// <summary>set frequency port</summary>
  /// <param name="freqPort">frequency port</param>
  void setSlaveFreqPort64( sc_in<uint64> *freqPort, ... )
  {
    mSlaveFreqPort64[0] = freqPort;
    va_list list;
    va_start( list, freqPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mSlaveFreqPort64[i] = va_arg( list, sc_in<uint64> * );
    }
  }

  /// <summary>set target socket</summary>
  /// <param name="socketName">target socket</param>
  void setTargetSocket64( char *socketName, ... )
  {
    char name[1024];

    sprintf( name, socketName );
    tSocket64[0] = new TlmTargetSocket<64>( name );
    strcat( name, "_if" );
    mBusSlaveIf64[0] = new BusSlaveIf<64>( name,
					   *mSlaveResetPort64[0],
					   *mSlaveFreqPort64[0],
					   *tSocket64[0] );
    (*tSocket64[0])(*mBusSlaveIf64[0]);  // bind between socket & I/F
    tSocket64[0]->setFuncPtrBaseAddressSize(mBusSlaveIf64[0], 
					     &BusSlaveIf<64>::setfunc);

    va_list list;
    va_start( list, socketName );
    for( unsigned char i=1; i<S_NUM; i++ ){
      sprintf( name, va_arg( list, char* ) );
      tSocket64[i] = new TlmTargetSocket<64>( name );
      strcat( name, "_if" );
      mBusSlaveIf64[i] = new BusSlaveIf<64>( name,
					     *mSlaveResetPort64[i],
					     *mSlaveFreqPort64[i],
					     *tSocket64[i] );
      (*tSocket64[i])(*mBusSlaveIf64[i]);  // bind between socket & I/F
      tSocket64[i]->setFuncPtrBaseAddressSize(mBusSlaveIf64[i], 
					       &BusSlaveIf<64>::setfunc);
    }
    va_end( list );
  }

};


template<unsigned int S_NUM>
class BusSlaveBase<128,S_NUM>
{
 private:
  sc_in<bool>         *mSlaveResetPort128[S_NUM]; //reset port
  sc_in<uint64>       *mSlaveFreqPort128[S_NUM]; //frequency port
 public:
  TlmTargetSocket<128> *tSocket128[S_NUM]; //initiator socket
  BusSlaveIf<128>      *mBusSlaveIf128[S_NUM]; //bus master I/F

  /// <summary>constructer</summary>
  BusSlaveBase(void){}

  /// <summary>destructer</summary>
  ~BusSlaveBase(void){
    for( unsigned char i=0; i<S_NUM; i++ ){
      delete tSocket128[i];
      delete mBusSlaveIf128[i];
    }
  }

  /// <summary>set reset port</summary>
  /// <param name="resetPort">resest port</param>
  void setSlaveResetPort128( sc_in<bool> *resetPort, ... )
  {
    mSlaveResetPort128[0] = resetPort;
    va_list list;
    va_start( list, resetPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mSlaveResetPort128[i] = va_arg( list, sc_in<bool> * );
    }
  }

  /// <summary>set frequency port</summary>
  /// <param name="freqPort">frequency port</param>
  void setSlaveFreqPort128( sc_in<uint64> *freqPort, ... )
  {
    mSlaveFreqPort128[0] = freqPort;
    va_list list;
    va_start( list, freqPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mSlaveFreqPort128[i] = va_arg( list, sc_in<uint64> * );
    }
  }

  /// <summary>set target socket</summary>
  /// <param name="socketName">target socket</param>
  void setTargetSocket128( char *socketName, ... )
  {
    char name[1024];

    sprintf( name, socketName );
    tSocket128[0] = new TlmTargetSocket<128>( name );
    strcat( name, "_if" );
    mBusSlaveIf128[0] = new BusSlaveIf<128>( name,
					   *mSlaveResetPort128[0],
					   *mSlaveFreqPort128[0],
					   *tSocket128[0] );
    (*tSocket128[0])(*mBusSlaveIf128[0]);  // bind between socket & I/F
    tSocket128[0]->setFuncPtrBaseAddressSize(mBusSlaveIf128[0], 
					     &BusSlaveIf<128>::setfunc);

    va_list list;
    va_start( list, socketName );
    for( unsigned char i=1; i<S_NUM; i++ ){
      sprintf( name, va_arg( list, char* ) );
      tSocket128[i] = new TlmTargetSocket<128>( name );
      strcat( name, "_if" );
      mBusSlaveIf128[i] = new BusSlaveIf<128>( name,
					     *mSlaveResetPort128[i],
					     *mSlaveFreqPort128[i],
					     *tSocket128[i] );
      (*tSocket128[i])(*mBusSlaveIf128[i]);  // bind between socket & I/F
      tSocket128[i]->setFuncPtrBaseAddressSize(mBusSlaveIf128[i], 
					       &BusSlaveIf<128>::setfunc);
    }
    va_end( list );
  }

};

#endif //__BUSSLAVEBASE_H__
