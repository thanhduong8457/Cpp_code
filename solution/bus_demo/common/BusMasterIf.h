/*
* Copyright(c) 2011-2017 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*/

#ifndef __BUSMASTERIF_H__
#define __BUSMASTERIF_H__

#include "global.h"
#include "OSCI2.h"     // class for User Modeling environment based on TLM2.0
#include "CmErrMsg.h"  // for error message
#include "BusMasterFuncIf.h"
#include <vector>
#include <queue>

#define BUS_MASTER_IF_DEFAULT_ID (unsigned int)0xFFFFFFFF
#define BUS_MASTER_IF_ERROR_ID   (unsigned int)0xFFFFFFFF

extern sc_time_unit glb_resolution_unit;

template<unsigned int BUSWIDTH>
class BusMasterIf :
  public BusAccessBreak,
  public sc_module,
  public virtual TlmBwTransportIf
{
 private:

  // TLM coding style
  typedef enum
  {
    AT  = 0,
    LT  = 1,
    DMI = 2
  } TransMode_t;

  // bus request
  struct BusRequest{
    TlmTransactionType    *transaction;
    sc_time                t;
    sc_event              *epTlmCompleted;
    bool                   tlmIncompleted;

    /// <summary>constructor</summary>
    BusRequest(void) :
      transaction( (TlmTransactionType *)0 ),
      t( SC_ZERO_TIME ),
      tlmIncompleted( false )
    {
      epTlmCompleted = new sc_event();
    }

    /// <summary>destructor</summary>
    ~BusRequest(void){}
  };

  // bus request list
  class BusRequestList{
  public:
    /// <summary>constructor</summary>
    BusRequestList( TlmInitiatorSocket<BUSWIDTH> &iSocket ) :
      mpFuncModule( (BusMasterFuncIf *)0 ),
      mEndReqId( (unsigned int)0 )
    {
    }

    /// <summary>destructor</summary>
    ~BusRequestList( void ){}

    /// <summary>register bus request list</summary>
    /// <param name="trans">transation</param>
    /// <param name="index">index</param>
    void registerList( TlmTransactionType &trans,
		       unsigned int index )
    {
      // transaction setting
      if( index == mEndReqId ){
	BusRequest dummy;
	mRequestList.push_back( dummy );
	mEndReqId++;
      }

      mRequestList[index].transaction = &trans;
      mRequestList[index].tlmIncompleted = true;

    }

    /// <summary>get request ID</summary>
    /// <param name="trans">transaction</param>
    unsigned int 
    getRequestId( TlmTransactionType &trans )
    {
#if 0 // include transaction object error checking
      unsigned int return_id = BUS_MASTER_IF_DEFAULT_ID;
      for( unsigned int id=0; id<mEndReqId; id++ ){
	if( !mRequestList[id].tlmIncompleted ){
	  if( return_id == BUS_MASTER_IF_DEFAULT_ID ){
	    return_id = id;
	  }
	}
	else{
	  if( &trans == mRequestList[id].transaction ){
	    return BUS_MASTER_IF_ERROR_ID;
	  }
	}
      }
      if( return_id == BUS_MASTER_IF_DEFAULT_ID ){
	return mEndReqId;
      }
      return return_id;
#else // include transaction object error checking
      for( unsigned int id=0; id<mEndReqId; id++ ){
	if( !mRequestList[id].tlmIncompleted ){
	  return id;
	}
      }
      return mEndReqId;
#endif// include transaction object error checking
    }

    /// <summary>notify transaction completion</summary>
    /// <param name="trans">transation</param>
    /// <param name="t">time</param>
    bool notifyTlmCompleted( TlmTransactionType &trans, sc_time &t )
    {
      for( unsigned int id=0; id<mEndReqId; id++ ){

	if( &trans == mRequestList[id].transaction &&
	    mRequestList[id].tlmIncompleted ){
	  mRequestList[id].epTlmCompleted->notify(t);
	  mRequestList[id].tlmIncompleted = false;
	  mpFuncModule->notifyTransComp( trans, t );
	  return true;
	}
      }
      return false;
    }

    /// <summary>reset</summary>
    void reset( void )
    {
      for( unsigned int i=0; i<mEndReqId; i++ ){
	mRequestList[i].tlmIncompleted = false;
      }
    }
    
    /// <summary>set core function pointer</summary>
    /// <param name="pFuncModule">function pointer</param>
    void setFuncModulePtr( BusMasterFuncIf *pFuncModule )
    {
      mpFuncModule = pFuncModule;
    }
  
  public:
    std::vector< BusRequest > mRequestList; //vector of request list
    BusMasterFuncIf     *mpFuncModule; //bus masterfunction I/F
  private:
    unsigned int         mEndReqId; //end request ID
  };

 public:
  SC_HAS_PROCESS(BusMasterIf);
  /// <summary>constructor</summary>
  BusMasterIf( sc_module_name               name,
	       sc_in<bool>                  &resetPort,
	       sc_in<uint64>                &freqPort,
	       TlmInitiatorSocket<BUSWIDTH> &iSocket ):
    sc_module( name ),
    mResetPort( resetPort ),
    mFreqPort( freqPort ),
    initiatorSocket( iSocket ),
    mSystemClock( (uint64)0 ),
    mAtRequestList( iSocket ),
    mWaitTrans( false ),
    meTlmCompleted(),
    meTransEvent(),
    mTransType( (TransMode_t)AT ),
    mDisableTransport( false ),
    mTransNmb( (unsigned int)0 ),
    mTransSendNmb( (unsigned int)0 ),
    mTransSendNmbLmt( (unsigned int)1 ),
    mCompRecepLatencyCycle( (uint64)1 ),
    mCompRecepLatencyUint64( (uint64)1 ),
    mCompRecepLatencyTime( SC_ZERO_TIME ),
    mBusProtocol( BUS_APB ),
    mBusWidthInByte( BUSWIDTH/8 ),
    mPipeLineOpFtoF( false ),
    mPipeLineOpBtoF( false )
#ifdef BUSIF_TST
    ,tstFp( (FILE *)0 )
#endif //BUSIF_TST
  {
    SC_METHOD(resetProcess);
    sensitive << mResetPort.neg();
    dont_initialize();

    SC_METHOD( calPeriodProcess );
    sensitive << mFreqPort;
    dont_initialize();

    SC_METHOD( transportProcess );
    sensitive << meTransEvent;
    dont_initialize();

  }

  /// <summary>destructor</summary>
  ~BusMasterIf(void){}

  /// <summary>read function</summary>
  bool read( TlmTransactionType &trans,
	     bool                debug=false,
	     BusTime_t          &t=*((BusTime_t *)0),
	     bool                withWait=false )
  {
    unsigned int index;
    
    trans.set_command( tlm::TLM_READ_COMMAND );
    if (trans.get_streaming_width() == 0) {
      trans.set_streaming_width(trans.get_data_length());
    }
    trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

    if( debug ){
#if defined(BUSIF_TST)
	if(tstFp){
	  fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	  fprintf(tstFp,"BusMasterIf::Call transport_dbg() from %s\n",
		  initiatorSocket.name());
	  outputLogAtTrans( &trans );
	}
#endif //BUSIF_TST
#if 0
      if( initiatorSocket->transport_dbg( trans ) !=
	  trans.get_data_length() ){
	char errorMessageBuf[1024];
	sprintf( errorMessageBuf,
		 "Data length in payload and return value of transport_dbg are not corresponding." );
	CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      }
#else // 0
      initiatorSocket->transport_dbg( trans );
#endif // 0
#ifdef BUSIF_TST
      if(tstFp){
	fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	fprintf(tstFp,"BusMasterIf::Return from transport_dbg()\n" );
	outputLogAtComp( &trans );
      }
#endif //BUSIF_TST
      return true;
    }
    else{
      TlmG3mExtension *G3mExtension = NULL;
      trans.get_extension( G3mExtension );
      if( G3mExtension == NULL ){
	char errorMessageBuf[1024];
	sprintf( errorMessageBuf,
		 "There is no TlmG3mExtension payload." );
	CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      }
      else{
	G3mExtension->setIfPointer( this );
      }

      if( mTransType == AT ){
	index = mAtRequestList.getRequestId( trans );
#if 0 // include transaction object error checking
	if( index == BUS_MASTER_IF_ERROR_ID ){
	  char errorMessageBuf[1024];
	  sprintf( errorMessageBuf,
		   "Several transactions use same payload object." );
	  CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
	}
#endif// include transaction object error checking
	mAtRequestList.registerList( trans, index );
	if( &t ){
	  return( transportWithFifo( index, withWait, t ) );
	}
	else{
	  BusTime_t tLocal = SC_ZERO_TIME;
	  return( transportWithFifo( index, withWait, tLocal ) );
	}
      }
      else{
#if 0 //*** Bus clock timing is not considered in LT ***
	t = calTimeToEdge( t );
#endif
#if defined(BUSIF_TST)
	if(tstFp){
	  fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	  fprintf(tstFp,"BusMasterIf::Call b_transport() from %s\n",
		  initiatorSocket.name());
	  outputLogAtTrans( &trans );
	  if( &t ){
	    fprintf(tstFp,"        t =%s\n",
		    t.to_string().c_str());
	  }
	  else{
	    fprintf(tstFp,"        t pointer is 0\n");
	  }
	}
#endif //BUSIF_TST

	BusAccessBreak::checkHitBreakAddress( trans );

	if( &t ){
	  initiatorSocket->b_transport( trans, t );
#ifdef BUSIF_TST
	  if(tstFp){
	    fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	    fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	    fprintf(tstFp,"BusMasterIf::Return from b_transport()\n" );
	    outputLogAtComp( &trans );
	    fprintf(tstFp,"        t =%s\n",
		    t.to_string().c_str());
	  }
#endif //BUSIF_TST
	}
	else{
	  BusTime_t tLocal = SC_ZERO_TIME;
	  initiatorSocket->b_transport( trans, tLocal );
#ifdef BUSIF_TST
	  if(tstFp){
	    fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	    fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	    fprintf(tstFp,"BusMasterIf::Return from b_transport()\n" );
	    outputLogAtComp( &trans );
	    fprintf(tstFp,"        t =%s\n",
		    tLocal.to_string().c_str());
	  }
#endif //BUSIF_TST
	}
	checkSlaveResponse( trans );
	return true;
      }
    }
  }

  /// <summary>write function</summary>
  bool write( TlmTransactionType &trans,
	      bool                debug=0,
	      BusTime_t          &t=*((BusTime_t *)0),
	      bool                withWait=0 )
  {
    unsigned int index;
    
    trans.set_command( tlm::TLM_WRITE_COMMAND );
    if (trans.get_streaming_width() == 0) {
      trans.set_streaming_width(trans.get_data_length());
    }
    trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

    if( debug ){
#if defined(BUSIF_TST)
	if(tstFp){
	  fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	  fprintf(tstFp,"BusMasterIf::Call transport_dbg() from %s\n",
		  initiatorSocket.name());
	  outputLogAtTrans( &trans );
	}
#endif //BUSIF_TST
#if 0
      if( initiatorSocket->transport_dbg( trans ) !=
	  trans.get_data_length() ){
	char errorMessageBuf[1024];
	sprintf( errorMessageBuf,
		 "Data length in payload and return value of transport_dbg are not corresponding." );
	CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      }
#else // 0
      initiatorSocket->transport_dbg( trans );
#endif //0
#ifdef BUSIF_TST
      if(tstFp){
	fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	fprintf(tstFp,"BusMasterIf::Return from transport_dbg()\n" );
	outputLogAtComp( &trans );
      }
#endif //BUSIF_TST
      return true;
    }
    else{
      TlmG3mExtension *G3mExtension = NULL;
      trans.get_extension( G3mExtension );
      if( G3mExtension == NULL ){
	char errorMessageBuf[1024];
	sprintf( errorMessageBuf,
		 "There is no TlmG3mExtension payload." );
	CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      }
      else{
	G3mExtension->setIfPointer( this );
      }

      if( mTransType == AT ){
	index = mAtRequestList.getRequestId( trans );
#if 0 // include transaction object error checking
	if( index == BUS_MASTER_IF_ERROR_ID ){
	  char errorMessageBuf[1024];
	  sprintf( errorMessageBuf,
		   "Several transactions use same payload object." );
	  CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
	}
#endif// include transaction object error checking
	mAtRequestList.registerList( trans, index );
	if( &t ){
	  return( transportWithFifo( index, withWait, t ) );
	}
	else{
	  BusTime_t tLocal = SC_ZERO_TIME;
	  return( transportWithFifo( index, withWait, tLocal ) );
	}
      }
      else{
#if 0 //*** Bus clock timing is not considered in LT ***
	t = calTimeToEdge( t );
#endif
#if defined(BUSIF_TST)
	if(tstFp){
	  fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	  fprintf(tstFp,"BusMasterIf::Call b_transport() from %s\n",
		  initiatorSocket.name());
	  outputLogAtTrans( &trans );
	  if( &t ){
	    fprintf(tstFp,"        t =%s\n",
		    t.to_string().c_str());
	  }
	  else{
	    fprintf(tstFp,"        t pointer is 0\n");
	  }
	}
#endif //BUSIF_TST

	BusAccessBreak::checkHitBreakAddress( trans );

	if( &t ){
	  initiatorSocket->b_transport( trans, t );
#ifdef BUSIF_TST
	  if(tstFp){
	    fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	    fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	    fprintf(tstFp,"BusMasterIf::Return from b_transport()\n" );
	    outputLogAtComp( &trans );
	    fprintf(tstFp,"        t =%s\n",
		    t.to_string().c_str());
	  }
#endif //BUSIF_TST
	}
	else{
	  BusTime_t tLocal = SC_ZERO_TIME;
	  initiatorSocket->b_transport( trans, tLocal );
#ifdef BUSIF_TST
	  if(tstFp){
	    fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	    fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	    fprintf(tstFp,"BusMasterIf::Return from b_transport()\n" );
	    outputLogAtComp( &trans );
	    fprintf(tstFp,"        t =%s\n",
		    tLocal.to_string().c_str());
	  }
#endif //BUSIF_TST
	}
	checkSlaveResponse( trans );
	return true;
      }
    }
  }

  /// <summary>transport using FIFO</summary>
  /// <param name="id">ID of request list</param>
  /// <param name="withWait">do wait or do not</param>
  /// <param name="t">time</param>
  /// <returns>if buffer is full</returns>
  bool transportWithFifo( unsigned int id, bool withWait, sc_time t )
  {
    bool isNotFull = true;

    mRequestFifo.push( mAtRequestList.mRequestList[id].transaction );
    mTransNmb++;
    if( mTransSendNmb < mTransSendNmbLmt ){
      if( !mWaitTrans ){
	meTransEvent.notify( calTimeToEdge( t ) );
	mWaitTrans = true;
      }
    }
    if( mTransNmb > mTransSendNmbLmt ){
      isNotFull = false;
    }
    if( withWait ){
      wait( *(mAtRequestList.mRequestList[id].epTlmCompleted) | 
	    mResetPort.negedge_event() );
    }
#if defined(BUSIF_TST)
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      if( isNotFull ){
	fprintf(tstFp,"BusMasterIf::FIFO is not full.\n");
      }
      else{
	fprintf(tstFp,"BusMasterIf::FIFO is full.\n");
      }
    }
#endif //BUSIF_TST
    return isNotFull;
  }

  /// <summary>transport process</summary>
  void transportProcess( void )
  {
    TlmTransactionType *trans;
    TlmPhase            phase = tlm::BEGIN_REQ;
    sc_time             t = SC_ZERO_TIME;

    if( !mRequestFifo.empty() && mTransSendNmb<mTransSendNmbLmt ){
      trans = mRequestFifo.front();
      mRequestFifo.pop();
#if defined(BUSIF_TST)
	if(tstFp){
	  fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	  fprintf(tstFp,"BusMasterIf::Call nb_transport_fw() from %s\n",
		  initiatorSocket.name());
	  outputLogAtTrans( trans );
	}
#endif //BUSIF_TST

	BusAccessBreak::checkHitBreakAddress( *trans );

      if( initiatorSocket->nb_transport_fw( *trans, phase, t ) == 
	  tlm::TLM_ACCEPTED ){
	mDisableTransport = true;
#if defined(BUSIF_TST)
	if(tstFp){
	  fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
	  fprintf(tstFp,"  TLM_ACCEPTED is returned from nb_transport_fw()\n");
	}
#endif //BUSIF_TST
      }
      mTransSendNmb++;
      mAtRequestList.mpFuncModule->notifyTransSend( *trans, t );
    }
    if( !mDisableTransport ){
      if( !mRequestFifo.empty() || t != SC_ZERO_TIME ){
	if( mTransSendNmb<mTransSendNmbLmt ){
	  if( mPipeLineOpFtoF && t.value() > mCompRecepLatencyUint64 ){
	    meTransEvent.notify(sc_time((double)(t.value()-mCompRecepLatencyUint64),
					glb_resolution_unit));
	  }
	  else{
	    meTransEvent.notify( t );
	  }
	  mWaitTrans = true;
	}
      }
      else{
	mWaitTrans = false;
      }
    }
  }      
  
  /// <summary>backward pass of non blocking transport</summary>
  /// <param name="trans">transaction</param>
  /// <param name="phase">TLM phase</param>
  /// <param name="t">time</param>
  /// <returns>synchronization word</returns>
  TlmSyncEnum nb_transport_bw( TlmTransactionType& trans, 
			       TlmPhase&           phase, 
			       sc_core::sc_time&   t )
  {
#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"BusMasterIf::nb_transport_bw() is called from %s\n",
	      initiatorSocket.name());
      outputLogAtComp( &trans );
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
    char errorMessageBuf[1024];
    
    checkSlaveResponse( trans );
    
    if( phase != tlm::BEGIN_RESP ){
      sprintf( errorMessageBuf,
	       "Phase was wrong when nb_transport_bw() was called. It was not BEGIN_RESP." );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    }
    t = t + calCompRecepLatency( trans );
    
    bool success = mAtRequestList.notifyTlmCompleted( trans, t );
    if( !success ){
      char errorMessageBuf[1024];
      sprintf( errorMessageBuf,
	       "Transaction missing." );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    }

    sc_time tForNext;
    if( mPipeLineOpBtoF ){
      tForNext = t - mCompRecepLatencyTime;
    }
    else{
      tForNext = t;
    }
    if( mDisableTransport ){
      mDisableTransport = false;
      meTransEvent.notify( tForNext );
    }
    else if( mTransSendNmb >= mTransSendNmbLmt ){
      meTransEvent.notify( tForNext );
    }
    mTransNmb--;
    mTransSendNmb--;

    phase = tlm::END_RESP;
    
#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"        t for completion =%s\n",
	      t.to_string().c_str());
      fprintf(tstFp,"        t for next transaction =%s\n",
	      tForNext.to_string().c_str());
    }
#endif //BUSIF_TST

    return tlm::TLM_UPDATED;
    
  }

  /// <summary></summary>
  /// <param name="phase">TLM phase</param>
  void invalidate_direct_mem_ptr( uint64 startRange,
				  uint64 endRange ) 
  {
#if 0 //dummy because DMI is not supported
    unsigned int index;
    if( (index=getDmiDataListIndex(startRange,endRange)) !=
	BUS_MASTER_IF_ERROR_ID ){
      mDmiDataList[index].allow_none();
    }
#endif//dummy because DMI is not supported
  }

  /// <summary>check slave response</summary>
  /// <param name="trans">transaction</param>
  void checkSlaveResponse(TlmTransactionType& trans)
  {
    
    char errorMessageBuf[1024];
    
    switch(trans.get_response_status()){
    case tlm::TLM_OK_RESPONSE:
      break;
    case tlm::TLM_INCOMPLETE_RESPONSE:
      sprintf( errorMessageBuf,
	       "Error response(TLM_INCOMPLETE_RESPONSE) was detected." );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      break;
    case tlm::TLM_GENERIC_ERROR_RESPONSE:
      sprintf( errorMessageBuf,
	       "Error response(TLM_GENERIC_ERROR_RESPONSE) was detected." );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      break;
    case tlm::TLM_ADDRESS_ERROR_RESPONSE:
      sprintf( errorMessageBuf,
	       "Error response(TLM_ADDRESS_ERROR_RESPONSE) was detected." );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      break;
    case tlm::TLM_COMMAND_ERROR_RESPONSE:
      sprintf( errorMessageBuf,
	       "Error response(TLM_COMMAND_ERROR_RESPONSE) was detected." );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      break;
    case tlm::TLM_BURST_ERROR_RESPONSE:
      sprintf( errorMessageBuf,
	       "Error response(TLM_BURST_ERROR_RESPONSE) was detected." );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      break;
    case tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE:
      sprintf( errorMessageBuf,
	       "Error response(TLM_BYTE_ENABLE_ERROR_RESPONSE) was detected." );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      break;
    default:
      sprintf( errorMessageBuf,
	       "Undefined error response was detected." );
      CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      break;
    }
    
  }

#if 0 //DMI is not supported
  unsigned int isDmiOk( TlmTransactionType &trans )
  {
    for(unsigned int i=0; i<mDmiDataList.size(); i++) {
      if(trans.get_address()>=mDmiDataList[i].get_start_address() &&
	 trans.get_address()+trans.get_data_length()-1<=mDmiDataList[i].get_end_address()){
	if( mDmiDataList[i].is_read_write_allowed() ){
	  return(i);
	}
      }
    }
    return(BUS_MASTER_IF_ERROR_ID);
  }

  unsigned int getDmiDataListIndex( uint64 startRange,
				    uint64 endRange )
  {
    for(unsigned int i=0; i<mDmiDataList.size(); i++) {
      if( startRange >= mDmiDataList[i].get_start_address() &&
	  endRange <= mDmiDataList[i].get_end_address() ){
	return(i);
      }
    }
    return(BUS_MASTER_IF_ERROR_ID);
  }

  void dmiTreat( TlmTransactionType &trans, unsigned int index )
  {
    uint64 address = trans.get_address();
    
    unsigned char *Mem = mDmiDataList[index].get_dmi_ptr() + address - 
                         mDmiDataList[index].get_start_address();
    
    if( trans.is_write() ){
      memcpy( &Mem[address], 
	      trans.get_data_ptr(), 
	      trans.get_data_length() );
      mT = mDmiDataList[index].get_write_latency();
    }
    else{
      memcpy( trans.get_data_ptr(), 
	      &Mem[address], 
	      trans.get_data_length() );
      mT = mDmiDataList[index].get_read_latency();
    }
  }
#endif// DMI is not supported

  /// <summary>reset process</summary>
  void resetProcess()
  {
    mWaitTrans = false;
    meTransEvent.cancel();
    mAtRequestList.reset();
    while( !mRequestFifo.empty() ){
      mRequestFifo.pop();
    }
    mDisableTransport = false;
    mTransNmb = (unsigned int)0;
    mTransSendNmb = (unsigned int)0;
#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
	  fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"BusMasterIf::resetProcess() is called\n");
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

  /// <summary>calculate time to clock edge</summary>
  /// <param name="t">time</param>
  /// <returns>time</returns>
  sc_time calTimeToEdge( sc_time t )
  {
    uint64 tUint64 = t.value();
    uint64 time_stamp = sc_time_stamp().value() + tUint64;
    uint64 toPosEdge = 
      (time_stamp/mSystemClock+(uint64)1)*mSystemClock - time_stamp;
    if( toPosEdge == mSystemClock ){
      toPosEdge = 0;
    }
    return( sc_time((double)(tUint64+toPosEdge),glb_resolution_unit) );
  }

  /// <summary>set TLM transaction type</summary>
  /// <param name="simMode">simulation mode (AT or LT)</param>
  void setTlmTransType( SIM_MODE_T simMode )
  {
    switch( simMode ){
    case SIM_MODE_CA:
      mTransType = AT;
      break;
    case SIM_MODE_FAST:
      mTransType = LT;
      break;
    }
  }

  /// <summary>set functional module pointer</summary>
  /// <param name="pFuncModule">functional module pointer</param>
  void setFuncModulePtr( BusMasterFuncIf *pFuncModule )
  {
    mAtRequestList.setFuncModulePtr( pFuncModule );
  }

  /// <summary>set transaction nunber limitation</summary>
  /// <param name="Nmb">transaction number</param>
  void setTransNmbLmt( unsigned int Nmb )
  {
    mTransSendNmbLmt = Nmb;
  }
  
  /// <summary>set bus protocol</summary>
  /// <param name="protocol">protocol</param>
  void setBusProtocol( BusProtocol_t protocol )
  {
    mBusProtocol = protocol;
    switch( mBusProtocol ){
    case BUS_APB:
      mCompRecepLatencyCycle = 1;
      mPipeLineOpFtoF        = false;
      mPipeLineOpBtoF        = false;
      mTransSendNmbLmt       = 1;
      break;
    case BUS_AHB:
      mCompRecepLatencyCycle = 1;
      mPipeLineOpFtoF        = false;
      mPipeLineOpBtoF        = true;
      mTransSendNmbLmt       = 1;
      break;
    case BUS_AXI:
      mCompRecepLatencyCycle = 1;
      mPipeLineOpFtoF        = false;
      mPipeLineOpBtoF        = true;
      mTransSendNmbLmt       = 0xFFFFFFFF;
      break;
    case BUS_VPI:
      mCompRecepLatencyCycle = 1;
      mPipeLineOpFtoF        = true;
      mPipeLineOpBtoF        = true;
      mTransSendNmbLmt       = 0xFFFFFFFF;
      break;
    case BUS_VCI:
      mCompRecepLatencyCycle = 1;
      mPipeLineOpFtoF        = false;
      mPipeLineOpBtoF        = true;
      mTransSendNmbLmt       = 0xFFFFFFFF;
      break;
    }
    calPeriod();
  }

  /// <summary>set 0 latency</summary>
  void set0Latency( void )
  {
    mCompRecepLatencyCycle = 0;
    calPeriod();
  }

  /// <summary>calculate completion reception latency</summary>
  /// <param name="trans">transaction</param>
  /// <returns>time</returns>
  sc_time calCompRecepLatency( TlmTransactionType &trans )
  {
    if( mBusProtocol==BUS_AXI && trans.is_write() ){
      return( mCompRecepLatencyTime );
    }
    else{
      unsigned int data_num = trans.get_data_length()/mBusWidthInByte;
      unsigned int rest = trans.get_data_length()%mBusWidthInByte;
      if( rest ){
	data_num++;
      }
      return( sc_time( (double)data_num*mCompRecepLatencyUint64,
		       glb_resolution_unit ) );
    }
  }
  
  /// <summary>get clock period</summary>
  /// <returns>clock period</returns>
  uint64 getClockPeriod( void )
  {
    return( mSystemClock );
  }

  /// <summary>calculate clock period</summary>
  void calPeriod( void )
  {
    mCompRecepLatencyUint64 = mSystemClock * mCompRecepLatencyCycle;
    mCompRecepLatencyTime =
      sc_time( (double)mCompRecepLatencyUint64, glb_resolution_unit );
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

private:
  /// <summary>ouptpu logs at transport</summary>
  /// <param name="trans">transactionn</param>
  void outputLogAtTrans( TlmTransactionType *trans )
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

  /// <summary>output logs at transaction completion</summary>
  /// <param name="trans">transaction</param>
  void outputLogAtComp( TlmTransactionType *trans )
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
#endif //BUSIF_TST

  
 private:
  sc_in<bool>                   &mResetPort; //reset port
  sc_in<uint64>                 &mFreqPort; //frequency port
 public:
  TlmInitiatorSocket<BUSWIDTH>  &initiatorSocket; //initiator socket
 private:

  uint64                         mSystemClock; //clock

#if 0 //DMI is not supported
  std::vector<tlm::tlm_dmi>           mDmiDataList;//DMI data list
#endif//DMI is not supported

  BusRequestList                 mAtRequestList; // AT request list

  std::queue<TlmTransactionType *>   mRequestFifo; //request pointer FIFO
  bool                           mWaitTrans; //transport completion wainting flag

  sc_event                       meTlmCompleted; //transaction completion event
  sc_event                       meTransEvent; //tansport event

  TransMode_t                    mTransType; //transaction type
 
  bool                           mDisableTransport; //disable transport
 
  unsigned int                   mTransNmb; //transaciton number
  unsigned int                   mTransSendNmb; //transaction transmission number
  unsigned int                   mTransSendNmbLmt; //transaction transmission maximum number

  uint64                         mCompRecepLatencyCycle; //completion reception latency (cycle)
  uint64                         mCompRecepLatencyUint64; //completion reception latency (uint64)
  sc_time                        mCompRecepLatencyTime; //completion reception latency (sc_time)
  BusProtocol_t                  mBusProtocol; // bus protocol
  unsigned int                   mBusWidthInByte; // bus width in byte
  bool                           mPipeLineOpFtoF; //whether pipe line operation is needed at Request(Fw)-Request(Fw)
  bool                           mPipeLineOpBtoF; //whether pipe line operation is needed at Response(Bw)-Request(Fw)
#if defined(BUSIF_TST)
  FILE                           *tstFp; // log file pointer bus I/F test
#endif // BUSIF_TST
};

#endif //__BUSMASTERIF__
