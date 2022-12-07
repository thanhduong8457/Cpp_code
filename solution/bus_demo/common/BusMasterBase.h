/*
* Copyright(c) 2011 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*/

#ifndef __BUSMASTERBASE_H__
#define __BUSMASTERBASE_H__

#include "BusMasterIf.h"
#include "CmErrMsg.h"  // for error message
#include <stdarg.h>

#define iSocketFlsh  iSocket128
#define iSocketVci   iSocket64
#define iSocketVci32 iSocket32
#define iSocketAxi   iSocket64
#define iSocketAhb   iSocket32
#define iSocketApb   iSocket32
#define iSocketVpi   iSocket32
#define iSocketGram  iSocket64

#define mBusMasterIfFlsh  mBusMasterIf128
#define mBusMasterIfVci   mBusMasterIf64
#define mBusMasterIfVci32 mBusMasterIf32
#define mBusMasterIfAxi   mBusMasterIf64
#define mBusMasterIfAhb   mBusMasterIf32
#define mBusMasterIfApb   mBusMasterIf32
#define mBusMasterIfVpi   mBusMasterIf32
#define mBusMasterIfGram  mBusMasterIf64

#define setMasterResetPortFlsh  setMasterResetPort128
#define setMasterResetPortVci   setMasterResetPort64
#define setMasterResetPortVci32 setMasterResetPort32
#define setMasterResetPortAxi   setMasterResetPort64
#define setMasterResetPortAhb   setMasterResetPort32
#define setMasterResetPortApb   setMasterResetPort32
#define setMasterResetPortVpi   setMasterResetPort32
#define setMasterResetPortGram  setMasterResetPort64

#define setMasterFreqPortFlsh  setMasterFreqPort128
#define setMasterFreqPortVci   setMasterFreqPort64
#define setMasterFreqPortVci32 setMasterFreqPort32
#define setMasterFreqPortAxi   setMasterFreqPort64
#define setMasterFreqPortAhb   setMasterFreqPort32
#define setMasterFreqPortApb   setMasterFreqPort32
#define setMasterFreqPortVpi   setMasterFreqPort32
#define setMasterFreqPortGram  setMasterFreqPort64

#define setInitiatorSocketFlsh  setInitiatorSocket128
#define setInitiatorSocketVci   setInitiatorSocket64
#define setInitiatorSocketVci32 setInitiatorSocket32
#define setInitiatorSocketAxi   setInitiatorSocket64
#define setInitiatorSocketAhb   setInitiatorSocket32
#define setInitiatorSocketApb   setInitiatorSocket32
#define setInitiatorSocketVpi   setInitiatorSocket32
#define setInitiatorSocketGram  setInitiatorSocket64

template<unsigned int BUSWIDTH, unsigned int S_NUM>
class BusMasterBase
{
 private:
  sc_in<bool>   *mMasterResetPort[S_NUM]; //reset port
  sc_in<uint64> *mMasterFreqPort[S_NUM]; //frequency port
 public:
  TlmInitiatorSocket<BUSWIDTH> *iSocket[S_NUM]; //initiator socket
  BusMasterIf<BUSWIDTH>        *mBusMasterIf[S_NUM]; //bus master I/F

  /// <summary>constructer</summary>
  BusMasterBase( void ){}
  
  /// <summary>destructer</summary>
  ~BusMasterBase(void){
    for( unsigned int i=0; i<S_NUM; i++ ){
      delete iSocket[i];
      delete mBusMasterIf[i];
    }
  }

  /// <summary>set reset port</summary>
  /// <param name="resetPort">resest port</param>
  void setMasterResetPort( sc_in<bool> *resetPort, ... )
  {
    mMasterResetPort[0] = resetPort;
    va_list list;
    va_start( list, resetPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mMasterResetPort[i] = va_arg( list, sc_in<bool> * );
    }
  }

  /// <summary>set frequency port</summary>
  /// <param name="freqPort">frequency port</param>
  void setMasterFreqPort( sc_in<uint64> *freqPort, ... )
  {
    mMasterFreqPort[0] = freqPort;
    va_list list;
    va_start( list, freqPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mMasterFreqPort[i] = va_arg( list, sc_in<uint64> * );
    }
  }

  /// <summary>set initiator socket</summary>
  /// <param name="socketName">initiator socket</param>
  void setInitiatorSocket( char *socketName, ... )
  {
    char name[1024];

    sprintf( name, socketName );
    iSocket[0] = new TlmInitiatorSocket<BUSWIDTH>( name );
    strcat( name, "_if" );
    mBusMasterIf[0] = 
      new BusMasterIf<BUSWIDTH>( name, *mMasterResetPort[0], 
				 *mMasterFreqPort[0], *iSocket[0] );
    (*iSocket[0])(*mBusMasterIf[0]);  // bind between socket & I/F 

    va_list list;
    va_start( list, socketName );
    for( unsigned int i=1; i<S_NUM; i++ ){
      sprintf( name, va_arg( list, char* ) );
      iSocket[i] = new TlmInitiatorSocket<BUSWIDTH>( name );
      strcat( name, "_if" );
      mBusMasterIf[i] = 
	new BusMasterIf<BUSWIDTH>( name, *mMasterResetPort[i], 
				   *mMasterFreqPort[i], *iSocket[i] );
      (*iSocket[i])(*mBusMasterIf[i]);  // bind between socket & I/F 
    }
    va_end( list );
  }

};


template<unsigned int S_NUM>
class BusMasterBase<32,S_NUM>
{
 private:
  sc_in<bool>   *mMasterResetPort32[S_NUM]; //reset port
  sc_in<uint64> *mMasterFreqPort32[S_NUM]; //frequency port
 public:
  TlmInitiatorSocket<32> *iSocket32[S_NUM]; //initiator socket
  BusMasterIf<32>        *mBusMasterIf32[S_NUM]; //bus master I/F

  /// <summary>constructor</summary>
  BusMasterBase( void ){}
  
  /// <summary>desstructor</summary>
  ~BusMasterBase(void){
    for( unsigned int i=0; i<S_NUM; i++ ){
      delete iSocket32[i];
      delete mBusMasterIf32[i];
    }
  }

  /// <summary>set reset port</summary>
  /// <param name="resetPort">resest port</param>
  void setMasterResetPort32( sc_in<bool> *resetPort, ... )
  {
    mMasterResetPort32[0] = resetPort;
    va_list list;
    va_start( list, resetPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mMasterResetPort32[i] = va_arg( list, sc_in<bool> * );
    }
  }

  /// <summary>set frequency port</summary>
  /// <param name="freqPort">frequency port</param>
  void setMasterFreqPort32( sc_in<uint64> *freqPort, ... )
  {
    mMasterFreqPort32[0] = freqPort;
    va_list list;
    va_start( list, freqPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mMasterFreqPort32[i] = va_arg( list, sc_in<uint64> * );
    }
  }

  /// <summary>set initiator socket</summary>
  /// <param name="socketName">socket name</param>
  void setInitiatorSocket32( char *socketName, ... )
  {
    char name[1024];

    sprintf( name, socketName );
    iSocket32[0] = new TlmInitiatorSocket<32>( name );
    strcat( name, "_if" );
    mBusMasterIf32[0] = 
      new BusMasterIf<32>( name, *mMasterResetPort32[0], 
			   *mMasterFreqPort32[0], *iSocket32[0] );
    (*iSocket32[0])(*mBusMasterIf32[0]);  // bind between socket & I/F 

    va_list list;
    va_start( list, socketName );
    for( unsigned int i=1; i<S_NUM; i++ ){
      sprintf( name, va_arg( list, char* ) );
      iSocket32[i] = new TlmInitiatorSocket<32>( name );
      strcat( name, "_if" );
      mBusMasterIf32[i] = 
	new BusMasterIf<32>( name, *mMasterResetPort32[i], 
			     *mMasterFreqPort32[i], *iSocket32[i] );
      (*iSocket32[i])(*mBusMasterIf32[i]);  // bind between socket & I/F 
    }
    va_end( list );
  }

};


template<unsigned int S_NUM>
class BusMasterBase<64,S_NUM>
{
 private:
  sc_in<bool>   *mMasterResetPort64[S_NUM]; //reset port
  sc_in<uint64> *mMasterFreqPort64[S_NUM]; // frequency port
 public:
  TlmInitiatorSocket<64> *iSocket64[S_NUM]; //initiator socket
  BusMasterIf<64>        *mBusMasterIf64[S_NUM]; //bus master I/F

  /// <summary>constructor</summary>
  BusMasterBase( void ){}
  
  /// <summary>destructor</summary>
  ~BusMasterBase(void){
    for( unsigned int i=0; i<S_NUM; i++ ){
      delete iSocket64[i];
      delete mBusMasterIf64[i];
    }
  }

  /// <summary>set reset port</summary>
  /// <param name="resetPort">resest port</param>
  void setMasterResetPort64( sc_in<bool> *resetPort, ... )
  {
    mMasterResetPort64[0] = resetPort;
    va_list list;
    va_start( list, resetPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mMasterResetPort64[i] = va_arg( list, sc_in<bool> * );
    }
  }

  /// <summary>set frequency port</summary>
  /// <param name="freqPort">frequency port</param>
  void setMasterFreqPort64( sc_in<uint64> *freqPort, ... )
  {
    mMasterFreqPort64[0] = freqPort;
    va_list list;
    va_start( list, freqPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mMasterFreqPort64[i] = va_arg( list, sc_in<uint64> * );
    }
  }

  /// <summary>set initiator socket</summary>
  /// <param name="socketName">socket name</param>
  void setInitiatorSocket64( char *socketName, ... )
  {
    char name[1024];

    sprintf( name, socketName );
    iSocket64[0] = new TlmInitiatorSocket<64>( name );
    strcat( name, "_if" );
    mBusMasterIf64[0] = 
      new BusMasterIf<64>( name, *mMasterResetPort64[0], 
			   *mMasterFreqPort64[0], *iSocket64[0] );
    (*iSocket64[0])(*mBusMasterIf64[0]);  // bind between socket & I/F 

    va_list list;
    va_start( list, socketName );
    for( unsigned int i=1; i<S_NUM; i++ ){
      sprintf( name, va_arg( list, char* ) );
      iSocket64[i] = new TlmInitiatorSocket<64>( name );
      strcat( name, "_if" );
      mBusMasterIf64[i] = 
	new BusMasterIf<64>( name, *mMasterResetPort64[i], 
			     *mMasterFreqPort64[i], *iSocket64[i] );
      (*iSocket64[i])(*mBusMasterIf64[i]);  // bind between socket & I/F 
    }
    va_end( list );
  }

};


template<unsigned int S_NUM>
class BusMasterBase<128,S_NUM>
{
 private:
  sc_in<bool>   *mMasterResetPort128[S_NUM]; //reset port
  sc_in<uint64> *mMasterFreqPort128[S_NUM]; //frequency
 public:
  TlmInitiatorSocket<128> *iSocket128[S_NUM]; //initiator socket
  BusMasterIf<128>        *mBusMasterIf128[S_NUM]; //bus master I/F

  /// <summary>constructor</summary>
  BusMasterBase( void ){}
  
  /// <summary>destructor</summary>
  ~BusMasterBase(void){
    for( unsigned int i=0; i<S_NUM; i++ ){
      delete iSocket128[i];
      delete mBusMasterIf128[i];
    }
  }

  /// <summary>set reset port</summary>
  /// <param name="resetPort">resest port</param>
  void setMasterResetPort128( sc_in<bool> *resetPort, ... )
  {
    mMasterResetPort128[0] = resetPort;
    va_list list;
    va_start( list, resetPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mMasterResetPort128[i] = va_arg( list, sc_in<bool> * );
    }
  }

  /// <summary>set frequency port</summary>
  /// <param name="freqPort">frequency port</param>
  void setMasterFreqPort128( sc_in<uint64> *freqPort, ... )
  {
    mMasterFreqPort128[0] = freqPort;
    va_list list;
    va_start( list, freqPort );
    for( unsigned int i=1; i<S_NUM; i++ ){
      mMasterFreqPort128[i] = va_arg( list, sc_in<uint64> * );
    }
  }

  /// <summary>set initiator socket</summary>
  /// <param name="socketName">socket name</param>
  void setInitiatorSocket128( char *socketName, ... )
  {
    char name[1024];

    sprintf( name, socketName );
    iSocket128[0] = new TlmInitiatorSocket<128>( name );
    strcat( name, "_if" );
    mBusMasterIf128[0] = 
      new BusMasterIf<128>( name, *mMasterResetPort128[0], 
			    *mMasterFreqPort128[0], *iSocket128[0] );
    (*iSocket128[0])(*mBusMasterIf128[0]);  // bind between socket & I/F 

    va_list list;
    va_start( list, socketName );
    for( unsigned int i=1; i<S_NUM; i++ ){
      sprintf( name, va_arg( list, char* ) );
      iSocket128[i] = new TlmInitiatorSocket<128>( name );
      strcat( name, "_if" );
      mBusMasterIf128[i] = 
	new BusMasterIf<128>( name, *mMasterResetPort128[i], 
			      *mMasterFreqPort128[i], *iSocket128[i] );
      (*iSocket128[i])(*mBusMasterIf128[i]);  // bind between socket & I/F 
    }
    va_end( list );
  }

};

#endif //__BUSMASTERBASE_H__
