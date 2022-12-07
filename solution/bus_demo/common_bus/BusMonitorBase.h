/*************************************************************************
 *
 *  BusMonitorBase.h
 *
 *  Copyright(c) 2014-2017 Renesas Electronics Corporation
 *  RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 *  This program must be used solely for the purpose for which
 *  it was furnished by Renesas Electronics Corporation. No part of this
 *  program may be reproduced or disclosed to others, in any
 *  form, without the prior written permission of Renesas Electronics
 *  Corporation.
 *
 *************************************************************************/

#ifndef __BUSMONITORBASE_H__
#define __BUSMONITORBASE_H__

#include "OSCI2.h"
#include "CmErrMsg.h"
#include "BusMasterIf.h"

extern ofstream bus_monitor_ofs; // ofstream for bus monitor

class BusMonitorBase
{
 public:
  /// <summary>TLM transport function type and direction</summary>
  typedef enum
  {
    TRANS_AT_REQ  = 0,
    TRANS_AT_RES  = 1,
    TRANS_LT_REQ  = 2,
    TRANS_LT_RES  = 3,
    TRANS_DBG_REQ = 4,
    TRANS_DBG_RES = 5
  } TransDir_t;

  /// <summary>constructer</summary>
  BusMonitorBase( void ):
    mMonitorOn(false),
    mMonitorLevel(0),
    mMonitorDebugOn(false),
    mMaster32(0),
    mMaster64(0),
    mMaster128(0),
    mG3mExt(0),
    mAhbExt(0),
    mApbExt(0),
    mAxiExt(0),
    mVciExt(0),
    mIndex(0)
  {};

  /// <summary>destructer</summary>
  ~BusMonitorBase( void )
  {};

  /// <summary>set bus monitor on</summary>
  /// <param name="level">monitor level</param>
  /// <param name="debug">if debug access is also monitored</param>
  void monitorOn( unsigned char level=0, char *debug=0 )
  {
    std::ostringstream msg;

    mMonitorDebugOn = false;

    if( !bus_monitor_ofs.is_open() ){ // File is not opened yet
      msg << "Need to open the file for bus monitor." << endl;
      CmErrMsg::printWarningMessage( "PY_BusMonitor", 
				     "MonitorOnPy", 
				     msg.str().c_str()   );
    }
    if( 2 < level ){ // undefined level
      msg << "Run with message level 0 because undefined message level ["
	  << (int)level << "] is specified." << endl;
      CmErrMsg::printWarningMessage( "PY_BusMonitor", 
				     "MonitorOnPy", 
				     msg.str().c_str()   );
      level = 0;
    }
    mMonitorOn      = true;
    mMonitorLevel   = level;
    if( debug ){
      if( !strcmp(debug,"debug") ){
	mMonitorDebugOn = true;
      }
    }
  };

  /// <summary>set bus monitor off</summary>
  void monitorOff( void )
  {
    mMonitorOn      = false;
    mMonitorDebugOn = false;
  };

  /// <summary>bus monitor log output</summary>
  /// <param name="trans">payload pointer</param>
  /// <param name="module">pointer of sc_module</param>
  void outputLog( TlmTransactionType *trans,
		  sc_module          *module,
		  TransDir_t          transDir,
		  sc_time            *t = 0 )
  {
    if( bus_monitor_ofs.is_open() && mMonitorOn &&
	( transDir<TRANS_DBG_REQ || mMonitorDebugOn ) ){ // LEVEL0
      
      bus_monitor_ofs << sc_time_stamp().to_string().c_str() << "," 
		      << module->name() << ","  ;
      switch( transDir ){
      case TRANS_AT_REQ:
	bus_monitor_ofs << "AT,REQ," ;
	break;
      case TRANS_AT_RES:
	bus_monitor_ofs << "AT,RES," ;
	break;
      case TRANS_LT_REQ:
	bus_monitor_ofs << "LT,REQ," ;
	break;
      case TRANS_LT_RES:
	bus_monitor_ofs << "LT,RES," ;
	break;
      case TRANS_DBG_REQ:
	bus_monitor_ofs << "debug,REQ," ;
	break;
      case TRANS_DBG_RES:
	bus_monitor_ofs << "debug,RES," ;
	break;
      default:
	bus_monitor_ofs << "," ;
	break;
      }
      if( t ){
	bus_monitor_ofs << t->to_string().c_str() << "," ;
      }
      else{
	bus_monitor_ofs << "0s," ;
      }
      bus_monitor_ofs << hex << trans << "," ;
      switch( trans->get_command() ){
      case tlm::TLM_READ_COMMAND:
	bus_monitor_ofs << "READ," ;
	break;
      case tlm::TLM_WRITE_COMMAND:
	bus_monitor_ofs << "WRITE," ;
	break;
      case tlm::TLM_IGNORE_COMMAND:
	bus_monitor_ofs << "IGNORE," ;
	break;
      default:
	bus_monitor_ofs << "," ;
	break;
      }
      bus_monitor_ofs << "0x" << trans->get_address() << "," ;
      unsigned int data_length = trans->get_data_length();
      bus_monitor_ofs << dec << data_length << "," ;
      unsigned char *data = trans->get_data_ptr();
      for( mIndex=0; mIndex<data_length; mIndex++ ){
	bus_monitor_ofs << "0x" ;
	if( data[mIndex] < 0x10 ){
	  bus_monitor_ofs << "0" ;
	}
	bus_monitor_ofs << hex << (unsigned int)(data[mIndex]) << " " ;
      }
      bus_monitor_ofs << "," << dec ;
      switch( trans->get_response_status() ){
      case tlm::TLM_OK_RESPONSE:
	bus_monitor_ofs << "TLM_OK_RESPONSE" ;
	break;
      case tlm::TLM_INCOMPLETE_RESPONSE:
	bus_monitor_ofs << "TLM_INCOMPLETE_RESPONSE" ;
	break;
      case tlm::TLM_GENERIC_ERROR_RESPONSE:
	bus_monitor_ofs << "TLM_GENERIC_ERROR_RESPONSE" ;
	break;
      case tlm::TLM_ADDRESS_ERROR_RESPONSE:
	bus_monitor_ofs << "TLM_ADDRESS_ERROR_RESPONSE" ;
	break;
      case tlm::TLM_COMMAND_ERROR_RESPONSE:
	bus_monitor_ofs << "TLM_COMMAND_ERROR_RESPONSE" ;
	break;
      case tlm::TLM_BURST_ERROR_RESPONSE:
	bus_monitor_ofs << "TLM_BURST_ERROR_RESPONSE" ;
	break;
      case tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE:
	bus_monitor_ofs << "TLM_BYTE_ENABLE_ERROR_RESPONSE" ;
	break;
      default:
	break;
      }
      
      if( mMonitorLevel > 0 ){ // LEVEL1
	
	bus_monitor_ofs << "," ;
	mG3mExt = (TlmG3mExtension *)0;
	trans->get_extension( mG3mExt );
	if( mG3mExt ){
	  BusMasterIf<32> *mMaster32
	    = dynamic_cast< BusMasterIf<32>* >(mG3mExt->getIfPointer());
	  if( mMaster32 ){
	    bus_monitor_ofs << mMaster32->name();
	  }
	  else{
	    BusMasterIf<64> *mMaster64
	      = dynamic_cast< BusMasterIf<64>* >(mG3mExt->getIfPointer());
	    if( mMaster64 ){
	      bus_monitor_ofs << mMaster64->name();
	    }
	    else{
	      BusMasterIf<128> *mMaster128
		= dynamic_cast< BusMasterIf<128>* >(mG3mExt->getIfPointer());
	      if( mMaster128 ){
		bus_monitor_ofs << mMaster128->name();
	      }
	    }
	  }
	  bus_monitor_ofs << "," ;
	  if( mG3mExt->isDmaAccess() ){
	    bus_monitor_ofs << "DMA," ;
	  }
	  else{
	    bus_monitor_ofs << "Not DMA," ;
	  }
	  bus_monitor_ofs << (int)(mG3mExt->getTcId()) << "," ;
	  bus_monitor_ofs << (int)(mG3mExt->getVmId()) << "," ;
	  bus_monitor_ofs << (int)(mG3mExt->getPeId()) << "," ;
	  bus_monitor_ofs << (int)(mG3mExt->getSpId()) << "," ;
	  if( mG3mExt->isUserMode() ){
	    bus_monitor_ofs << "User mode," ;
	  }
	  else{
	    bus_monitor_ofs << "Supervisor mode," ;
	  }
	  if( mG3mExt->isVirtualMode() ){
	    bus_monitor_ofs << "Virtual mode," ;
	  }
	  else{
	    bus_monitor_ofs << "Native mode," ;
	  }
	  if( mG3mExt->isSpcLd() ){
	    bus_monitor_ofs << "Speculative load," ;
	  }
	  else{
	    bus_monitor_ofs << "non-Speculative load," ;
	  }
	  if( mG3mExt->isSecure() ){
	    bus_monitor_ofs << "Secure" ;
	  }
	  else{
	    bus_monitor_ofs << "non-Secure" ;
	  }
	}
	else{
	  bus_monitor_ofs << ",,,,,,," ;
	}
	
	if( mMonitorLevel > 1 ){ // LEVEL2
	  
	  bus_monitor_ofs << "," ;
	  mAhbExt = (TlmAhbExtension *)0;
	  trans->get_extension( mAhbExt );
	  if( mAhbExt ){
	    switch( mAhbExt->getBurstType() ){
	    case TlmAhbExtension::BURST_FIXED:
	      bus_monitor_ofs << "Fixed," ;
	      break;
	    case TlmAhbExtension::BURST_INCR:
	      bus_monitor_ofs << "Increment," ;
	      break;
	    case TlmAhbExtension::BURST_WRAP:
	      bus_monitor_ofs << "Wrap," ;
	      break;
	    default:
	      bus_monitor_ofs << "," ;
	      break;
	    }
	    if( mAhbExt->isLock() ){
	      bus_monitor_ofs << "Lock," ;
	    }
	    else{
	      bus_monitor_ofs << "Not lock," ;
	    }
	    if( mAhbExt->isCachable() ){
	      bus_monitor_ofs << "Cachable," ;
	    }
	    else{
	      bus_monitor_ofs << "Not cachable," ;
	    }
	    if( mAhbExt->isBufferable() ){
	      bus_monitor_ofs << "Bufferable," ;
	    }
	    else{
	      bus_monitor_ofs << "Not bufferable," ;
	    }
	    if( mAhbExt->isPrivilege() ){
	      bus_monitor_ofs << "Privilege," ;
	    }
	    else{
	      bus_monitor_ofs << "Not privilege," ;
	    }
	    if( mAhbExt->getDataOrOp()
		== TlmAhbExtension::ACC_DATA ){
	      bus_monitor_ofs << "Access to data," ;
	    }
	    else{
	      bus_monitor_ofs << "Access to OP code," ;
	    }
	  }
	  else{
	    bus_monitor_ofs << ",,,,,," ;
	  }
	  
	  TlmApbExtension *mApbExt = (TlmApbExtension *)0;
	  trans->get_extension( mApbExt );
	  if( mApbExt ){
	    if( mApbExt->isLock() ){
	      bus_monitor_ofs << "Lock," ;
	    }
	    else{
	      bus_monitor_ofs << "Not lock," ;
	    }
	    if( mApbExt->isExclusion() ){
	      bus_monitor_ofs << "Exclusion," ;
	    }
	    else{
	      bus_monitor_ofs << "Not exclusion," ;
	    }
	  }
	  else{
	    bus_monitor_ofs << ",," ;
	  }
	  
	  mAxiExt = (TlmAxiExtension *)0;
	  trans->get_extension( mAxiExt );
	  if( mAxiExt ){
	    switch( mAxiExt->getBurstType() ){
	    case TlmAxiExtension::BURST_FIXED:
	      bus_monitor_ofs << "Fixed," ;
	      break;
	    case TlmAxiExtension::BURST_INCR:
	      bus_monitor_ofs << "Increment," ;
	      break;
	    case TlmAxiExtension::BURST_WRAP:
	      bus_monitor_ofs << "Wrap," ;
	      break;
	    default:
	      bus_monitor_ofs << "," ;
	      break;
	    }
	    if( mAxiExt->isLock() ){
	      bus_monitor_ofs << "Lock," ;
	    }
	    else{
	      bus_monitor_ofs << "Not lock," ;
	    }
	    if( mAxiExt->isCachable() ){
	      bus_monitor_ofs << "Cachable," ;
	    }
	    else{
	      bus_monitor_ofs << "Not cachable," ;
	    }
	    if( mAxiExt->isBufferable() ){
	      bus_monitor_ofs << "Bufferable," ;
	    }
	    else{
	      bus_monitor_ofs << "Not bufferable," ;
	    }
	    bus_monitor_ofs << "0x" << hex << (int)(mAxiExt->getSecure())
			    << "," ;
	    bus_monitor_ofs << "0x" << hex << (int)(mAxiExt->getTransId())
			    << "," ;
	    switch( mAxiExt->getBitOpType() ){
	    case TlmAxiExtension::BITOP_NON:
	      bus_monitor_ofs << "NON," ;
	      break;
	    case TlmAxiExtension::BITOP_SET1:
	      bus_monitor_ofs << "SET1," ;
	      break;
	    case TlmAxiExtension::BITOP_CLR1:
	      bus_monitor_ofs << "CLR1," ;
	      break;
	    case TlmAxiExtension::BITOP_NOT1:
	      bus_monitor_ofs << "NOT1," ;
	      break;
	    default:
	      bus_monitor_ofs << "," ;
	      break;
	    }
	    bus_monitor_ofs << dec << (int)(mAxiExt->getBitOpPos()) << "," ;
	  }
	  else{
	    bus_monitor_ofs << ",,,,,,,," ;
	  }
	  
	  mVciExt = (TlmVciExtension *)0;
	  trans->get_extension( mVciExt );
	  if( mVciExt ){
	    switch( mVciExt->getRequestType() ){
	    case TlmVciExtension::CMD_READ:
	      bus_monitor_ofs << "Read," ;
	      break;
	    case TlmVciExtension::CMD_WRITE:
	      bus_monitor_ofs << "Write," ;
	      break;
	    case TlmVciExtension::CMD_BITOP_SET:
	      bus_monitor_ofs << "BitOp(set)," ;
	      break;
	    case TlmVciExtension::CMD_BITOP_AND:
	      bus_monitor_ofs << "BitOp(and)," ;
	      break;
	    case TlmVciExtension::CMD_BITOP_OR:
	      bus_monitor_ofs << "BitOp(or)," ;
	      break;
	    case TlmVciExtension::CMD_BITOP_XOR:
	      bus_monitor_ofs << "BitOp(xor)," ;
	      break;
	    case TlmVciExtension::CMD_CAXI:
	      bus_monitor_ofs << "CAXI," ;
	      break;
	    case TlmVciExtension::CMD_EXCL_LDLW:
	      bus_monitor_ofs << "LDL.W," ;
	      break;
	    case TlmVciExtension::CMD_EXCL_STCW:
	      bus_monitor_ofs << "STC.W," ;
	      break;
	    default:
	      bus_monitor_ofs << "," ;
	      break;
	    }
	  }
	  else{
	    bus_monitor_ofs << "," ;
	  }
	  
	}
      }
      bus_monitor_ofs << endl;
    }
  };
  
 private:
  bool          mMonitorOn;      // bus monitor ON/OFF flag
  unsigned char mMonitorLevel;   // bus monitor level
  bool          mMonitorDebugOn; // bus monitor debug access log ON/OFF flag
  
  BusMasterIf<32>  *mMaster32;   // BusMasterIf pointer for 32bit bus
  BusMasterIf<64>  *mMaster64;   // BusMasterIf pointer for 64bit bus
  BusMasterIf<128> *mMaster128;  // BusMasterIf pointer for 128bit bus

  TlmG3mExtension *mG3mExt;      // G3M extenstion payload pointer
  TlmAhbExtension *mAhbExt;      // AHB extenstion payload pointer
  TlmApbExtension *mApbExt;      // APB extenstion payload pointer
  TlmAxiExtension *mAxiExt;      // AXI extenstion payload pointer
  TlmVciExtension *mVciExt;      // VCI extenstion payload pointer

  int mIndex;                    // index used for "for" loop
};


#endif // !__BUSMONITORBASE_H__
