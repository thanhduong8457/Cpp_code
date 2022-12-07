/*************************************************************************
 *
 *  BusBridgeCoreBase.h
 *
 *  Copyright(c) 2009-2013 Renesas Electronics Corporation
 *  RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 *  This program must be used solely for the purpose for which
 *  it was furnished by Renesas Electronics Corporation. No part of this
 *  program may be reproduced or disclosed to others, in any
 *  form, without the prior written permission of Renesas Electronics
 *  Corporation.
 *
 *************************************************************************/

#ifndef __BUS_BRIDGE_CORE_BASE_H__
#define __BUS_BRIDGE_CORE_BASE_H__


#include "BusMasterIf.h"
#include "BusBridgeSlaveIf.h"
#include "BusSlaveFuncIf.h"
#include "BusMasterFuncIf.h"
#include "BusBridgeModuleBase.h"
#include "BusTimeBase.h"


template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH> class BusBridgeModuleBase;
template<unsigned int M_BUSWIDTH> class BusMasterIf;
template<unsigned int S_BUSWIDTH> class BusBridgeSlaveIf;
class BusSlaveFuncIf;
class BusMasterFuncIf;

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
class BusBridgeCoreBase : 
public virtual BusSlaveFuncIf,
public virtual BusMasterFuncIf,
public virtual BusTimeBase
{

 public:
  /// <summary>TLM transport mode</summary>
  typedef enum
  {
    AT  = 0,
    LT  = 1,
    DMI = 2
  } TransMode_t;

  /// <summary>constructor</summary>
  /// <param name="parent">pointer of parent class</param>
  /// <param name="slaveIf">pointer of bus slave I/F</param>
  /// <param name="masterIf">pointer of bus master I/F</param>
  BusBridgeCoreBase( BusBridgeModuleBase<S_BUSWIDTH,M_BUSWIDTH> *parent,
		     BusBridgeSlaveIf<S_BUSWIDTH>    *slaveIf,
		     BusMasterIf<M_BUSWIDTH>         *masterIf );

  /// <summary>destructor</summary>
  ~BusBridgeCoreBase(void);

  /// <summary>read function</summary>
  /// <param name="offsetAddress">offset address</param>
  /// <param name="trans">generic payload</param>
  /// <param name="t">time</param>
  /// <param name="debug">debug access</param>
  virtual void read( unsigned int     offsetAddress, 
		     TlmBasicPayload& trans, 
		     BusTime_t*       t,
		     bool             debug=false );

  /// <summary>write function</summary>
  /// <param name="offsetAddress">offset address</param>
  /// <param name="trans">generic payload</param>
  /// <param name="t">time</param>
  /// <param name="debug">debug access</param>
  virtual void write( unsigned int     offsetAddress, 
		      TlmBasicPayload& trans, 
		      BusTime_t*       t,
		      bool             debug=false );

  /// <summary>reset function</summary>
  virtual void reset( void );

  /// <summary>delete transactin object</summary>
  virtual void deleteTrans( void );

  /// <summary>delete all transactin object</summary>
  void deleteAllTrans( void );

  /// <summary>set TLM transaction type</summary>
  /// <param name="simMode">simulation mode (AT or LT)</param>
  void setTlmTransType( SIM_MODE_T simMode );

  /// <summary>set bridge latency for forward path</summary>
  /// <param name="cycle">latency for forward path</param>
  void setBridgeFwLatency( uint64 latency );

  /// <summary>set bridge latency for backward path</summary>
  /// <param name="cycle">latency for backward path</param>
  void setBridgeBwLatency( uint64 latency );

  /// <summary>set that transaction splitter is used</summary>
  /// <param name="transSplit">if transaction splitter is used</param>
  void setTransSplitter( bool transSplit );

#ifdef BUSIF_TST
  /// <summary>set log file pointer</summary>
  /// <param name="fp">file pointer</param>
  void setLogFileP( FILE *fp );
#endif //BUSIF_TST

  BusMasterIf<M_BUSWIDTH>       *mMasterIf; // Bus master I/F
  BusBridgeSlaveIf<S_BUSWIDTH>  *mSlaveIf; // Bus slave I/F
  BusBridgeModuleBase<S_BUSWIDTH,M_BUSWIDTH> *mParent; // parent pointer
  TransMode_t                    mTransType; //TLM transport type

#ifdef BUSIF_TST
  FILE *tstFp; // log file pointer bus I/F test
#endif //BUSIF_TST

 private:
  /// <summary>delete other extended payload</summary>
  virtual void deleteTransOthers( TlmBasicPayload &trans );

  /// <summary>setting function for basic transaction</summary>
  /// <param name="inTrans">input transaction </param>
  /// <param name="outTrans">output transaction </param>
  void setTransBasic( TlmBasicPayload &inTrans,
		      TlmBasicPayload &outTrans );

  /// <summary>setting function for G3M extension transaction</summary>
  /// <param name="inTrans">input transaction </param>
  /// <param name="outTrans">output transaction </param>
  void setTransG3m( TlmBasicPayload &inTrans,
		    TlmBasicPayload &outTrans );

  /// <summary>setting function for other extension transaction</summary>
  /// <param name="inTrans">input transaction </param>
  /// <param name="outTrans">output transaction </param>
  virtual void setTransOthers( TlmBasicPayload &inTrans,
			       TlmBasicPayload &outTrans );

  /// <summary>notify transaction completion</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  virtual void notifyTransComp( TlmBasicPayload &trans, BusTime_t &t );

  /// <summary>read function</summary>
  /// <param name="offsetAddress">offset address</param>
  /// <param name="trans">generic payload</param>
  /// <param name="t">time</param>
  /// <param name="debug">debug access</param>
  /// <param name="isWrite">read or write</param>
  virtual void sendAfterSplit( unsigned int     offsetAddress, 
			       TlmBasicPayload& trans, 
			       BusTime_t*       t,
			       bool             isWrite=false,
			       bool             debug=false );

 private:
  std::map<TlmBasicPayload*,TlmBasicPayload*> mRequestMap; // Bus request map
  std::map<TlmBasicPayload*,unsigned int>     mDataNmbMap;  // Data number map
  std::queue<TlmBasicPayload *>  mClearTransFifo; // clear transaction list
  double mFwLatency; // latency for forward path
  double mBwLatency; // latency for backward path

  bool mTransSplit;  // if needed to convert from burst to single
  unsigned int mBusWidthInByte; // bus width in byte
  
};


template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
BusBridgeCoreBase( BusBridgeModuleBase<S_BUSWIDTH,M_BUSWIDTH> *parent, 
		   BusBridgeSlaveIf<S_BUSWIDTH>    *slaveIf,
		   BusMasterIf<M_BUSWIDTH>         *masterIf ):
  BusSlaveFuncIf(),
  BusMasterFuncIf(),
  mMasterIf( masterIf ),
  mSlaveIf( slaveIf ), 
  mParent( parent ),
  mTransType( (TransMode_t)AT ),
#ifdef BUSIF_TST
  tstFp( stdout ),
#endif //BUSIF_TST
  mFwLatency( (double)0 ),
  mBwLatency( (double)0 ),
  mTransSplit( false ),
  mBusWidthInByte( M_BUSWIDTH/8 )
{
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
~BusBridgeCoreBase( void ){}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
read( unsigned int     offsetAddress, 
      TlmBasicPayload& trans, 
      BusTime_t*       t,
      bool             debug )
{
  *t += toBusTime( mFwLatency*mSlaveIf->getClockPeriod() );

  if( mTransSplit && !debug ){
    sendAfterSplit( offsetAddress, trans, t, false, debug );
  }
  else{
    if( debug || mTransType==LT ){
      mMasterIf->read( trans, debug, *t, false );
      if( !debug ){
	*t += toBusTime( mBwLatency*mSlaveIf->getClockPeriod() );
      }
    }
    else{
      TlmBasicPayload *sendTrans = new TlmBasicPayload;
      setTransBasic( trans, *sendTrans );
      setTransG3m( trans, *sendTrans );
      setTransOthers( trans, *sendTrans );
      mRequestMap[sendTrans] = &trans;
      mMasterIf->read( *sendTrans, debug, *t, false );
    }
  }
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
write( unsigned int     offsetAddress, 
       TlmBasicPayload& trans, 
       BusTime_t*       t,
       bool             debug )
{
  *t += toBusTime( mFwLatency*mSlaveIf->getClockPeriod() );

  if( mTransSplit && !debug ){
    sendAfterSplit( offsetAddress, trans, t, true, debug );
  }
  else{
    if( debug || mTransType==LT ){
      mMasterIf->write( trans, debug, *t, false );
      if( !debug ){
	*t += toBusTime( mBwLatency*mSlaveIf->getClockPeriod() );
      }
    }
    else{
      TlmBasicPayload *sendTrans = new TlmBasicPayload;
      setTransBasic( trans, *sendTrans );
      setTransG3m( trans, *sendTrans );
      setTransOthers( trans, *sendTrans );
      mRequestMap[sendTrans] = &trans;
      mMasterIf->write( *sendTrans, debug, *t, false );
    }
  }
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
reset( void )
{
  deleteAllTrans();
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
deleteTrans( void )
{
  TlmBasicPayload *trans;
  TlmG3mExtension *G3mExt;

  while( !mClearTransFifo.empty() ){
    trans = mClearTransFifo.front();

    G3mExt = (TlmG3mExtension *)0;
    trans->get_extension( G3mExt );
    trans->release_extension( G3mExt );
    
    deleteTransOthers( *trans );
    
    delete trans;
    mRequestMap.erase( trans );

    mClearTransFifo.pop();
#ifdef BUSIF_TST
    fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#ifdef PLTFRM_TST
    fprintf(tstFp,"%s\n        ",mParent->name());
#endif //PLTFRM_TST
    fprintf(tstFp,
	    "BusBridgeCoreBase::release_externsion(G3mExt) was called\n                           and payload object was deleted for 0x%x\n",
	    trans);
#endif //BUSIF_TST
  }
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
deleteAllTrans( void )
{
  std::map< TlmBasicPayload*,TlmBasicPayload* >::iterator it
    = mRequestMap.begin();
  while( it != mRequestMap.end() ){
    mClearTransFifo.push( (*it).first );
    ++it;
  }
  deleteTrans();
  mDataNmbMap.clear();
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
setTlmTransType( SIM_MODE_T simMode )
{
  switch( simMode ){
  case SIM_MODE_CA:
    mTransType = AT;
    break;
  case SIM_MODE_FAST:
    mTransType = LT;
    break;
  }
  mMasterIf->setTlmTransType( simMode );
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
setBridgeFwLatency( uint64 latency )
{
  mFwLatency = (double)latency;
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
setBridgeBwLatency( uint64 latency )
{
  mBwLatency = (double)latency;
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
setTransSplitter( bool transSplit )
{
  mTransSplit = transSplit;
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
deleteTransOthers( TlmBasicPayload &trans )
{
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
setTransBasic( TlmBasicPayload &inTrans, TlmBasicPayload &outTrans)
{
  outTrans.set_address( inTrans.get_address() );
  outTrans.set_data_ptr( inTrans.get_data_ptr() );
  outTrans.set_data_length( inTrans.get_data_length() );
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
setTransG3m( TlmBasicPayload &inTrans, TlmBasicPayload &outTrans )
{
  TlmG3mExtension *inG3mExt  = (TlmG3mExtension *)0;
  TlmG3mExtension *outG3mExt = new TlmG3mExtension;
  inTrans.get_extension( inG3mExt );
  if( inG3mExt == NULL ){
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf,
	     "There is no TlmG3mExtension payload." );
    CmErrMsg::printErrorMessage( mParent->name(), 0, errorMessageBuf );
  }
  else{
    outG3mExt->setDmaAccess( inG3mExt->isDmaAccess() );
    outG3mExt->setVmId( inG3mExt->getVmId() );
    outG3mExt->setTcId( inG3mExt->getTcId() );
    outG3mExt->setPeId( inG3mExt->getPeId() );
    outG3mExt->setSpId( inG3mExt->getSpId() );
    outG3mExt->setUserMode( inG3mExt->isUserMode() );
    outG3mExt->setVirtualMode( inG3mExt->isVirtualMode() );
    outG3mExt->setSpcLd( inG3mExt->isSpcLd() );
    outG3mExt->setSecure( inG3mExt->isSecure() );
    outTrans.set_extension( outG3mExt );
  }
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
setTransOthers( TlmBasicPayload &inTrans, TlmBasicPayload &outTrans )
{
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
notifyTransComp( TlmBasicPayload &trans, BusTime_t &t )
{
  if( !mRequestMap.empty() ){
    if( mTransSplit ){
      mDataNmbMap[mRequestMap[&trans]] -= 1;
      if( !mDataNmbMap[mRequestMap[&trans]] ){
	BusTime_t tForUpper = 
	  t + toBusTime( mBwLatency*mSlaveIf->getClockPeriod() );
	mDataNmbMap.erase( mRequestMap[&trans] );
	mSlaveIf->notifyTransComp( *mRequestMap[&trans], tForUpper );
      }
    }
    else{
      BusTime_t tForUpper = 
	t + toBusTime( mBwLatency*mSlaveIf->getClockPeriod() );
      mSlaveIf->notifyTransComp( *mRequestMap[&trans], tForUpper );
    }
    mParent->deleteTrans();
    mClearTransFifo.push( &trans );
  }
  else{
    BusTime_t tForUpper = 
      t + toBusTime( mBwLatency*mSlaveIf->getClockPeriod() );
    mSlaveIf->notifyTransComp( trans, tForUpper );
  }
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
sendAfterSplit( unsigned int     offsetAddress, 
		TlmBasicPayload& trans, 
		BusTime_t*       t,
		bool             isWrite,
		bool             debug )
{
  sc_dt::uint64 address   = trans.get_address();
  unsigned char *dataPtr  = trans.get_data_ptr();
  unsigned int dataLength = trans.get_data_length();
  if( debug || mTransType==LT ){
    sc_dt::uint64 startAddress   = address;
    unsigned char *startDataPtr  = dataPtr;
    unsigned int startDataLength = dataLength;
    while(1){
      trans.set_address( address );
      trans.set_data_ptr( dataPtr );
      if( dataLength > mBusWidthInByte ){
	trans.set_data_length( mBusWidthInByte );
      }
      else{
	trans.set_data_length( dataLength );
      }
      if( isWrite ){
	mMasterIf->write( trans, debug, *t, false );
      }
      else{
	mMasterIf->read( trans, debug, *t, false );
      }
      if( dataLength > mBusWidthInByte ){
	address += mBusWidthInByte;
	dataPtr += mBusWidthInByte;
	dataLength -= mBusWidthInByte;
      }
      else{
	break;
      }
    }
    trans.set_address(startAddress);
    trans.set_data_ptr(startDataPtr);
    trans.set_data_length(startDataLength); 
    if( !debug ){
      *t += toBusTime( mBwLatency*mSlaveIf->getClockPeriod() );
    }
  }
  else{
    mDataNmbMap[&trans] = 1;
    while(1){
      TlmBasicPayload *sendTrans = new TlmBasicPayload;
      setTransG3m( trans, *sendTrans );
      setTransOthers( trans, *sendTrans );
      mRequestMap[sendTrans] = &trans;
      sendTrans->set_address( address );
      sendTrans->set_data_ptr( dataPtr );
      if( dataLength > mBusWidthInByte ){
	sendTrans->set_data_length( mBusWidthInByte );
      }
      else{
	sendTrans->set_data_length( dataLength );
      }
      if( isWrite ){
	mMasterIf->write( *sendTrans, debug, *t, false );
      }
      else{
	mMasterIf->read( *sendTrans, debug, *t, false );
      }
      if( dataLength > mBusWidthInByte ){
	address += mBusWidthInByte;
	dataPtr += mBusWidthInByte;
	dataLength -= mBusWidthInByte;
	mDataNmbMap[&trans] += 1;
      }
      else{
	break;
      }
    }
  }
}

#ifdef BUSIF_TST
template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
setLogFileP( FILE *fp )
{
  tstFp = fp;
}
#endif //BUSIF_TST


#endif // !__BUS_BRIDGE_CORE_BASE_H__
