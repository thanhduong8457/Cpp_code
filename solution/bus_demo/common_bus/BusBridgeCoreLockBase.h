/*************************************************************************
 *
 *  BusBridgeCoreLockBase.h
 *
 *  Copyright(c) 2013 Renesas Electronics Corporation
 *  RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 *  This program must be used solely for the purpose for which
 *  it was furnished by Renesas Electronics Corporation. No part of this
 *  program may be reproduced or disclosed to others, in any
 *  form, without the prior written permission of Renesas Electronics
 *  Corporation.
 *
 *************************************************************************/

#ifndef __BUS_BRIDGE_CORE_LOCK_BASE_H__
#define __BUS_BRIDGE_CORE_LOCK_BASE_H__

#include <vector>

#include "BusBridgeCoreBase.h"

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
class BusBridgeCoreLockBase : 
public BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>
{
 public:
  /// <summary>constructor</summary>
  /// <param name="parent">pointer of parent class</param>
  /// <param name="slaveIf">pointer of bus slave I/F</param>
  /// <param name="masterIf">pointer of bus master I/F</param>
  BusBridgeCoreLockBase( BusBridgeModuleBase<S_BUSWIDTH,M_BUSWIDTH> *parent,
			 BusBridgeSlaveIf<S_BUSWIDTH>    *slaveIf,
			 BusMasterIf<M_BUSWIDTH>         *masterIf );

  /// <summary>destructor</summary>
  ~BusBridgeCoreLockBase(void);

  /// <summary>read function</summary>
  /// <param name="offsetAddress">offset address</param>
  /// <param name="trans">generic payload</param>
  /// <param name="t">time</param>
  /// <param name="debug">debug access</param>
  void read( unsigned int     offsetAddress, 
	     TlmBasicPayload& trans, 
	     BusTime_t*       t,
	     bool             debug=false );

  /// <summary>write function</summary>
  /// <param name="offsetAddress">offset address</param>
  /// <param name="trans">generic payload</param>
  /// <param name="t">time</param>
  /// <param name="debug">debug access</param>
  void write( unsigned int     offsetAddress, 
	      TlmBasicPayload& trans, 
	      BusTime_t*       t,
	      bool             debug=false );

  /// <summary>reset function</summary>
  void reset( void );

  /// <summary>check if the transaction is Read-Modify-Write</summary>
  /// <param name="trans">generic payload</param>
  /// <returns>check result</returns>
  virtual bool checkIfRMW( TlmBasicPayload& trans );
  
 private:
  /// <summary>bus master ID</summary>
  struct busMasterId
  {
    unsigned char VmId;
    unsigned char TcId;
    unsigned char PeId;
    busMasterId( void )
    {
      VmId = 0;
      TcId = 0;
      PeId = 0;
    }
  } mLockerId; // locking bus master ID

  bool mLock; // lock flag
  std::vector< TlmBasicPayload* > mReservedTransList; //reserved transactions

  /// <summary>check if the transaction is sent by locking master</summary>
  /// <param name="trans">generic payload</param>
  /// <returns>check result</returns>
  bool checkIfLocker( TlmBasicPayload& trans );
  
  /// <summary>set locking master</summary>
  /// <param name="trans">generic payload</param>
  void setLocker( TlmBasicPayload& trans );
  
};


template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
BusBridgeCoreLockBase<S_BUSWIDTH,M_BUSWIDTH>::
BusBridgeCoreLockBase( BusBridgeModuleBase<S_BUSWIDTH,M_BUSWIDTH> *parent, 
		       BusBridgeSlaveIf<S_BUSWIDTH>    *slaveIf,
		       BusMasterIf<M_BUSWIDTH>         *masterIf ):
  BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>( parent, slaveIf, masterIf ),
  mLockerId(),
  mLock(false)
{}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
BusBridgeCoreLockBase<S_BUSWIDTH,M_BUSWIDTH>::
~BusBridgeCoreLockBase( void ){}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void
BusBridgeCoreLockBase<S_BUSWIDTH,M_BUSWIDTH>::
read( unsigned int     offsetAddress, 
      TlmBasicPayload& trans, 
      BusTime_t*       t,
      bool             debug )
{
  if( debug || 
      BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::mTransType==
      BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::LT ){
    BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
      read( offsetAddress, trans, t, debug );
  }
  else{
    if( !mLock ){
      if( checkIfRMW( trans ) ){
	mLock = true;
	setLocker( trans );
      }
      BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
	read( offsetAddress, trans, t, debug );
    }
    else{
      if( checkIfLocker( trans ) ){
	BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
	  read( offsetAddress, trans, t, debug );
      }
      else{
	mReservedTransList.push_back( &trans );
      }
    }
  }
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void
BusBridgeCoreLockBase<S_BUSWIDTH,M_BUSWIDTH>::
write( unsigned int     offsetAddress, 
       TlmBasicPayload& trans, 
       BusTime_t*       t,
       bool             debug )
{
  if( debug || 
      BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::mTransType==
      BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::LT ){
    BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
      write( offsetAddress, trans, t, debug );
  }
  else{
    if( !mLock ){
      BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
	write( offsetAddress, trans, t, debug );
    }
    else{
      if( checkIfLocker( trans ) ){
	BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
	  write( offsetAddress, trans, t, debug );
	if( checkIfRMW( trans ) ){
	  mLock = false;
	  unsigned int i = 0;
	  BusTime_t tForReserve;
	  TlmBasicPayload *reservedTrans;
	  while(1){
	    tForReserve = SC_ZERO_TIME;
	    unsigned int vecSize = mReservedTransList.size();
	    if( !vecSize || i>=vecSize ){
	      break;
	    }
	    reservedTrans = mReservedTransList[i];
	    if( !mLock ){
	      if( reservedTrans->is_read() ){
		if( checkIfRMW( *reservedTrans ) ){
		  mLock = true;
		  setLocker( *reservedTrans );
		}
		BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
		  read( offsetAddress, *reservedTrans, &tForReserve, debug );
	      }
	      else{
		BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
		  write( offsetAddress, *reservedTrans, &tForReserve, debug );
	      }
	      mReservedTransList.erase( mReservedTransList.begin()+i );
	    }
	    else{
	      if( checkIfLocker( *reservedTrans ) ){
		if( reservedTrans->is_write() ){
		  if( checkIfRMW( *reservedTrans ) ){
		    mLock = false;
		    i = 0;
		  }
		  BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
		    write( offsetAddress, *reservedTrans, &tForReserve, debug );
		}
		else{
		  BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::
		    read( offsetAddress, *reservedTrans, &tForReserve, debug );
		}
		mReservedTransList.erase( mReservedTransList.begin()+i );
	      }
	      else{
		i++;
	      }
	    }
	  }
	}
      }
      else{
	mReservedTransList.push_back( &trans );
      }
    }
  }
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreLockBase<S_BUSWIDTH,M_BUSWIDTH>::
reset( void )
{
  mLockerId.VmId = 0;
  mLockerId.TcId = 0;
  mLockerId.PeId = 0;
  mLock = false;
  mReservedTransList.clear();
#ifdef BUSIF_TST
    fprintf(BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::tstFp,"%s:",sc_time_stamp().to_string().c_str());
#ifdef PLTFRM_TST
    fprintf(BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::tstFp,"%s\n        ",BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::mParent->name());
#endif //PLTFRM_TST
    fprintf(BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::tstFp,
	    "BusBridgeCoreLockBase::mLockerId and mLock were initialized.\n                               mReservedTrans.List.clear() was called\n");
#endif //BUSIF_TST
  BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::reset();
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
bool 
BusBridgeCoreLockBase<S_BUSWIDTH,M_BUSWIDTH>::
checkIfRMW( TlmBasicPayload& trans )
{
  return false;
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
bool 
BusBridgeCoreLockBase<S_BUSWIDTH,M_BUSWIDTH>::
checkIfLocker( TlmBasicPayload& trans )
{
  TlmG3mExtension *pG3mExtension = NULL;
  trans.get_extension( pG3mExtension );

  if( !pG3mExtension ){
    // error (not support TlmG3mExtension)
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf,
	     "Received transaction doesn't support the TlmG3mExtension." );
    CmErrMsg::printErrorMessage( BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::mParent->name(),
				 0, errorMessageBuf );
    return false;
  }

  if( mLockerId.VmId == pG3mExtension->getVmId() &&
      mLockerId.TcId == pG3mExtension->getTcId() &&
      mLockerId.PeId == pG3mExtension->getPeId() ){
    return true;
  }
  else{
    return false;
  }
}

template<unsigned int S_BUSWIDTH,unsigned int M_BUSWIDTH>
void 
BusBridgeCoreLockBase<S_BUSWIDTH,M_BUSWIDTH>::
setLocker( TlmBasicPayload& trans )
{
  TlmG3mExtension *pG3mExtension = NULL;
  trans.get_extension( pG3mExtension );

  if( !pG3mExtension ){
    // error (not support TlmG3mExtension)
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf,
	     "Received transaction doesn't support the TlmG3mExtension." );
    CmErrMsg::printErrorMessage( BusBridgeCoreBase<S_BUSWIDTH,M_BUSWIDTH>::mParent->name(),
				 0, errorMessageBuf );
    return;
  }

  mLockerId.VmId = pG3mExtension->getVmId();
  mLockerId.TcId = pG3mExtension->getTcId();
  mLockerId.PeId = pG3mExtension->getPeId();
}


#endif // !__BUS_BRIDGE_CORE_BASE_H__
