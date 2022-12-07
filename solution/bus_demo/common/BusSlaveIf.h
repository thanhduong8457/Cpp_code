/*
* Copyright(c) 2011-2020 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*/

#ifndef __BUSSLAVEIF_H__
#define __BUSSLAVEIF_H__

#include "OSCI2.h"     // class for User Modeling environment based on TLM2.0
#include "CmErrMsg.h"  // for error message
#include "BusSlaveFuncIf.h"
#include <map>
#include <queue>
#include <tlm_utils/peq_with_get.h>

extern sc_time_unit glb_resolution_unit;

template<unsigned int BUSWIDTH>
class BusSlaveIf :
public BusAccessBreak,
public sc_module,
public virtual TlmFwTransportIf
{
 public:
  SC_HAS_PROCESS(BusSlaveIf);
  /// <summary>constructor</summary>
  BusSlaveIf(sc_module_name  name,
	     sc_in<bool>     &resetPort,
	     sc_in<uint64>   &freqPort,
#if 0 //DMI is not supported
	     TlmTargetSocket<BUSWIDTH> &tSocket,
	     bool            invalidate = false ):
#else //DMI is not supported
    TlmTargetSocket<BUSWIDTH> &tSocket ):
#endif//DMI is not supported
    sc_module( name ),
    targetSocket( tSocket ),
    mpFuncModule( (BusSlaveFuncIf*)0 ),
    mResetPort( resetPort ),
    mFreqPort( freqPort ),
    meRRespEvent(),
    meWRespEvent(),
    meReadEvent(),
    meWriteEvent(),
    mBusProtocol( BUS_APB ),
#if 0 //DMI is not supported
    mInvalidate( false ),
    mInvalidateDmiEvents(),
    mInvalidateDmiTime( SC_ZERO_TIME ),
#endif//DMI is not supported
    mSystemClock( (uint64)0 ),
    mrInitLatencyCycle( (uint64)0 ),
    mwInitLatencyCycle( (uint64)0 ),
    mrFirstDataLatencyCycle( (uint64)0 ),
    mwFirstDataLatencyCycle( (uint64)0 ),
    mrNextDataLatencyCycle( (uint64)0 ),
    mwNextDataLatencyCycle( (uint64)0 ),
    mrInitLatencyTime( SC_ZERO_TIME ),
    mwInitLatencyTime( SC_ZERO_TIME ),
    mrFirstDataLatencyTime( SC_ZERO_TIME ),
    mwFirstDataLatencyTime( SC_ZERO_TIME ),
    mrNextDataLatencyTime( SC_ZERO_TIME ),
    mwNextDataLatencyTime( SC_ZERO_TIME ),
    mrInitLatencyUint64( (uint64)0 ),
    mwInitLatencyUint64( (uint64)0 ),
    mrFirstDataLatencyUint64( (uint64)0 ),
    mwFirstDataLatencyUint64( (uint64)0 ),
    mrNextDataLatencyUint64( (uint64)0 ),
    mwNextDataLatencyUint64( (uint64)0 ),
    mpRTransInfoFwFifo( (std::queue< transInfo_t > *)0 ),
    mpWTransInfoFwFifo( (std::queue< transInfo_t > *)0 ),
    mpRTransBwFifo( (std::queue< TlmTransactionType* > *)0 ),
    mpWTransBwFifo( (std::queue< TlmTransactionType* > *)0 ),
    mpRWaitFwTrans( (bool *)0 ),
    mpWWaitFwTrans( (bool *)0 ),
    mpRWaitBwTrans( (bool *)0 ),
    mpWWaitBwTrans( (bool *)0 ),
    mRRespPreprocessPEQ("RRespPreprocessPEQ"),
    mWRespPreprocessPEQ("WRespPreprocessPEQ"),
    mTransNmb( (unsigned int)0 ),
    mTransNmbLmt( (unsigned int)1 ),
    mAddRInitLatencyCycle( (uint64)1 ),
    mAddWInitLatencyCycle( (uint64)1 ),
    mAddRFirstDataLatencyCycle( (uint64)0 ),
    mAddWFirstDataLatencyCycle( (uint64)0 ),
    mAddRNextDataLatencyCycle( (uint64)0 ),
    mAddWNextDataLatencyCycle( (uint64)0 ),
    mBusWidthInByte( BUSWIDTH/8 ),
    mPipeLineOp( false ),
    mApplyLatency( true ),
    mEnableToCheckDataSize( false ),
    mIsG3mExtensionSet( false )
  {
#if 0 //DMI is not supported
    if( invalidate ){
      SC_METHOD(invalidateDmiMethod);
      sensitive << mInvalidateDmiEvent;
      dont_initialize();
      mInvalidateDmiTime = sc_time(25, SC_NS);
    }
#endif//DMI is not supported

    SC_METHOD(readProcess);
    sensitive << meReadEvent;
    dont_initialize();

    SC_METHOD(writeProcess);
    sensitive << meWriteEvent;
    dont_initialize();

    SC_METHOD(rRespPreprocess);
    sensitive << mRRespPreprocessPEQ.get_event();
    dont_initialize();

    SC_METHOD(wRespPreprocess);
    sensitive << mWRespPreprocessPEQ.get_event();
    dont_initialize();

    SC_METHOD(rRespProcess);
    sensitive << meRRespEvent;
    dont_initialize();

    SC_METHOD(wRespProcess);
    sensitive << meWRespEvent;
    dont_initialize();

    SC_METHOD(resetProcess);
    sensitive << mResetPort.neg();
    dont_initialize();

    SC_METHOD( calPeriodProcess );
    sensitive << mFreqPort;
    dont_initialize();

    mpRTransInfoFwFifo = new std::queue< transInfo_t >;
    mpWTransInfoFwFifo = mpRTransInfoFwFifo;
    mpRTransBwFifo = new std::queue< TlmTransactionType* >;
    mpWTransBwFifo = mpRTransBwFifo;

    mpRWaitFwTrans = new bool;
    mpWWaitFwTrans = mpRWaitFwTrans;
    mpRWaitBwTrans = new bool;
    mpWWaitBwTrans = mpRWaitBwTrans;
    *mpRWaitFwTrans = false;
    *mpRWaitBwTrans = false;

  }

  /// <summary>destructor</summary>
  ~BusSlaveIf(void)
  {
    if (mpWTransInfoFwFifo != mpRTransInfoFwFifo) {
      delete mpWTransInfoFwFifo;
    }
    delete mpRTransInfoFwFifo;

    if (mpWTransBwFifo != mpRTransBwFifo) {
      delete mpWTransBwFifo;
    }
    delete mpRTransBwFifo;

    if (mpWWaitFwTrans != mpRWaitFwTrans) {
      delete mpWWaitFwTrans;
    }
    delete mpRWaitFwTrans;

    if (mpWWaitBwTrans != mpRWaitBwTrans) {
      delete mpWWaitBwTrans;
    }
    delete mpRWaitBwTrans;
  }

  /// <summary>set read latency for APB protocol</summary>
  /// <param name="readLatency">read latency</param>
  void setReadLatency( uint64 readLatency )
  {
    mrInitLatencyCycle = readLatency;
    calPeriod();
  }

  /// <summary>set write latency for APB protocol</summary>
  /// <param name="writeLatency">write latency</param>
  void setWriteLatency( uint64 writeLatency )
  {
    mwInitLatencyCycle = writeLatency;
    calPeriod();
  }

  /// <summary>set read initial latency for AXI,AHB,VPI protocol</summary>
  /// <param name="readLatency">read initial latency</param>
  void setReadInitLatency( uint64 readLatency )
  {
    mrInitLatencyCycle = readLatency;
    calPeriod();
  }

  /// <summary>set write initial latency for AXI,AHB,VPI protocol</summary>
  /// <param name="writeLatency">write initial latency</param>
  void setWriteInitLatency( uint64 writeLatency )
  {
    mwInitLatencyCycle = writeLatency;
    calPeriod();
  }

  /// <summary>set read first data latency for AXI,AHB,VPI protocol</summary>
  /// <param name="readLatency">read first data latency</param>
  void setReadFirstDataLatency( uint64 readLatency )
  {
    mrFirstDataLatencyCycle = readLatency;
    calPeriod();
  }

  /// <summary>set write first data latency for AXI,AHB,VPI protocol</summary>
  /// <param name="writeLatency">write first data latency</param>
  void setWriteFirstDataLatency( uint64 writeLatency )
  {
    mwFirstDataLatencyCycle = writeLatency;
    calPeriod();
  }

  /// <summary>set read next data latency for AXI,AHB,VPI protocol</summary>
  /// <param name="readLatency">read next data latency</param>
  void setReadNextDataLatency( uint64 readLatency )
  {
    mrNextDataLatencyCycle = readLatency;
    calPeriod();
  }

  /// <summary>set write next data latency for AXI,AHB,VPI protocol</summary>
  /// <param name="writeLatency">write next data latency</param>
  void setWriteNextDataLatency( uint64 writeLatency )
  {
    mwNextDataLatencyCycle = writeLatency;
    calPeriod();
  }

  /// <summary>set transaction number limitation</summary>
  /// <param name="Nmb">transaction number</param>
  void setTransNmbLmt( unsigned int Nmb ){
    mTransNmbLmt = Nmb;
  }

  /// <summary>set bus protocol</summary>
  /// <param name="protocol">bus protocol</param>
  void setBusProtocol( BusProtocol_t protocol )
  {
    mBusProtocol = protocol;
    switch( mBusProtocol ){
    case BUS_APB:
      mAddRInitLatencyCycle = 1;
      mAddWInitLatencyCycle = 1;
      mAddRFirstDataLatencyCycle = 0;
      mAddWFirstDataLatencyCycle = 0;
      mAddRNextDataLatencyCycle = 0;
      mAddWNextDataLatencyCycle = 0;
      mpWTransInfoFwFifo = mpRTransInfoFwFifo;
      mpWTransBwFifo = mpRTransBwFifo;
      mpWWaitFwTrans = mpRWaitFwTrans;
      mpWWaitBwTrans = mpRWaitBwTrans;
      mTransNmbLmt = 1;
      mPipeLineOp = false;
      break;
    case BUS_AHB:
      mAddRInitLatencyCycle = 1; 
      mAddWInitLatencyCycle = 1; 
      mAddRFirstDataLatencyCycle = 0;
      mAddWFirstDataLatencyCycle = 0;
      mAddRNextDataLatencyCycle = 0;
      mAddWNextDataLatencyCycle = 0;
      mpWTransInfoFwFifo = mpRTransInfoFwFifo;
      mpWTransBwFifo = mpRTransBwFifo;
      mpWWaitFwTrans = mpRWaitFwTrans;
      mpWWaitBwTrans = mpRWaitBwTrans;
      mTransNmbLmt = 1;
      mPipeLineOp = false;
      break;
    case BUS_AXI:
      mAddRInitLatencyCycle = 1;
      mAddWInitLatencyCycle = 1;
      mAddRFirstDataLatencyCycle = 1;
      mAddWFirstDataLatencyCycle = 1;
      mAddRNextDataLatencyCycle = 1;
      mAddWNextDataLatencyCycle = 1;
      mpWTransInfoFwFifo = new std::queue< transInfo_t >;
      mpWTransBwFifo = new std::queue< TlmTransactionType* >;
      mpWWaitFwTrans = new bool;
      mpWWaitBwTrans = new bool;
      *mpWWaitFwTrans = false;
      *mpWWaitBwTrans = false;
      mTransNmbLmt = 0xFFFFFFFF;
      mPipeLineOp = false;
      break;
    case BUS_VPI:// temp for E1.00
      mAddRInitLatencyCycle = 2;
      mAddWInitLatencyCycle = 3;
      mAddRFirstDataLatencyCycle = 0;
      mAddWFirstDataLatencyCycle = 0;
      mAddRNextDataLatencyCycle = 0;
      mAddWNextDataLatencyCycle = 0;
      mpWTransInfoFwFifo = mpRTransInfoFwFifo;
      mpWTransBwFifo = mpRTransBwFifo;
      mpWWaitFwTrans = mpRWaitFwTrans;
      mpWWaitBwTrans = mpRWaitBwTrans;
      mTransNmbLmt = 0xFFFFFFFF;
      mPipeLineOp = true;
      break;
    case BUS_VCI:
      mAddRInitLatencyCycle = 1;
      mAddWInitLatencyCycle = 1;
      mAddRFirstDataLatencyCycle = 0;
      mAddWFirstDataLatencyCycle = 0;
      mAddRNextDataLatencyCycle = 0;
      mAddWNextDataLatencyCycle = 0;
      mpWTransInfoFwFifo = mpRTransInfoFwFifo;
      mpWTransBwFifo = mpRTransBwFifo;
      mpWWaitFwTrans = mpRWaitFwTrans;
      mpWWaitBwTrans = mpRWaitBwTrans;
      mTransNmbLmt = 0xFFFFFFFF;
      mPipeLineOp = false;
      break;
    }
    calPeriod();
  }


  /// <summary>set 0 latency</summary>
  void set0Latency( void )
  {
    mrInitLatencyCycle = 0;
    mwInitLatencyCycle = 0;
    mrFirstDataLatencyCycle = 0;
    mwFirstDataLatencyCycle = 0;
    mrNextDataLatencyCycle = 0;
    mwNextDataLatencyCycle = 0;

    mAddRInitLatencyCycle = 0;
    mAddWInitLatencyCycle = 0;
    mAddRFirstDataLatencyCycle = 0;
    mAddWFirstDataLatencyCycle = 0;
    mAddRNextDataLatencyCycle = 0;
    mAddWNextDataLatencyCycle = 0;

    mApplyLatency = false;

    calPeriod();
  }


  /// <summary>set enable to check data size</summary>
  void setEnableToCheckDataSize( void )
  {
    mEnableToCheckDataSize = true;
  }


  /// <summary>forward pass of non blocking transport</summary>
  /// <param name="trans">transaction</param>
  /// <param name="phase">TLM phase</param>
  /// <param name="t">time</param>
  /// <returns>synchronization word</returns>
  TlmSyncEnum nb_transport_fw(TlmTransactionType& trans, 
			      TlmPhase&           phase, 
			      sc_time&            t)
  {
#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"BusSlaveIf::nb_transport_fw() is called from %s\n",
	      targetSocket.name());
      outputLogAtRecep( &trans );
      if( phase == tlm::BEGIN_REQ ){
	fprintf(tstFp,"             phase =tlm::BEGIN_REQ\n");
      }
      else{
	fprintf(tstFp,"             phase =other\n");
      }
      fprintf(tstFp,"        t =%s\n",
	      t.to_string().c_str());
    }
#endif //BUSIF_TST
    if( phase != tlm::BEGIN_REQ ){
      char errorMessageBuf[1024];
      sprintf( errorMessageBuf,
	       "Phase was wrong when nb_transport_fw() was called. It was not BEGIN_REQ." );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    }

    if( mEnableToCheckDataSize ){
      checkDataSize( trans );
    }

#if 1 //for Bridge i/f
    unsigned int offsetAddress = checkAddress( trans );
#else //for Bridge i/f
    unsigned int offsetAddress = 
      trans.get_address()-targetSocket.getBaseAddress();
    if( offsetAddress >= targetSocket.getBaseSize() ){
      char errorMessageBuf[1024];
      sprintf( errorMessageBuf,
	       "Received the transaction to wrong address[0x%llx].",
	       trans.get_address() );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    }
#endif//for Bridge i/f

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    trans.set_dmi_allowed(false);
    phase = tlm::END_REQ;

    transInfo_t transInfo;
    transInfo.trans    = &trans;
    transInfo.receiveT = sc_time_stamp();
    if( trans.get_data_length() < 9 ){
      t = mAtFwLatency[trans.get_command()][trans.get_data_length()] + t;
    }
    else{
      t = calFwLatency(trans) + t;
    }
    t                  = calTimeToEdge( t );
    transInfo.t        = t;
    if( trans.get_command() == tlm::TLM_WRITE_COMMAND ){
      mpWTransInfoFwFifo->push( transInfo );
      if( !(*mpWWaitFwTrans) ){
	meWriteEvent.notify( transInfo.t );
	*mpWWaitFwTrans = true;
      }
    }
    else{
      mpRTransInfoFwFifo->push( transInfo );
      if( !(*mpRWaitFwTrans) ){
	meReadEvent.notify( transInfo.t );
	*mpRWaitFwTrans = true;
      }
    }
    mTransNmb++;
    if( mTransNmb > mTransNmbLmt ){
#ifdef BUSIF_TST
      if(tstFp){
	fprintf(tstFp,"        Return TLM_ACCEPTED\n");
      }
#endif //BUSIF_TST
      return tlm::TLM_ACCEPTED;
    }
    else{
#ifdef BUSIF_TST
      if(tstFp){
	fprintf(tstFp,"        Return TLM_UPDATED\n");
      }
#endif //BUSIF_TST
      return tlm::TLM_UPDATED;
    }
    
  }

  /// <summary>blocking transport</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  void b_transport(TlmTransactionType& trans, sc_time& t)
  {
#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"BusSlaveIf::b_transport() is called from %s\n",
	      targetSocket.name());
      outputLogAtRecep( &trans );
      fprintf(tstFp,"        t =%s\n",
	      t.to_string().c_str());
    }
#endif //BUSIF_TST

    if( mEnableToCheckDataSize ){
      checkDataSize( trans );
    }

#if 1 //for Bridge i/f
    unsigned int offsetAddress = checkAddress( trans );
#else //for Bridge i/f
    unsigned int offsetAddress = 
      trans.get_address()-targetSocket.getBaseAddress();
    if( offsetAddress >= targetSocket.getBaseSize() ){
      char errorMessageBuf[1024];
      sprintf( errorMessageBuf,
	       "Received the transaction to wrong address[0x%llx].",
	       trans.get_address() );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    }
#endif//for Bridge i/f

    if( trans.get_command() == tlm::TLM_WRITE_COMMAND ){
      mpFuncModule->write( offsetAddress,
			   trans,
			   &t );
    }
    else{
      mpFuncModule->read( offsetAddress,
			  trans,
			  &t );
    }

    BusAccessBreak::checkHitBreakAddress( trans );

    if( trans.get_data_length() < 9 ){
      t += mLtLatency[trans.get_command()][trans.get_data_length()];
    }
    else{
      uint64 latency = calInitLatency( trans );
      latency += calDataLatency( trans );
      latency += calCompRecepLatency( trans );
#if 0 //*** Bus clock timing is not considered in LT ***
      latency += calTimeToEdge( t );
#endif
      t += sc_time( (double)latency, glb_resolution_unit );
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    trans.set_dmi_allowed(false);
#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"BusSlaveIf::Return from b_transport()\n");
      outputLogAtResp( &trans );
      fprintf(tstFp,"        t =%s\n",
	      t.to_string().c_str());
    }
#endif //BUSIF_TST
  }

  /// <summary>debug transport</summary>
  /// <param name="r">transaction</param>
  /// <returns>treated data byte number</returns>
  unsigned int transport_dbg(TlmTransactionType& r)
  {
    
#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"BusSlaveIf::transport_dbg() is called from %s\n",
	      targetSocket.name());
    }
#endif //BUSIF_TST

    setG3mExtension( r );

#ifdef BUSIF_TST
    if(tstFp){
      outputLogAtRecep( &r );
    }
#endif //BUSIF_TST

    unsigned int num_bytes, org_num_bytes;
    BusTime_t t = SC_ZERO_TIME;
#if 1 //for Bridge i/f
    unsigned int offsetAddress = checkAddress( r );
#else //for Bridge i/f
    unsigned int offsetAddress = 
      r.get_address()-targetSocket.getBaseAddress();
    
    if( offsetAddress >= targetSocket.getBaseSize() ){
      char errorMessageBuf[1024];
      sprintf( errorMessageBuf,
	       "Received the transaction to wrong address[0x%llx].",
	       r.get_address() );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    }
#endif//for Bridge i/f
    
    if ( offsetAddress + r.get_data_length() >= 
	 targetSocket.getBaseSize() ) {
      org_num_bytes = r.get_data_length();
      num_bytes = 
	(unsigned int)(targetSocket.getBaseSize() - offsetAddress);
      r.set_data_length( num_bytes );
    } else {
      org_num_bytes = num_bytes = r.get_data_length();
    }
    
    if( r.get_command() == tlm::TLM_WRITE_COMMAND ){
      mpFuncModule->write( offsetAddress,
			   r,
			   &t,
			   true );
    }
    else{
      mpFuncModule->read( offsetAddress,
			  r,
			  &t,
			  true );
    }

    r.set_data_length( org_num_bytes );
    
    r.set_response_status(tlm::TLM_OK_RESPONSE);
    
#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"BusSlaveIf::Return from transport_dbg()\n");
      outputLogAtResp( &r );
      fprintf(tstFp,"        t =%s\n",
	      t.to_string().c_str());
      fprintf(tstFp,"        return value =%d\n",num_bytes);
    }
#endif //BUSIF_TST

    releaseG3mExtension( r );

    return num_bytes;
  }
  
  /// <summary>backward pass of non blocking transport</summary>
  /// <param name="trans">transaction</param>
  /// <param name="dmiData">data</param>
  /// <returns>DMI is allowed or not</returns>
  bool get_direct_mem_ptr( TlmTransactionType& trans,
			   tlm::tlm_dmi&  dmiData )
  {
#if 0 //DMI is not supported
    bool bRtn = true;
    uint64 sAdr = (uint64)targetSocket.getBaseAddress();
    uint64 eAdr = sAdr + (uint64)targetSocket.getBaseSize() - (uint64)1;
    
    if( trans.get_address()<sAdr || trans.get_address()>eAdr ){
      bRtn = false;
      dmiData.allow_none();
      dmiData.set_start_address(sAdr);
      dmiData.set_end_address(eAdr);
    }
    else{
      dmiData.allow_read_write();
      dmiData.set_start_address(sAdr);
      dmiData.set_end_address(eAdr);
      dmiData.set_dmi_ptr(mpFuncModule->getMemPtr());
      dmiData.set_read_latency(rLatencyTime);
      dmiData.set_write_latency(wLatencyTime);
    }
    return bRtn;
#else //DMI is not supported
    return false;
#endif//DMI is not supported
  }

#if 0 //DMI is not supported
  /// <summary>inform to invalidate DMI</summary>
  void invalidateDmiMethod() // not used in the BusSlaveIf
  {
    uint64 sAdr = (uint64)targetSocket.getBaseAddress();
    uint64 eAdr = sAdr + (uint64)targetSocket.getBaseSize() - (uint64)1;
    
    targetSocket->invalidate_direct_mem_ptr( sAdr, eAdr );
  }
#endif//DMI is not supported

  /// <summary>set memory size</summary>
  void setfunc(void)
  {
    ADDRESS_TYPE size = targetSocket.getBaseSize();
    mpFuncModule->setfunc( size );
  }

  /// <summary>reset process</summary>
  void resetProcess()
  {
    *mpRWaitFwTrans = false;
    *mpWWaitFwTrans = false;
    *mpRWaitBwTrans = false;
    *mpWWaitBwTrans = false;

    meRRespEvent.cancel();
    meWRespEvent.cancel();
    meReadEvent.cancel();
    meWriteEvent.cancel();

    while( !mpRTransInfoFwFifo->empty() ){
      mpRTransInfoFwFifo->pop();
    }
    if( mpRTransInfoFwFifo != mpWTransInfoFwFifo ){
      while( !mpWTransInfoFwFifo->empty() ){
	mpWTransInfoFwFifo->pop();
      }
    }

    while( !mpRTransBwFifo->empty() ){
      mpRTransBwFifo->pop();
    }
    if( mpRTransBwFifo != mpWTransBwFifo ){
      while( !mpWTransBwFifo->empty() ){
	mpWTransBwFifo->pop();
      }
    }

    mRRespPreprocessPEQ.cancel_all();
    mWRespPreprocessPEQ.cancel_all();

    mTransNmb = 0;
    
    mIsG3mExtensionSet = false;

#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"BusSlaveIf::resetProcess() is called\n");
    }
#endif //BUSIF_TST
  }

  /// <summary>process to calculate clock period</summary>
  void calPeriodProcess( void )
  {
    if( mFreqPort.read() == 0 ){
      mSystemClock = 0xFFFFFFFFFFFFFFFFULL;
    }
    else{
      switch(glb_resolution_unit){
      case SC_SEC:
	mSystemClock = (uint64)(1.0)/mFreqPort.read();
	break;
      case SC_MS:
	mSystemClock = (uint64)(1.0e+3)/mFreqPort.read();
	break;
      case SC_US:
	mSystemClock = (uint64)(1.0e+6)/mFreqPort.read();
	break;
      case SC_NS:
	mSystemClock = (uint64)(1.0e+9)/mFreqPort.read();
	break;
      case SC_PS:
	mSystemClock = (uint64)(1.0e+12)/mFreqPort.read();
	break;
      case SC_FS:
	mSystemClock = (uint64)(1.0e+15)/mFreqPort.read();
	break;
      }
    }
    calPeriod();
  }

  /// <summary>set functional module pointer</summary>
  /// <param name="funcModule">pointer of functional module</param>
  void setFuncModulePtr( BusSlaveFuncIf *funcModule )
  {
    mpFuncModule = funcModule;
  }

  /// <summary>calculate data latency</summary>
  /// <param name="r">transaction</param>
  /// <returns>latency for data trasfer</returns>
  uint64 calDataLatency( TlmTransactionType &trans )
  {

    /***** Calculate the time for Data transmission *****/
    uint64 waitForDataTrans;

    uint64 data_length = trans.get_data_length();
    uint64 beat_number;
    if( data_length > mBusWidthInByte ){
      beat_number = data_length / mBusWidthInByte;
      if( data_length%mBusWidthInByte ){
	beat_number += 1;
      }
    }
    else{
      beat_number = 1;
    }
    if( trans.get_command() == tlm::TLM_WRITE_COMMAND ){
      waitForDataTrans = 
	mwFirstDataLatencyUint64 + (beat_number-1)*mwNextDataLatencyUint64;
    }
    else{
      waitForDataTrans = 
	mrFirstDataLatencyUint64 + (beat_number-1)*mrNextDataLatencyUint64;
    }
    return ( waitForDataTrans );

  }      
  
  /// <summary>get clock period</summary>
  /// <returns>clock period</returns>
  uint64 getClockPeriod( void )
  {
    return( mSystemClock );
  }


public:
  TlmTargetSocket<BUSWIDTH>     &targetSocket; //target socket
  BusSlaveFuncIf                *mpFuncModule; //bus slave functional module I/F

private:
  struct transInfo_t{
    TlmTransactionType *trans;
    sc_time             t;
    sc_time             receiveT;

    transInfo_t(void) :
      trans( 0 ),
      t( SC_ZERO_TIME ),
      receiveT( SC_ZERO_TIME ){}

    ~transInfo_t(void){}
  }; //transaction information

  sc_in<bool>                   &mResetPort; //reset port
  sc_in<uint64>                 &mFreqPort; //frequency port

  sc_event                       meRRespEvent; //read response event
  sc_event                       meWRespEvent; //write response event
  sc_event                       meReadEvent; //read function call event
  sc_event                       meWriteEvent; //write function call event
  BusProtocol_t                  mBusProtocol; //bus protocol
#if 0 //DMI is not supported
  bool                           mInvalidate; //DMI invalidate
  sc_event                       mInvalidateDmiEvent; //event to inform DMI invalidate
  sc_time                        mInvalidateDmiTime; //invalidate DMI time
#endif//DMI is not supported
  uint64                         mSystemClock; // system clock
  uint64                         mrInitLatencyCycle; //read initial latency in cycle
  uint64                         mwInitLatencyCycle; //write initial latency in cycle
  uint64                         mrFirstDataLatencyCycle; //read first data latency in cycle
  uint64                         mwFirstDataLatencyCycle; //writ first data latency in cycle
  uint64                         mrNextDataLatencyCycle; //read next data latency in cycle
  uint64                         mwNextDataLatencyCycle; //write next data latency in cycle
  sc_time                        mrInitLatencyTime; //read initial latency in time
  sc_time                        mwInitLatencyTime; //write initial latency in time
  sc_time                        mrFirstDataLatencyTime; //read first data latency in time
  sc_time                        mwFirstDataLatencyTime; //write first data latency in time
  sc_time                        mrNextDataLatencyTime; //read next data latency in time
  sc_time                        mwNextDataLatencyTime; //write next data latency in time
  uint64                         mrInitLatencyUint64; //read initial latency in uint64
  uint64                         mwInitLatencyUint64; //write initial latency in uint64
  uint64                         mrFirstDataLatencyUint64; //read first data latency in uint64
  uint64                         mwFirstDataLatencyUint64; //write first data latency in uint64
  uint64                         mrNextDataLatencyUint64; //read next data latency in uint64
  uint64                         mwNextDataLatencyUint64; //write next data latency in uint64
  sc_time                        mLtLatency[3][9]; // total latency for LT
  sc_time                        mAtFwLatency[3][9]; // total latency for AT forward path
  sc_time                        mAtBwLatency[3][9]; // total latency for AT backward path

  std::queue< transInfo_t >     *mpRTransInfoFwFifo; //read transaction infomation FW FIFO
  std::queue< transInfo_t >     *mpWTransInfoFwFifo; //write transaction infomation FW FIFO
  std::queue< TlmTransactionType* > *mpRTransBwFifo; //read transaction BW FIFO
  std::queue< TlmTransactionType* > *mpWTransBwFifo; //write transaction BW FIFO
  bool                          *mpRWaitFwTrans; //wait for read FW transport
  bool                          *mpWWaitFwTrans; //wait for write FW transport
  bool                          *mpRWaitBwTrans; //wait for read BW transport
  bool                          *mpWWaitBwTrans; //wait for write BW transport
  tlm_utils::peq_with_get< TlmTransactionType > mRRespPreprocessPEQ; //read resp preprocess queue
  tlm_utils::peq_with_get< TlmTransactionType > mWRespPreprocessPEQ; //write resp preprocess queue

  unsigned int                   mTransNmb; //transaction number
  unsigned int                   mTransNmbLmt; //transaciton number limitation

  uint64                         mAddRInitLatencyCycle; //additional initial latency
  uint64                         mAddWInitLatencyCycle; //additional initial latency
  uint64                         mAddRFirstDataLatencyCycle; //additional first data latency
  uint64                         mAddWFirstDataLatencyCycle; //additional first data latency
  uint64                         mAddRNextDataLatencyCycle; //additional next data latency
  uint64                         mAddWNextDataLatencyCycle; //additional next data latency
  
  unsigned int                   mBusWidthInByte; //bus width in byte

  bool                           mPipeLineOp; //whether pipe line operation is needed

  bool                           mApplyLatency; //whether latency treatment is applied

  bool                           mEnableToCheckDataSize; //whether checking of data size is enable

  bool                           mIsG3mExtensionSet; //whether TlmG3mExtension is set

  /// <summary>calculate clock period</summary>
  void calPeriod( void )
  {
    uint64 rIL;
    uint64 wIL;
    uint64 rFDL;
    uint64 wFDL;
    uint64 rNDL;
    uint64 wNDL;

    if( mSystemClock == 0xFFFFFFFFFFFFFFFFULL ){
      if( mrInitLatencyCycle == 0 ){
	mrInitLatencyUint64      = 0;
	mrInitLatencyTime        = SC_ZERO_TIME;
      }
      else{
	mrInitLatencyUint64      = mSystemClock;
	mrInitLatencyTime        = 
	  sc_time((double)mSystemClock,glb_resolution_unit);
      }
      if( mwInitLatencyCycle == 0 ){
	mwInitLatencyUint64      = 0;
	mwInitLatencyTime        = SC_ZERO_TIME;
      }
      else{
	mwInitLatencyUint64      = mSystemClock;
	mwInitLatencyTime        = 
	  sc_time((double)mSystemClock,glb_resolution_unit);
      }
      if( mrFirstDataLatencyCycle == 0 ){
	mrFirstDataLatencyUint64 = 0;
	mrFirstDataLatencyTime   = SC_ZERO_TIME;
      }
      else{
	mrFirstDataLatencyUint64 = mSystemClock;
	mrFirstDataLatencyTime   = 
	  sc_time((double)mSystemClock,glb_resolution_unit);
      }
      if( mwFirstDataLatencyCycle == 0 ){
	mwFirstDataLatencyUint64 = 0;
	mwFirstDataLatencyTime   = SC_ZERO_TIME;
      }
      else{
	mwFirstDataLatencyUint64 = mSystemClock;
	mwFirstDataLatencyTime   = 
	  sc_time((double)mSystemClock,glb_resolution_unit);
      }
      if( mrNextDataLatencyCycle == 0 ){
	mrNextDataLatencyUint64  = 0;
	mrNextDataLatencyTime    = SC_ZERO_TIME;
      }
      else{
	mrNextDataLatencyUint64  = mSystemClock;
	mrNextDataLatencyTime    = 
	  sc_time((double)mSystemClock,glb_resolution_unit);
      }
      if( mwNextDataLatencyCycle == 0 ){
	mwNextDataLatencyUint64  = 0;
	mwNextDataLatencyTime    = SC_ZERO_TIME;
      }
      else{
	mwNextDataLatencyUint64  = mSystemClock;
	mwNextDataLatencyTime    = 
	  sc_time((double)mSystemClock,glb_resolution_unit);
      }
    }
    else{
      rIL  = mrInitLatencyCycle+mAddRInitLatencyCycle;
      wIL  = mwInitLatencyCycle+mAddWInitLatencyCycle;
      rFDL = mrFirstDataLatencyCycle;
      rNDL = mrNextDataLatencyCycle;
      if( mBusProtocol == BUS_AXI ){
	wFDL = mwFirstDataLatencyCycle+mAddWFirstDataLatencyCycle;
	wNDL = mwNextDataLatencyCycle+mAddWNextDataLatencyCycle;
      }
      else{
	wFDL = mwFirstDataLatencyCycle;
	wNDL = mwNextDataLatencyCycle;
      }
      mrInitLatencyTime      = (double)rIL *
	sc_time((double)mSystemClock,glb_resolution_unit);
      mwInitLatencyTime      = (double)wIL *
	sc_time((double)mSystemClock,glb_resolution_unit);
      mrFirstDataLatencyTime = (double)rFDL *
	sc_time((double)mSystemClock,glb_resolution_unit);
      mwFirstDataLatencyTime = (double)wFDL *
	sc_time((double)mSystemClock,glb_resolution_unit);
      mrNextDataLatencyTime  = (double)rNDL *
	sc_time((double)mSystemClock,glb_resolution_unit);
      mwNextDataLatencyTime  = (double)wNDL *
	sc_time((double)mSystemClock,glb_resolution_unit);
      mrInitLatencyUint64      = rIL  * mSystemClock;
      mwInitLatencyUint64      = wIL  * mSystemClock;
      mrFirstDataLatencyUint64 = rFDL * mSystemClock;
      mwFirstDataLatencyUint64 = wFDL * mSystemClock;
      mrNextDataLatencyUint64  = rNDL * mSystemClock;
      mwNextDataLatencyUint64  = wNDL * mSystemClock;
    }

    unsigned int beatNumber;
    mLtLatency[tlm::TLM_READ_COMMAND][0]     = SC_ZERO_TIME;
    mLtLatency[tlm::TLM_WRITE_COMMAND][0]    = SC_ZERO_TIME;
    mLtLatency[tlm::TLM_IGNORE_COMMAND][0]   = SC_ZERO_TIME;
    mAtFwLatency[tlm::TLM_READ_COMMAND][0]   = SC_ZERO_TIME;
    mAtFwLatency[tlm::TLM_WRITE_COMMAND][0]  = SC_ZERO_TIME;
    mAtFwLatency[tlm::TLM_IGNORE_COMMAND][0] = SC_ZERO_TIME;
    mAtBwLatency[tlm::TLM_READ_COMMAND][0]   = SC_ZERO_TIME;
    mAtBwLatency[tlm::TLM_WRITE_COMMAND][0]  = SC_ZERO_TIME;
    mAtBwLatency[tlm::TLM_IGNORE_COMMAND][0] = SC_ZERO_TIME;
    for( unsigned int i=1; i<9; i++ ){
      if( i > mBusWidthInByte ){
	beatNumber = i / mBusWidthInByte;
	if( i % mBusWidthInByte ){
	  beatNumber += 1;
	}
      }
      else{
	beatNumber = 1;
      }
      mLtLatency[tlm::TLM_READ_COMMAND][i]
	= mrInitLatencyTime + mrFirstDataLatencyTime + 
	  mrNextDataLatencyTime * (beatNumber-1) +
	sc_time( (double)(mApplyLatency*beatNumber*mSystemClock),
		   glb_resolution_unit );
      mAtFwLatency[tlm::TLM_READ_COMMAND][i] 
	= mrInitLatencyTime;
      mAtBwLatency[tlm::TLM_READ_COMMAND][i] 
	= mrFirstDataLatencyTime + 
	  mrNextDataLatencyTime * (beatNumber-1);
      if( mBusProtocol == BUS_AXI ){
	mLtLatency[tlm::TLM_WRITE_COMMAND][i]
	  = mwInitLatencyTime + mwFirstDataLatencyTime + 
	    mwNextDataLatencyTime * (beatNumber-1) + 
	  sc_time( (double)(mApplyLatency*mSystemClock), 
		     glb_resolution_unit );
	mAtFwLatency[tlm::TLM_WRITE_COMMAND][i] 
	  = mwInitLatencyTime + mwFirstDataLatencyTime +
	    mwNextDataLatencyTime * (beatNumber-1);
	mAtBwLatency[tlm::TLM_WRITE_COMMAND][i] 
	  = SC_ZERO_TIME;
      }
      else{
	mLtLatency[tlm::TLM_WRITE_COMMAND][i]
	  = mwInitLatencyTime + mwFirstDataLatencyTime + 
	    mwNextDataLatencyTime * (beatNumber-1) +
	  sc_time( (double)(mApplyLatency*beatNumber*mSystemClock),
		     glb_resolution_unit );
	mAtFwLatency[tlm::TLM_WRITE_COMMAND][i] 
	  = mwInitLatencyTime;
	mAtBwLatency[tlm::TLM_WRITE_COMMAND][i] 
	  = mwFirstDataLatencyTime + 
	    mwNextDataLatencyTime * (beatNumber-1);
      }
      mLtLatency[tlm::TLM_IGNORE_COMMAND][i]   = SC_ZERO_TIME;
      mAtFwLatency[tlm::TLM_IGNORE_COMMAND][i] = SC_ZERO_TIME;
      mAtBwLatency[tlm::TLM_IGNORE_COMMAND][i] = SC_ZERO_TIME;
    }

  }

  /// <summary>calculate forward pass latency</summary>
  /// <param name="trans">transaction</param>
  sc_time calFwLatency( TlmTransactionType &trans )
  {
    uint64 latencyInUint64     = calInitLatency( trans );
    if( mBusProtocol==BUS_AXI && trans.is_write() ){
      latencyInUint64 += calDataLatency( trans );
    }
    sc_time latencyInScTime = sc_time( (double)latencyInUint64, 
				       glb_resolution_unit );
    return( latencyInScTime );
  }

  /// <summary>calculate backward pass latency</summary>
  /// <param name="trans">transaction</param>
  sc_time calBwLatency( TlmTransactionType &trans )
  {
    if( mBusProtocol!=BUS_AXI || trans.is_read() ){
      return( sc_time((double)calDataLatency( trans ), 
		      glb_resolution_unit) );
    }
    return( SC_ZERO_TIME );
  }

  /// <summary>read response preprocess</summary>
  void rRespPreprocess()
  {
    TlmTransactionType *trans;

    while ((trans = mRRespPreprocessPEQ.get_next_transaction()) != 0) {
#ifdef BUSIF_TST
      if(tstFp){
        fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
        fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
        fprintf(tstFp,"BusSlaveIf::rRespPreprocess() invoked\n");
        outputLogAtResp( trans );
      }
#endif //BUSIF_TST
      mpRTransBwFifo->push( trans );
      if( !(*mpRWaitBwTrans) ){
        meRRespEvent.notify();
      }
    }
  }

  /// <summary>write response preprocess</summary>
  void wRespPreprocess()
  {
    TlmTransactionType *trans;

    while ((trans = mWRespPreprocessPEQ.get_next_transaction()) != 0) {
#ifdef BUSIF_TST
      if(tstFp){
        fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
        fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
        fprintf(tstFp,"BusSlaveIf::wRespPreprocess() invoked\n",
	        targetSocket.name());
        outputLogAtResp( trans );
      }
#endif //BUSIF_TST
      mpWTransBwFifo->push( trans );
      if( !(*mpWWaitBwTrans) ){
        meWRespEvent.notify();
      }
    }
  }

  /// <summary>read response process</summary>
  void rRespProcess( void )
  {
    if( !mpRTransBwFifo->empty() ){
      TlmPhase phase = tlm::BEGIN_RESP;
      TlmTransactionType *trans = mpRTransBwFifo->front();
      mpRTransBwFifo->pop();
      sc_time t;
      if( trans->get_data_length() <= 8 ){
	t = mAtBwLatency[tlm::TLM_READ_COMMAND][trans->get_data_length()];
      }
      else{
	t = calBwLatency( *trans );
      }
      trans->set_response_status(tlm::TLM_OK_RESPONSE);
#ifdef BUSIF_TST
      if(tstFp){
	fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	fprintf(tstFp,"BusSlaveIf::Call nb_transport_bw() from %s\n",
		targetSocket.name());
	outputLogAtResp( trans );
	if( phase == tlm::BEGIN_RESP ){
	  fprintf(tstFp,"             phase =tlm::BEGIN_RESP\n");
	}
	else{
	  fprintf(tstFp,"             phase =other\n");
	}
	fprintf(tstFp,"        t =%s\n",
		t.to_string().c_str());
      }
#endif //BUSIF_TST
      targetSocket->nb_transport_bw( *trans, phase, t );
      meRRespEvent.notify( t );
      *mpRWaitBwTrans = true;
      mTransNmb--;
    }
    else{
      *mpRWaitBwTrans = false;
    }
  }
  
  /// <summary>write response process</summary>
  void wRespProcess( void )
  {
    if( !mpWTransBwFifo->empty() ){
      TlmPhase phase = tlm::BEGIN_RESP;
      TlmTransactionType *trans = mpWTransBwFifo->front();
      mpWTransBwFifo->pop();
      sc_time t;
      if( trans->get_data_length() <= 8 ){
	t = mAtBwLatency[tlm::TLM_WRITE_COMMAND][trans->get_data_length()];
      }
      else{
	t = calBwLatency( *trans );
      }
      trans->set_response_status(tlm::TLM_OK_RESPONSE);
#ifdef BUSIF_TST
      if(tstFp){
	fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	fprintf(tstFp,"BusSlaveIf::Call nb_transport_bw() from %s\n",
		targetSocket.name());
	outputLogAtResp( trans );
	if( phase == tlm::BEGIN_RESP ){
	  fprintf(tstFp,"             phase =tlm::BEGIN_RESP\n");
	}
	else{
	  fprintf(tstFp,"             phase =other\n");
	}
	fprintf(tstFp,"        t =%s\n",
		t.to_string().c_str());
      }
#endif //BUSIF_TST
      targetSocket->nb_transport_bw( *trans, phase, t );
      meWRespEvent.notify( t );
      *mpWWaitBwTrans = true;
      mTransNmb--;
    }
    else{
      *mpWWaitBwTrans = false;
    }
  }
  
  /// <summary>read function calling process</summary>
  void readProcess( void )
  {
#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
      fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"BusSlaveIf::readProcess is called\n");
    }
#endif //BUSIF_TST
    transInfo_t transInfo = mpRTransInfoFwFifo->front();
    mpRTransInfoFwFifo->pop();

    TlmTransactionType *trans = transInfo.trans;
    sc_time t = SC_ZERO_TIME;

    unsigned int offsetAddress = 
      (unsigned int)(trans->get_address()-targetSocket.getBaseAddress());

    mpFuncModule->read( offsetAddress,
			*trans,
			&t );

    BusAccessBreak::checkHitBreakAddress( *trans );

    notifyRespPreEvent( *trans, t );

    if( !mpRTransInfoFwFifo->empty() ){
      transInfo = mpRTransInfoFwFifo->front();
      sc_time delay;
      if( mPipeLineOp ){
	delay = pipeLineOperation( transInfo );
      }
      else{
	delay = transInfo.t;
      }
      if( transInfo.trans->is_read() ){
	meReadEvent.notify( delay );
      }
      else{
	meWriteEvent.notify( delay );
      }
    }
    else{
      *mpRWaitFwTrans = false;
    }
  }
  
  /// <summary>write function calling process</summary>
  void writeProcess( void )
  {
#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
      fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"BusSlaveIf::writeProcess is called\n");
    }
#endif //BUSIF_TST
    transInfo_t transInfo = mpWTransInfoFwFifo->front();
    mpWTransInfoFwFifo->pop();

    TlmTransactionType *trans = transInfo.trans;
    sc_time t = SC_ZERO_TIME;

    unsigned int offsetAddress = 
      (unsigned int)(trans->get_address()-targetSocket.getBaseAddress());

    mpFuncModule->write( offsetAddress,
			 *trans,
			 &t );

    BusAccessBreak::checkHitBreakAddress( *trans );

    notifyRespPreEvent( *trans, t );

    if( !mpWTransInfoFwFifo->empty() ){
      transInfo = mpWTransInfoFwFifo->front();
      sc_time delay;
      if( mPipeLineOp ){
	delay = pipeLineOperation( transInfo );
      }
      else{
	delay = transInfo.t;
      }
      if( transInfo.trans->is_read() ){
	meReadEvent.notify( delay );
      }
      else{
	meWriteEvent.notify( delay );
      }
    }
    else{
      *mpWWaitFwTrans = false;
    }
  }
  
  /// <summary>check address</summary>
  /// <param name="trans">transaction</param>
  /// <returns>address</returns>
  virtual unsigned int checkAddress( TlmTransactionType &trans )
  {
    unsigned int offsetAddress = 
      (unsigned int)(trans.get_address()-targetSocket.getBaseAddress());
    if( offsetAddress >= targetSocket.getBaseSize() ){
      char errorMessageBuf[1024];
      sprintf( errorMessageBuf,
	       "Received the transaction to wrong address[0x%llx].",
	       trans.get_address() );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    }
    return ( offsetAddress );
  }
  
  /// <summary>check data size</summary>
  /// <param name="trans">transaction</param>
  void checkDataSize( TlmTransactionType &trans )
  {
    if( trans.get_data_length() > mBusWidthInByte ){
      char errorMessageBuf[1024];
      sprintf( errorMessageBuf,
	       "Data size[%d] was greater than Bus width[%d].",
	       trans.get_data_length(), mBusWidthInByte );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    }
  }
  
public:
  /// <summary>notify response preprocess event</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  virtual void notifyRespPreEvent( TlmTransactionType &trans, sc_time &t )
  {
    if( trans.is_read() ){
      mRRespPreprocessPEQ.notify(trans, t);
    }
    else{
      mWRespPreprocessPEQ.notify(trans, t);
    }
  }
  
private:
  /// <summary>calculate initial latency</summary>
  /// <param name="trans">transaction</param>
  /// <returns>latency</returns>
  uint64 calInitLatency( TlmTransactionType &trans )
  {

    /***** Set the read/write latency ****/
    uint64 latency;

    if( trans.get_command() == tlm::TLM_WRITE_COMMAND ){
      latency = mwInitLatencyUint64;
    }
    else{
      latency = mrInitLatencyUint64;
    }

    return ( latency );

  }      
  
  /// <summary>calculate completion reception latency</summary>
  /// <param name="trans">transaction</param>
  /// <returns>latency</returns>
  uint64 calCompRecepLatency( TlmTransactionType &trans )
  {
    if( mBusProtocol==BUS_AXI && trans.is_write() ){
      return( mApplyLatency*mSystemClock );
    }
    else{
      unsigned int data_num = trans.get_data_length()/mBusWidthInByte;
      unsigned int rest = trans.get_data_length()%mBusWidthInByte;
      if( rest ){
	data_num++;
      }
      return( mApplyLatency*mSystemClock*data_num );
    }
  }
  
  /// <summary>calculate time to clock edge</summary>
  /// <param name="trans">transaction</param>
  /// <returns>time to clock edge</returns>
  sc_time calTimeToEdge( sc_time& t )
  {
    /***** Calculate the time to next clock edge *****/
    uint64 timeStamp = sc_time_stamp().value()+t.value();
    uint64 timeToEdge = 
      (timeStamp/mSystemClock+(uint64)1)*mSystemClock - timeStamp;//TBC

    if( timeToEdge == mSystemClock ){
      timeToEdge = 0;
    }

    return ( sc_time( ((double)t.value() + timeToEdge), glb_resolution_unit ) );
  }

  /// <summary>pipe line operation</summary>
  /// <param name="trans">transaction</param>
  /// <returns>time to clock edge</returns>
  sc_time pipeLineOperation( transInfo_t &transInfo )
  {
    uint64 t = transInfo.t.value();
    if( sc_time_stamp().value() > transInfo.receiveT.value() ){
      return( sc_time((double)(t-mSystemClock),glb_resolution_unit) );
    }
    else{
      return( transInfo.t );
    }
  }

  /// <summary>set TlmG3mExtension if there is not</summary>
  /// <param name="trans">transaction</param>
  void setG3mExtension( TlmTransactionType &trans )
  {
    TlmG3mExtension *g3mExt = (TlmG3mExtension *)0;
    trans.get_extension( g3mExt );
    if( !g3mExt ){
      g3mExt = new TlmG3mExtension;
      trans.set_extension( g3mExt );
      g3mExt->setPeId( 0xFF );
      g3mExt->setSecure( true );
      mIsG3mExtensionSet = true;
    }
    else{
      //nothing to do
    }
  }

  /// <summary>release TlmG3mExtension if set</summary>
  /// <param name="trans">transaction</param>
  void releaseG3mExtension( TlmTransactionType &trans )
  {
    if( mIsG3mExtensionSet ){
      TlmG3mExtension *g3mExt = (TlmG3mExtension *)0;
      trans.get_extension( g3mExt );
      trans.release_extension( g3mExt );
      mIsG3mExtensionSet = false;
#ifdef BUSIF_TST
      if(tstFp){
	fprintf(tstFp,"        TlmG3mExtension was released\n");
      }
#endif //BUSIF_TST
    }
    else{
      //nothing to do
    }
  }


#if defined(BUSIF_TST)
public:
#if defined(PLTFRM_TST)
  /// <summary>end of elaboration</summary>
  void end_of_elaboration( void )
  {
    tstFp = stdout;
  }
#endif //PLTFRM_TST

  /// <summary>set log file pointer</summary>
  /// <param name="fp">file pointer</param>
  void setLogFileP( FILE *fp )
  {
    tstFp = fp;
  }

#if 0 // for export to BusBridgeSlaveIf
private:
#endif// for export to BusBridgeSlaveIf
  /// <summary>output logs at reception</summary>
  /// <param name="trans">transactionn</param>
  void outputLogAtRecep( TlmTransactionType *trans )
  {
    fprintf(tstFp,"        transaction pointer=0x%llx\n",
	    trans);
    if( trans->get_command() == tlm::TLM_READ_COMMAND ){
      fprintf(tstFp,"        TLM_READ_COMMAND\n");
    }
    else{
      fprintf(tstFp,"        TLM_WRITE_COMMAND\n");
    }
    fprintf(tstFp,"        address     =0x%llx\n",
	    trans->get_address());
    int data_length = trans->get_data_length();
    fprintf(tstFp,"        data_length =0x%x\n",
	    data_length);
    unsigned char *data = trans->get_data_ptr();
    for(int i=0; i<data_length; i++){
      fprintf(tstFp,"        data[%d]     =0x%x\n",
	      i,data[i]);
    }
    fprintf(tstFp,"        stream_width=%d\n",
	    trans->get_streaming_width());
    if(trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE ){
      fprintf(tstFp,"        status      =tlm::TLM_INCOMPLETE_RESPONSE\n" );
    }
    else{
      fprintf(tstFp,"        status      =other\n" );
    }
    TlmG3mExtension *g3mExt = (TlmG3mExtension *)0;
    trans->get_extension( g3mExt );
    if( g3mExt ){
      if( g3mExt->isDmaAccess() ){
	fprintf(tstFp,"        G3M extension::DMA access\n");
      }
      else{
	fprintf(tstFp,"        G3M extension::not DMA access\n");
      }
      fprintf(tstFp,"                       TC ID     =0x%x\n",
	      g3mExt->getTcId());
      fprintf(tstFp,"                       VM ID     =0x%x\n",
	      g3mExt->getVmId());
      fprintf(tstFp,"                       PE ID     =0x%x\n",
	      g3mExt->getPeId());
      fprintf(tstFp,"                       SP ID     =0x%x\n",
	      g3mExt->getSpId());
      if( g3mExt->isUserMode() ){
	fprintf(tstFp,"                       User mode\n");
      }
      else{
	fprintf(tstFp,"                       Supervisor mode\n");
      }
      if( g3mExt->isVirtualMode() ){
	fprintf(tstFp,"                       Virtual mode\n");
      }
      else{
	fprintf(tstFp,"                       Native mode\n");
      }
      if( g3mExt->isSpcLd() ){
	fprintf(tstFp,"                       Speculative load\n");
      }
      else{
	fprintf(tstFp,"                       non-Speculative load\n");
      }
      if( g3mExt->isSecure() ){
	fprintf(tstFp,"                       Secure\n");
      }
      else{
	fprintf(tstFp,"                       non-Secure\n");
      }
      fprintf(tstFp,"                       IfPointer =0x%llx\n",
	      g3mExt->getIfPointer());
    }
    TlmVciExtension *vciExt = (TlmVciExtension *)0;
    trans->get_extension( vciExt );
    if( vciExt ){
      if( vciExt->getRequestType() == TlmVciExtension::CMD_READ ){
	fprintf(tstFp,"        VCI extension::Read command\n");
      }
      else if( vciExt->getRequestType() == TlmVciExtension::CMD_WRITE ){
	fprintf(tstFp,"        VCI extension::Write command\n");
      }
      else if( vciExt->getRequestType() == TlmVciExtension::CMD_BITOP_SET ){
	fprintf(tstFp,"        VCI extension::BitOp command(set)\n");
      }
      else if( vciExt->getRequestType() == TlmVciExtension::CMD_BITOP_AND ){
	fprintf(tstFp,"        VCI extension::BitOp command(and)\n");
      }
      else if( vciExt->getRequestType() == TlmVciExtension::CMD_BITOP_OR ){
	fprintf(tstFp,"        VCI extension::BitOp command(or)\n");
      }
      else if( vciExt->getRequestType() == TlmVciExtension::CMD_BITOP_XOR ){
	fprintf(tstFp,"        VCI extension::BitOp command(xor)\n");
      }
      else if( vciExt->getRequestType() == TlmVciExtension::CMD_CAXI ){
	fprintf(tstFp,"        VCI extension::CAXI\n");
      }
      else if( vciExt->getRequestType() == TlmVciExtension::CMD_EXCL_LDLW ){
	fprintf(tstFp,"        VCI extension::Exclusive command(LDL.W)\n");
      }
      else if( vciExt->getRequestType() == TlmVciExtension::CMD_EXCL_STCW ){
	fprintf(tstFp,"        VCI extension::Exclusive command(STC.W)\n");
      }
    }
    TlmAxiExtension *axiExt = (TlmAxiExtension *)0;
    trans->get_extension( axiExt );
    if( axiExt ){
      if( axiExt->getBurstType() == TlmAxiExtension::BURST_FIXED ){
	fprintf(tstFp,"        AXI extension::BURST_FIXED\n");
      }
      else if( axiExt->getBurstType() == TlmAxiExtension::BURST_INCR ){
	fprintf(tstFp,"        AXI extension::BURST_INCR\n");
      }
      else if( axiExt->getBurstType() == TlmAxiExtension::BURST_WRAP ){
	fprintf(tstFp,"        AXI extension::BURST_WRAP\n");
      }
      if( axiExt->isLock() ){
	fprintf(tstFp,"                       Lock\n");
      }
      else{
	fprintf(tstFp,"                       Not lock\n");
      }
      if( axiExt->isCachable() ){
	fprintf(tstFp,"                       Cachable\n");
      }
      else{
	fprintf(tstFp,"                       Not cachable\n");
      }
      if( axiExt->isBufferable() ){
	fprintf(tstFp,"                       Bufferable\n");
      }
      else{
	fprintf(tstFp,"                       Not bufferable\n");
      }
      fprintf(tstFp,"                       secure information =0x%x\n",
	      axiExt->getSecure());
      fprintf(tstFp,"                       transaction ID     =0x%x\n",
	      axiExt->getTransId());
      if( axiExt->getBitOpType() == TlmAxiExtension::BITOP_NON ){
	fprintf(tstFp,"                       BITOP_NON\n");
      }
      else if( axiExt->getBitOpType() == TlmAxiExtension::BITOP_SET1 ){
	fprintf(tstFp,"                       BITOP_SET1\n");
      }
      else if( axiExt->getBitOpType() == TlmAxiExtension::BITOP_CLR1 ){
	fprintf(tstFp,"                       BITOP_CLR1\n");
      }
      else if( axiExt->getBitOpType() == TlmAxiExtension::BITOP_NOT1 ){
	fprintf(tstFp,"                       BITOP_NOT1\n");
      }
      fprintf(tstFp,"                       Bit op position =0x%x\n",
	      axiExt->getBitOpPos());
    }
    TlmVpiExtension *vpiExt = (TlmVpiExtension *)0;
    trans->get_extension( vpiExt );
    if( vpiExt ){
      fprintf(tstFp,"        VPI extension::Master ID =0x%x\n",
	      vpiExt->getMasterId());
      fprintf(tstFp,"                       Packet ID =0x%x\n",
	      vpiExt->getPacketId());
      fprintf(tstFp,"                       Slave ID  =0x%x\n",
	      vpiExt->getSlaveId());
      if( vpiExt->getRequestType() == TlmVpiExtension::CMD_READ ){
	fprintf(tstFp,"                       Read command\n");
      }
      else if( vpiExt->getRequestType() == TlmVpiExtension::CMD_WRITE ){
	fprintf(tstFp,"                       Write command\n");
      }
      else if( vpiExt->getRequestType() == TlmVpiExtension::CMD_BITOP_SET ){
	fprintf(tstFp,"                       BitOp command(set)\n");
      }
      else if( vpiExt->getRequestType() == TlmVpiExtension::CMD_BITOP_AND ){
	fprintf(tstFp,"                       BitOp command(and)\n");
      }
      else if( vpiExt->getRequestType() == TlmVpiExtension::CMD_BITOP_OR ){
	fprintf(tstFp,"                       BitOp command(or)\n");
      }
      else if( vpiExt->getRequestType() == TlmVpiExtension::CMD_BITOP_XOR ){
	fprintf(tstFp,"                       BitOp command(xor)\n");
      }
      else if( vpiExt->getRequestType() == TlmVpiExtension::CMD_SYNC_BUS ){
	fprintf(tstFp,"                       Sync command(bus)\n");
      }
      else if( vpiExt->getRequestType() == TlmVpiExtension::CMD_SYNC_MASTER ){
	fprintf(tstFp,"                       Sync command(master)\n");
      }
    }
    TlmAhbExtension *ahbExt = (TlmAhbExtension *)0;
    trans->get_extension( ahbExt );
    if( ahbExt ){
      if( ahbExt->getBurstType() == TlmAhbExtension::BURST_FIXED ){
	fprintf(tstFp,"        AHB extension::BURST_FIXED\n");
      }
      else if( ahbExt->getBurstType() == TlmAhbExtension::BURST_INCR ){
	fprintf(tstFp,"        AHB extension::BURST_INCR\n");
      }
      else if( ahbExt->getBurstType() == TlmAhbExtension::BURST_WRAP ){
	fprintf(tstFp,"        AHB extension::BURST_WRAP\n");
      }
      if( ahbExt->isLock() ){
	fprintf(tstFp,"                       Lock\n");
      }
      else{
	fprintf(tstFp,"                       Not lock\n");
      }
      if( ahbExt->isCachable() ){
	fprintf(tstFp,"                       Cachable\n");
      }
      else{
	fprintf(tstFp,"                       Not cachable\n");
      }
      if( ahbExt->isBufferable() ){
	fprintf(tstFp,"                       Bufferable\n");
      }
      else{
	fprintf(tstFp,"                       Not bufferable\n");
      }
      if( ahbExt->isPrivilege() ){
	fprintf(tstFp,"                       Privilege\n");
      }
      else{
	fprintf(tstFp,"                       Not Privilege\n");
      }
      if( ahbExt->getDataOrOp() == TlmAhbExtension::ACC_DATA ){
	fprintf(tstFp,"                       access to Data\n");
      }
      else if( ahbExt->getDataOrOp() == TlmAhbExtension::ACC_OP_CODE ){
	fprintf(tstFp,"                       access to OP code\n");
      }
    }
    TlmApbExtension *apbExt = (TlmApbExtension *)0;
    trans->get_extension( apbExt );
    if( apbExt ){
      if( apbExt->isLock() ){
	fprintf(tstFp,"        APB extension::Lock\n");
      }
      else{
	fprintf(tstFp,"        APB extension::Not lock\n");
      }
      if( apbExt->isExclusion() ){
	fprintf(tstFp,"                       Exclusion\n");
      }
      else{
	fprintf(tstFp,"                       Not exclusion\n");
      }
    }
  }

  /// <summary>output logs at response</summary>
  /// <param name="trans">transaction</param>
  void outputLogAtResp( TlmTransactionType *trans )
  {
    fprintf(tstFp,"        transaction pointer=0x%llx\n",
	    trans);
    if( trans->get_command() == tlm::TLM_READ_COMMAND ){
      fprintf(tstFp,"        TLM_READ_COMMAND\n");
    }
    else{
      fprintf(tstFp,"        TLM_WRITE_COMMAND\n");
    }
    fprintf(tstFp,"        address     =0x%llx\n",
	    trans->get_address());
    int data_length = trans->get_data_length();
    fprintf(tstFp,"        data_length =0x%x\n",
	    data_length);
    unsigned char *data = trans->get_data_ptr();
    for(int i=0; i<data_length; i++){
      fprintf(tstFp,"        data[%d]     =0x%x\n",
	      i,data[i]);
    }
    if(trans->get_response_status() == tlm::TLM_OK_RESPONSE ){
      fprintf(tstFp,"        status      =tlm::TLM_OK_RESPONSE\n" );
    }
    else{
      fprintf(tstFp,"        status      =other\n" );
    }
  }

  FILE *tstFp; //test log file pointer

#endif //BUSIF_TST

};

#endif //__BUSSLAVEIF__
