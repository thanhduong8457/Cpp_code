/*************************************************************************
 *
 *  OSCI2ARBTR.h
 *
 *  Copyright(c) 2011 Renesas Electronics Corporation
 *  RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 *  This program must be used solely for the purpose for which
 *  it was furnished by Renesas Electronics Corporation. No part of this
 *  program may be reproduced or disclosed to others, in any
 *  form, without the prior written permission of Renesas Electronics
 *  Corporation.
 *
 *************************************************************************/

#ifndef OSCI2ARBTR_H
#define OSCI2ARBTR_H

#include <vector>

#include "OSCI2.h"
#include "CmErrMsg.h"

extern sc_time_unit glb_resolution_unit;

template<unsigned int BUSWIDTH>
class OSCI2ARBTR : 
public sc_module,
public virtual TlmFwTransportIf,
public virtual TlmBwTransportIf
{
protected:
  const static unsigned int NO_REQ_ID = 0xFFFFFFFF; // no request
  const static unsigned int NO_BUS_ID = 0xFFFFFFFF; // no bus ID
  const static unsigned long long MAX_PERIOD = 0xFFFFFFFFFFFFFFFFULL; // max value of period

  typedef tlm::tlm_base_initiator_socket_b<BUSWIDTH> I_SOCKET_TYPE;

  /// <summary>ArbitrationPolicy</summary>
  enum {
    PRIORITYSCHEDULE = 0,
    ROUNDROBIN       = 1
  }; 

  /// <summary>structure of bus request</summary>
  struct BusArbRequest{
    TlmTransactionType *pTransaction;
    TlmPhase            phase;
    sc_core::sc_time    t;
    TlmBwTransportIf   *pIfPointer;
    bool                targetLock;
    bool                doneFlag;
    sc_dt::uint64       reqTime;
  };

  /// <summary>bus request list class</summary>
  class BusArbRequestList{
  public:
    /// <summary>constructor</summary>
    BusArbRequestList( void );

    /// <summary>destructor</summary>
    ~BusArbRequestList( void );

    /// <summary>register bus request list</summary>
    /// <param name="trans">transaction</param>
    /// <param name="phase">TLM phase</param>
    /// <param name="t">time</param>
    /// <param name="t">G3M extension payload</param>
    /// <returns>synchronization information</returns>
    void               registerList( TlmTransactionType *pTrans,
				     TlmPhase *phase,
				     sc_core::sc_time *t,
				     TlmG3mExtension *pG3mExtension );

    /// <summary>find request</summary>
    /// <param name="trans">interface pointer</param>
    /// <returns>index</returns>
    unsigned int       findRequest( TlmBwTransportIf *pIf );

    /// <summary>find incompleted request</summary>
    /// <param name="trans">interface pointer</param>
    /// <returns>index</returns>
    unsigned int       findIncompletedRequest( TlmBwTransportIf *pIf );


    /// <summary>get request</summary>
    /// <param name="trans">interface pointer</param>
    /// <returns>request pointer</returns>
    BusArbRequest      *getRequest( TlmBwTransportIf *pIf );

    /// <summary>get other request</summary>
    /// <param name="trans">interface pointer</param>
    /// <returns>request pointer</returns>
    virtual BusArbRequest* getOtherRequest( TlmBwTransportIf *pIf );

    /// <summary>get if any requests exist or not</summary>
    /// <returns>exit or not</returns>
    bool               isExistRequest( void );

    /// <summary>reset function</summary>
    void               reset( void );

    /// <summary>set arbitration policy
    /// but roundrobin can be set only</summary>
    /// <param name="policy">arbitration policy</param>
    void               setArbPolicy( unsigned int policy );
#if 0 
    void               setBusIdList( TlmBwTransportIf *pIf );
#endif // 0

    /// <summary>get bus ID list size</summary>
    /// <returns>list size</returns>
    unsigned int       getBusIdListSize(void);

    /// <summary>get bus ID</summary>
    /// <param name="i">index</param>
    /// <returns>interface pointer</returns>
    TlmBwTransportIf  *getBusId( unsigned int i );

    /// <summary>get bus list index</summary>
    /// <param name="pIf">interface pointer</param>
    /// <returns>index</returns>
    unsigned int       getBusIdListIndex( TlmBwTransportIf *pIf );

    /// <summary>get first bus ID</summary>
    /// <returns>interface pointer</returns>
    TlmBwTransportIf  *getFirstBusId( void );
  public:
    std::vector< BusArbRequest* >    mRequestList; //bus request list
    std::vector< TlmBwTransportIf* > mBusIdList; //bus ID list
    unsigned int                     mArbPolicy; //arbitration policy
    TlmBwTransportIf                *mFirstBusId; //first bus ID
  };
  
public:
  sc_in<bool>             resetPort;  //Reset (active LOW)
  sc_in<sc_dt::uint64>    freqPort;   //clock frequency
  TlmInitiatorSocket<BUSWIDTH>  is;   //TLM2 initiator socket
  TlmTargetSocket<BUSWIDTH>     ts;   //TLM2 target socket

  SC_HAS_PROCESS(OSCI2ARBTR);

#if defined(BUSIF_TST)
  OSCI2ARBTR( sc_module_name moduleName,
	      FILE           *fp = 0 );
#else // BUSIF_TST
  /// <summary>constructor</summary>
  /// <param name="moduleName">name of sc_module</param>
  OSCI2ARBTR( sc_module_name moduleName );
#endif // BUSIF_TST
  ~OSCI2ARBTR( );

  //--- member function for  target_port
  /// <summary>transport API for backward pass of AT</summary>
  /// <param name="trans">transaction</param>
  /// <param name="phase">TLM phase</param>
  /// <param name="t">time</param>
  /// <returns>synchronization information</returns>
  virtual TlmSyncEnum nb_transport_bw( TlmTransactionType& trans,
                                       TlmPhase& phase, sc_core::sc_time& t);

  /// <summary>transport API for forward pass of AT</summary>
  /// <param name="trans">transaction</param>
  /// <param name="phase">TLM phase</param>
  /// <param name="t">time</param>
  /// <returns>synchronization information</returns>
  virtual TlmSyncEnum nb_transport_fw( TlmTransactionType& trans,
                                        TlmPhase& phase, sc_core::sc_time& t);

  /// <summary>transport API for debug access</summary>
  /// <param name="r">transaction</param>
  /// <returns>data number</returns>
  unsigned int transport_dbg( TlmTransactionType& r );
  /// dummy
  void invalidate_direct_mem_ptr( sc_dt::uint64 startRange,
				  sc_dt::uint64 endRange );
  /// <summary>dummy</summary>
  bool get_direct_mem_ptr( TlmTransactionType& trans,
			   tlm::tlm_dmi&       dmiData );

  /// <summary>transport API for LT</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  void b_transport( TlmTransactionType& trans, sc_core::sc_time& t );

  /// <summary>reset process</summary>
  virtual void resetProcess(void);

  /// <summary>arbitration</summary>
  virtual void arbitrate( void );

  /// <summary>arbitration</summary>
  virtual void actionAtClockPos( void );

  /// <summary>end of elaboration</summary>
  void end_of_elaboration( void );

  /// <summary>calculate period</summary>
  void calculatePeriod( void );

  /// <summary>register port</summary>
  /// <param name="port">initiator port</param>
  /// <param name="name">name of initiator port</param>
  void register_port( sc_port_base& port, const char* name );

protected:
  bool               mWaitForBw; //wait for transaction completion
  bool               mTargetLock; //lock
  TlmBwTransportIf  *mOwnerId; //bus owner ID
  TlmBwTransportIf  *mPrevDmaId; //previous DMA bus owner ID
  TlmBwTransportIf  *mPrevPeId; //previous PE bus owner ID
  BusArbRequestList  mDmaRequestList; //DMA request list
  BusArbRequestList  mPeRequestList; //PE request list
  BusArbRequestList  mTempDmaRequestList; //temporary DMA request list
  BusArbRequestList  mTempPeRequestList; //temporary PE request list
  BusArbRequest     *mpRequest; // granted request
  unsigned int       mArbPolicy; // arbitration policy

  sc_dt::uint64      mPeriod; // clock period

  sc_event           mTransportEvent; // transport event

  sc_pvector< TlmBwTransportIf* >   mInterfaceList; //interface list

#if defined(BUSIF_TST)
  FILE *tstFp;
#endif // BUSIF_TST

};


template< unsigned int BUSWIDTH >
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::BusArbRequestList( void )
{
  mArbPolicy = ROUNDROBIN;
  mFirstBusId = 0;
}


template< unsigned int BUSWIDTH >
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::~BusArbRequestList( void )
{
  mRequestList.resize(0);
  mBusIdList.resize(0);
}


template< unsigned int BUSWIDTH >
void
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::registerList( TlmTransactionType *pTrans,
				 TlmPhase *phase,
				 sc_time *t,
				 TlmG3mExtension *pG3mExtension )
{

  unsigned int requestId = findRequest( pG3mExtension->getIfPointer() );
  if( requestId != NO_REQ_ID && mRequestList[ requestId ]->doneFlag ){
    mRequestList[ requestId ]->pTransaction = pTrans;
    mRequestList[ requestId ]->phase        = *phase;
    mRequestList[ requestId ]->t            = *t;
    mRequestList[ requestId ]->pIfPointer
      = pG3mExtension->getIfPointer();
    mRequestList[ requestId ]->doneFlag     = false;
    mRequestList[ requestId ]->reqTime = sc_time_stamp().value();
    TlmAxiExtension *AxiExt = (TlmAxiExtension *)0;
    pTrans->get_extension( AxiExt );
    if( AxiExt ){
      mRequestList[ requestId ]->targetLock = AxiExt->isLock();
    }
    TlmAhbExtension *AhbExt = (TlmAhbExtension *)0;
    pTrans->get_extension( AhbExt );
    if( AhbExt ){
      mRequestList[ requestId ]->targetLock = AhbExt->isLock();
    }
    TlmApbExtension *ApbExt = (TlmApbExtension *)0;
    pTrans->get_extension( ApbExt );
    if( ApbExt ){
      mRequestList[ requestId ]->targetLock = ApbExt->isLock();
    }
  }
  else{
    BusArbRequest *request = new BusArbRequest;
    request->pTransaction = pTrans;
    request->phase        = *phase;
    request->t            = *t;
    request->pIfPointer
      = pG3mExtension->getIfPointer();
    request->doneFlag     = false;
    request->reqTime     = sc_time_stamp().value();
    request->targetLock = false;
    TlmAxiExtension *AxiExt = (TlmAxiExtension *)0;
    pTrans->get_extension( AxiExt );
    if( AxiExt ){
      request->targetLock = AxiExt->isLock();
    }
    TlmAhbExtension *AhbExt = (TlmAhbExtension *)0;
    pTrans->get_extension( AhbExt );
    if( AhbExt ){
      request->targetLock = AhbExt->isLock();
    }
    TlmApbExtension *ApbExt = (TlmApbExtension *)0;
    pTrans->get_extension( ApbExt );
    if( ApbExt ){
      request->targetLock = ApbExt->isLock();
    }
    mRequestList.push_back( request );
  }

}


template< unsigned int BUSWIDTH >
unsigned int 
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::findRequest( TlmBwTransportIf *pIf )
{
  unsigned int requestSize = mRequestList.size();
  for( unsigned int requestId = 0; requestId < requestSize; requestId++ ){
    if( pIf == mRequestList[ requestId ]->pIfPointer )
      return requestId;
  }
  return NO_REQ_ID;
}


template< unsigned int BUSWIDTH >
unsigned int
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::findIncompletedRequest( TlmBwTransportIf *pIf )
{
  unsigned int requestId = findRequest( pIf );
  if( requestId != NO_REQ_ID ){
    if( !(mRequestList[ requestId ]->doneFlag) &&
	mRequestList[ requestId ]->reqTime < sc_time_stamp().value() ){
      return requestId;
    }
  }
  return NO_REQ_ID;
}


template< unsigned int BUSWIDTH >
typename OSCI2ARBTR<BUSWIDTH>::BusArbRequest*
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::getRequest( TlmBwTransportIf *pIf )
{
  unsigned int requestId = findIncompletedRequest( pIf );
  if( requestId == (unsigned int)NO_REQ_ID ){
    return( (BusArbRequest *)0 );
  }
  else{
    return( mRequestList[requestId] );
  }
}


template< unsigned int BUSWIDTH >
typename OSCI2ARBTR<BUSWIDTH>::BusArbRequest* 
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::getOtherRequest( TlmBwTransportIf *pIf )
{
  unsigned int requestId;

  if ( mArbPolicy == ROUNDROBIN) {  //ROUND ROBIN
    unsigned int i;
    unsigned int currentIndex = getBusIdListIndex( pIf )+1;
    //    i++;
    for( i=currentIndex; i<getBusIdListSize(); i++ ){
      if( (requestId=findIncompletedRequest( getBusId(i) )) != 
	  NO_REQ_ID ){
	return( mRequestList[requestId] );
      }
    }
    for( i=0; i<currentIndex; i++ ){
      if( (requestId=findIncompletedRequest( getBusId(i)) ) != 
	  NO_REQ_ID ){
	return( mRequestList[requestId] );
      }
    }
    return( (BusArbRequest *)0 );
  }
  else {    // PRIORITY
    // not supported
    return( (BusArbRequest *)0 );
  }
}


template< unsigned int BUSWIDTH >
bool 
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::isExistRequest( void )
{
  unsigned int requestSize = mRequestList.size();
  for( unsigned int requestId = 0; requestId < requestSize; requestId++ ){
    if( !mRequestList[ requestId ]->doneFlag )
      return true;
  }
  return false;
}


template< unsigned int BUSWIDTH >
void 
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::reset( void )
{
  unsigned int requestSize = mRequestList.size();
  for( unsigned int requestId = 0; requestId < requestSize; requestId++ ){
    mRequestList[ requestId ]->doneFlag = true;
  }
}


template< unsigned int BUSWIDTH >
void
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::setArbPolicy(unsigned int policy)
{
  mArbPolicy = policy;
}


template< unsigned int BUSWIDTH >
unsigned int 
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::getBusIdListSize()
{
  return( mBusIdList.size() );
}


template< unsigned int BUSWIDTH >
TlmBwTransportIf *
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::getBusId(unsigned int i)
{
  return(mBusIdList[i]);
}


template< unsigned int BUSWIDTH >
unsigned int
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::getBusIdListIndex(TlmBwTransportIf *pIf)
{
  for(unsigned int i=0; i<mBusIdList.size(); i++) {
    if (pIf == getBusId(i)) return(i);
  }
  return(NO_BUS_ID);
}


template< unsigned int BUSWIDTH >
TlmBwTransportIf *
OSCI2ARBTR<BUSWIDTH>::
BusArbRequestList::getFirstBusId( void )
{
  return( mFirstBusId );
}


template< unsigned int BUSWIDTH >
OSCI2ARBTR<BUSWIDTH>::
#if defined(BUSIF_TST)
OSCI2ARBTR( sc_module_name moduleName, 
	    FILE *fp ):
#else // BUSIF_TST
OSCI2ARBTR( sc_module_name moduleName ):
#endif // BUSIF_TST
  sc_module( moduleName ),
  resetPort( "resetPort" ),
  freqPort( "freqPort" ),
  is( "is" ),
  ts( "ts" ),
  mWaitForBw( false ),
  mTargetLock( false ),
  mOwnerId( (TlmBwTransportIf*)0 ),
  mPrevDmaId( (TlmBwTransportIf*)0 ),
  mPrevPeId( (TlmBwTransportIf*)0 ),
  mpRequest( (BusArbRequest*)0 ),
  mArbPolicy(0),
  mPeriod(0),
  mTransportEvent()
#if defined(BUSIF_TST)
  ,tstFp( fp )
#endif // BUSIF_TST
{

  is(*this);
  ts( *this );

  mDmaRequestList.reset();
  mPeRequestList.reset();
  mTempDmaRequestList.reset();
  mTempPeRequestList.reset();

  SC_METHOD( actionAtClockPos );
  sensitive << mTransportEvent;
  dont_initialize();
  
  SC_METHOD(resetProcess);
  sensitive << resetPort.neg();
  dont_initialize();

  SC_METHOD( calculatePeriod );
  sensitive << freqPort;
  dont_initialize();
}


template< unsigned int BUSWIDTH >
OSCI2ARBTR<BUSWIDTH>::
~OSCI2ARBTR( )
{
}


template< unsigned int BUSWIDTH >
TlmSyncEnum 
OSCI2ARBTR<BUSWIDTH>::
nb_transport_fw( TlmTransactionType& trans,
		 TlmPhase& phase,
		 sc_time& t )
{

#if defined(BUSIF_TST)
  if(tstFp){
    fprintf(tstFp,"%s:ARBITER:nb_transport_fw() invoked\n",
	    sc_time_stamp().to_string().c_str());
    if( trans.get_command() == tlm::TLM_READ_COMMAND )
      fprintf(tstFp,"        TLM_READ_COMMAND\n");
    else
      fprintf(tstFp,"        TLM_WRITE_COMMAND\n");
    fprintf(tstFp,"        address     =0x%llx\n",
	    trans.get_address());
    int dataLength = trans.get_data_length();
    fprintf(tstFp,"        dataLength  =%d\n",
	    dataLength);
    unsigned char *data = trans.get_data_ptr();
    for(int i=0; i<dataLength; i++)
      fprintf(tstFp,"        data[%d]     =0x%x\n",
	      i,data[i]);
    fprintf(tstFp,"        streamWidth =%d\n",
	    trans.get_streaming_width());
    if( trans.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE )
      fprintf(tstFp,"        TLM_INCOMPLETE_RESPONSE\n");
    else
      fprintf(tstFp,"        responseStatus error\n");
    TlmG3mExtension *pG3mExtension = NULL;
    trans.get_extension( pG3mExtension );
    if( pG3mExtension ){
      fprintf(tstFp,"        IfPointer   =0x%llx\n",
	      pG3mExtension->getIfPointer());
    }
  }
#endif // BUSIF_TST

  TlmG3mExtension *pG3mExtension = NULL;
  trans.get_extension( pG3mExtension );

  if( !pG3mExtension ){
    // error (not support TlmG3mExtension)
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf,
	     "Received transaction doesn't support the TlmG3mExtension." );
    CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    return tlm::TLM_ACCEPTED;
  }

  if( phase != tlm::BEGIN_REQ ){
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf,
	     "Phase was wrong at nb_transport_fw() calling. It was not BEGIN_REQ." );
    CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
  }

  if( mDmaRequestList.getBusIdListIndex(pG3mExtension->getIfPointer()) != 
      NO_BUS_ID ){
    mDmaRequestList.registerList( &trans, &phase, &t, pG3mExtension );
#if defined(BUSIF_TST)
    if(tstFp){
      fprintf(tstFp,"        Stored as DMA : IfPointer=0x%llx\n"
	      ,pG3mExtension->getIfPointer());
    }
#endif // BUSIF_TST
  }
  else if( mPeRequestList.getBusIdListIndex(pG3mExtension->getIfPointer()) != 
	   NO_BUS_ID ){
    mPeRequestList.registerList( &trans, &phase, &t, pG3mExtension );
#if defined(BUSIF_TST)
    if(tstFp){
      fprintf(tstFp,"        Stored as PE : IfPointer=0x%llx\n"
	      ,pG3mExtension->getIfPointer());
    }
#endif // BUSIF_TST
  }
  else{ // Transaction is received from undefined Master IP.
    mPeRequestList.mBusIdList.push_back( pG3mExtension->getIfPointer() );
    mPeRequestList.registerList( &trans, &phase, &t, pG3mExtension );
    // warning unknown socket
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf, 
	     "Received transaction from unknown tlm_bw_transport_if[0x%llx].",
	     pG3mExtension->getIfPointer() );
    CmErrMsg::printWarningMessage( name(), 0, errorMessageBuf );
  }
  if( ( mpRequest == 0 ) ||
      ( (mpRequest != 0) && !mWaitForBw ) ){
    mTransportEvent.notify(sc_time((double)mPeriod,glb_resolution_unit));
  }

  trans.set_response_status(tlm::TLM_OK_RESPONSE);
  phase = tlm::END_REQ;

  return tlm::TLM_UPDATED;

}


template< unsigned int BUSWIDTH >
TlmSyncEnum 
OSCI2ARBTR<BUSWIDTH>::
nb_transport_bw( TlmTransactionType& trans,
		 TlmPhase& phase,
		 sc_time& t )
{
  mWaitForBw = false;
 
  TlmSyncEnum rtn = 
    mpRequest->pIfPointer->nb_transport_bw(trans, phase, t);

  mTransportEvent.notify();

#if defined(BUSIF_TST)
  if(tstFp){
    fprintf(tstFp,"%s:ARBITER:nb_transport_bw() invoked\n",
	    sc_time_stamp().to_string().c_str());
  }
#endif // BUSIF_TST

  return( rtn );
}


template< unsigned int BUSWIDTH >
unsigned int
OSCI2ARBTR<BUSWIDTH>::
transport_dbg( TlmTransactionType& r )
{
#if defined(BUSIF_TST)
  if(tstFp){
    fprintf(tstFp,"%s:ARBITER:transport_dbg() invoked\n",
	    sc_time_stamp().to_string().c_str());
    if( r.get_command() == tlm::TLM_READ_COMMAND )
      fprintf(tstFp,"        TLM_READ_COMMAND\n");
    else
      fprintf(tstFp,"        TLM_WRITE_COMMAND\n");
    fprintf(tstFp,"        address     =0x%llx\n",
	    r.get_address());
    int dataLength = r.get_data_length();
    fprintf(tstFp,"        dataLength  =%d\n",
	    dataLength);
    unsigned char *data = r.get_data_ptr();
    for(int i=0; i<dataLength; i++)
      fprintf(tstFp,"        data[%d]     =0x%x\n",
	      i,data[i]);
    fprintf(tstFp,"        streamWidth =%d\n",
	    r.get_streaming_width());
    if( r.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE )
      fprintf(tstFp,"        TLM_INCOMPLETE_RESPONSE\n");
    else
      fprintf(tstFp,"        responseStatus error\n");
    TlmG3mExtension *pG3mExtension = NULL;
    r.get_extension( pG3mExtension );
    if( pG3mExtension ){
      fprintf(tstFp,"        IfPointer   =0x%llx\n",
	      pG3mExtension->getIfPointer());
    }
  }
#endif // BUSIF_TST

  return( is->transport_dbg( r ) );
}


template< unsigned int BUSWIDTH >
void 
OSCI2ARBTR<BUSWIDTH>::
invalidate_direct_mem_ptr( sc_dt::uint64 startRange,
			   sc_dt::uint64 endRange ){
#if 0 //dummy because DMI is not supported
#if defined(BUSIF_TST)
  if(tstFp){
    fprintf(tstFp,"%s:ARBITER:invalidate_direct_mem_ptr() invoked\n",
	    sc_time_stamp().to_string().c_str());
    fprintf(tstFp,"        startRange =0x%llx\n",
	    startRange);
    fprintf(tstFp,"        endRange   =0x%llx\n",
	    endRange);
  }
#endif // BUSIF_TST

  int size = mTerminalSocketList.size();
  for( int i=0; i<size; i++ ){
    mTerminalSocketList.fetch( i )->get_base_interface().invalidate_direct_mem_ptr( startRange, endRange );
  }
#endif//dummy because DMI is not supported
}


template< unsigned int BUSWIDTH >
bool
OSCI2ARBTR<BUSWIDTH>::
get_direct_mem_ptr( TlmTransactionType& trans, tlm::tlm_dmi&  dmiData )
{
#if 0 //DMI is not supported
#if defined(BUSIF_TST)
  if(tstFp){
    fprintf(tstFp,"%s:ARBITER:get_direct_mem_ptr() invoked\n",
	    sc_time_stamp().to_string().c_str());
    fprintf(tstFp,"        dmiData     =%x\n",
	    dmiData);
  }
#endif // BUSIF_TST

  return( is->get_direct_mem_ptr( trans, dmiData ) );
#else //DMI is not supported
    return false;
#endif//DMI is not supported
}


template< unsigned int BUSWIDTH >
void
OSCI2ARBTR<BUSWIDTH>::
b_transport(TlmTransactionType& trans, sc_time& t)
{

  TlmG3mExtension *pG3mExtension = NULL;
  trans.get_extension( pG3mExtension );

  if( !pG3mExtension ){
    // error (not supported AdditionalPayload)
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf,
	     "Received transaction is not supported the additional payload." );
    CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
  }

#if defined(BUSIF_TST)
  if(tstFp){
    fprintf(tstFp,"%s:ARBITER:b_transport() invoked\n",
	    sc_time_stamp().to_string().c_str());
    if( trans.get_command() == tlm::TLM_READ_COMMAND )
      fprintf(tstFp,"        TLM_READ_COMMAND\n");
    else
      fprintf(tstFp,"        TLM_WRITE_COMMAND\n");
    fprintf(tstFp,"        address     =0x%llx\n",
	    trans.get_address());
    int dataLength = trans.get_data_length();
    fprintf(tstFp,"        dataLength  =%d\n",
	    dataLength);
    unsigned char *data = trans.get_data_ptr();
    for(int i=0; i<dataLength; i++)
      fprintf(tstFp,"        data[%d]     =0x%x\n",
	      i,data[i]);
    fprintf(tstFp,"        streamWidth =%d\n",
	    trans.get_streaming_width());
    if( trans.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE )
      fprintf(tstFp,"        TLM_INCOMPLETE_RESPONSE\n");
    else
      fprintf(tstFp,"        responseStatus error\n");
    TlmG3mExtension *pG3mExtension = NULL;
    trans.get_extension( pG3mExtension );
  }
#endif // BUSIF_TST

  t = t + sc_time((double)mPeriod,glb_resolution_unit);
  is->b_transport(trans, t);

}


template< unsigned int BUSWIDTH >
void
OSCI2ARBTR<BUSWIDTH>::
resetProcess()
{

#if defined(BUSIF_TST)
  if(tstFp){
    fprintf(tstFp,"%s:ARBITER:reset() invoked\n",
	    sc_time_stamp().to_string().c_str());
  }
#endif // BUSIF_TST

  mDmaRequestList.reset();
  mPeRequestList.reset();
  mpRequest        = 0;
  mTargetLock      = false;
  mOwnerId         = (TlmBwTransportIf*)0;
  mPrevDmaId       = (TlmBwTransportIf*)0;
  mPrevPeId        = (TlmBwTransportIf*)0;
  mWaitForBw       = false;
  mTransportEvent.cancel();

}


template< unsigned int BUSWIDTH >
void
OSCI2ARBTR<BUSWIDTH>::arbitrate( void )
{

#if defined(BUSIF_TST)
  if(tstFp){
    fprintf(tstFp,"%s:ARBITER:arbitrate() invoked\n",
	    sc_time_stamp().to_string().c_str());
    fprintf(tstFp,"   [before arbitration]\n");
    fprintf(tstFp,"           ownerId  =0x%llx\n",mOwnerId);
    fprintf(tstFp,"           PrevDmaId=0x%llx\n",mPrevDmaId);
    fprintf(tstFp,"           PrevPeId =0x%llx\n",mPrevPeId);
    fprintf(tstFp,"           lock     =0x%x\n",mTargetLock);
  }
#endif // BUSIF_TST

  typename OSCI2ARBTR<BUSWIDTH>::BusArbRequest *request = 0;
  TlmBwTransportIf *firstBusId;
  
  if( mDmaRequestList.getBusIdListSize() ){
    
    firstBusId = mDmaRequestList.getBusId(0);
    
    /* arbitration among DMAs */
    if( ( mOwnerId != (TlmBwTransportIf*)0 ) && ( mPrevDmaId != (TlmBwTransportIf*)0 ) ){
      if( mTargetLock == true ){
	request = mDmaRequestList.getRequest( mOwnerId );
      }
      else{
	request = mDmaRequestList.getOtherRequest( mPrevDmaId );
      }
    }
    else if( mTargetLock == false ){
      request = mDmaRequestList.getRequest( firstBusId );
      if((( request == 0 ) || 
	  (( request != 0 ) && request->doneFlag ))){
	request = mDmaRequestList.getOtherRequest( firstBusId );
      }

      if (mTargetLock == false) {
	if(( request == 0 ) || 
	   (( request != 0 ) && request->doneFlag ))
	  mPrevDmaId = (TlmBwTransportIf*)0;
	else 
	  mPrevDmaId = request->pIfPointer;
      }
      
    }
  }

  /* arbitration among PEs */
  if( mPeRequestList.getBusIdListSize() ){
    firstBusId = mPeRequestList.getBusId(0);
    if(( request == 0 ) || 
       (( request != 0 ) && request->doneFlag )){
      if( ( mOwnerId != (TlmBwTransportIf*)0 ) && ( mPrevPeId != (TlmBwTransportIf*)0 ) ){
	if( mTargetLock == true ){
	  request = mPeRequestList.getRequest( mOwnerId );
	}
	else{
	  request = mPeRequestList.getOtherRequest( mPrevPeId );
	}
      }
      else if( mTargetLock == false ){
	request = mPeRequestList.getRequest( firstBusId );
	if((( request == 0 ) || 
	    (( request != 0 ) && request->doneFlag )))
	request = mPeRequestList.getOtherRequest( firstBusId );
      }
      
      if (mTargetLock == false) {
	if(( request == 0 ) || 
	   (( request != 0 ) && request->doneFlag ))
	  mPrevPeId = (TlmBwTransportIf*)0;
	else 
	  mPrevPeId = request->pIfPointer;
      }
      
    }
  }

  if (mTargetLock == false) {
    mOwnerId    = ( request == 0 ) ? (TlmBwTransportIf*)0 : 
      request->pIfPointer;
  }
  if( ( request != 0 ) && request->doneFlag )
    mpRequest = 0;
  else
    mpRequest = request;
  mTargetLock = ( request == 0 ) ? mTargetLock :
    request->targetLock;

#if defined(BUSIF_TST)
  if(tstFp){
    fprintf(tstFp,"   [after arbitration]\n");
    fprintf(tstFp,"           ownerId  =0x%llx\n",mOwnerId);
    fprintf(tstFp,"           PrevDmaId=0x%llx\n",mPrevDmaId);
    fprintf(tstFp,"           PrevPeId =0x%llx\n",mPrevPeId);
    fprintf(tstFp,"           lock     =0x%x\n",mTargetLock);
  }
#endif // BUSIF_TST

}


template< unsigned int BUSWIDTH >
void
OSCI2ARBTR<BUSWIDTH>::actionAtClockPos( void )
{
  if((mDmaRequestList.isExistRequest())||(mPeRequestList.isExistRequest())){
    arbitrate();
  }
  else{
    if (mTargetLock == false ) {
      mOwnerId = (TlmBwTransportIf*)0;
    }
  }
  if( mpRequest != 0 && mpRequest->pTransaction &&
     !(mpRequest->doneFlag) ){   // This "if" sentence is for reset input
    mpRequest->phase = tlm::BEGIN_REQ;
    mpRequest->pTransaction->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    is->nb_transport_fw( *(mpRequest->pTransaction),
			 mpRequest->phase,
			 mpRequest->t );
    mpRequest->doneFlag = true;
    mWaitForBw = true;
  }
}


template< unsigned int BUSWIDTH >
void 
OSCI2ARBTR<BUSWIDTH>::end_of_elaboration( void )
{
  int size = mInterfaceList.size();
  TlmBwTransportIf *pInterface;
  for( int i=0; i<size; i++ ){
    pInterface = mInterfaceList.fetch( i );
    if( mTempDmaRequestList.getBusIdListIndex(pInterface)!=NO_BUS_ID ){
      mDmaRequestList.mBusIdList.push_back( pInterface );
    }
    else{
      mPeRequestList.mBusIdList.push_back( pInterface );
    }
  }
}


template< unsigned int BUSWIDTH >
void 
OSCI2ARBTR<BUSWIDTH>::calculatePeriod( void )
{
  if( freqPort.read() == 0 ){
    mPeriod = MAX_PERIOD;
  }
  else{
    switch(glb_resolution_unit){
    case SC_SEC:
      mPeriod = (sc_dt::uint64)(1.0)/(sc_dt::uint64)freqPort.read();
      break;
    case SC_MS:
      mPeriod = (sc_dt::uint64)(1.0e+3)/(sc_dt::uint64)freqPort.read();
      break;
    case SC_US:
      mPeriod = (sc_dt::uint64)(1.0e+6)/(sc_dt::uint64)freqPort.read();
      break;
    case SC_NS:
      mPeriod = (sc_dt::uint64)(1.0e+9)/(sc_dt::uint64)freqPort.read();
      break;
    case SC_PS:
      mPeriod = (sc_dt::uint64)(1.0e+12)/(sc_dt::uint64)freqPort.read();
      break;
    case SC_FS:
      mPeriod = (sc_dt::uint64)(1.0e+15)/(sc_dt::uint64)freqPort.read();
      break;
    }
  }
}


template< unsigned int BUSWIDTH >
void 
OSCI2ARBTR<BUSWIDTH>::register_port( sc_port_base& port, const char* name )
{
  I_SOCKET_TYPE *socket = dynamic_cast<I_SOCKET_TYPE * >( &port );
  if( socket == NULL ){
  }
  else{
    TlmBwTransportIf *pIf = &(socket->get_base_interface());
    mInterfaceList.push_back(pIf);
  }
}


#endif // OSCI2ARBTR_H
