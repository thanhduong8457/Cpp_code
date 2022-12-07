/*
* Copyright(c) 2011 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*/

#ifndef __BUSBRIDGESLAVEIF_H__
#define __BUSBRIDGESLAVEIF_H__

#include "CmErrMsg.h"  // for error message
#include "BusSlaveIf.h"

extern sc_time_unit glb_resolution_unit;

template<unsigned int BUSWIDTH>
class BusBridgeSlaveIf :
public BusSlaveIf<BUSWIDTH>
{
 public:
  SC_HAS_PROCESS(BusBridgeSlaveIf);
  /// <summary>constructor</summary>
  /// <param name="name">name of sc_module</param>
  /// <param name="resestPort">reset port</param>
  /// <param name="freqPort">frequency port</param>
  /// <param mame="tSocket">target socket</param>
  BusBridgeSlaveIf(sc_module_name  name,
	     sc_in<bool>     &resetPort,
	     sc_in<uint64>   &freqPort,
#if 0 //DMI is not supported
	     TlmTargetSocket<BUSWIDTH> &tSocket,
	     bool            invalidate = false ):
#else //DMI is not supported
	     TlmTargetSocket<BUSWIDTH> &tSocket ):
#endif//DMI is not supported
    BusSlaveIf<BUSWIDTH>( name, resetPort, freqPort, tSocket )
  {
  }

  /// <summary>destructor</summary>
  ~BusBridgeSlaveIf(void){}

  /// <summary>transport API for debug access</summary>
  /// <param name="r">transaction</param>
  /// <returns>data number</returns>
  unsigned int transport_dbg(TlmTransactionType& r)
  {
    
#ifdef BUSIF_TST
    if(BusSlaveIf<BUSWIDTH>::tstFp){
      fprintf(BusSlaveIf<BUSWIDTH>::tstFp,"%s:",
	      sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
      fprintf(BusSlaveIf<BUSWIDTH>::tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(BusSlaveIf<BUSWIDTH>::tstFp,
	      "BusBridgeSlaveIf::transport_dbg() is called from %s\n",
	      BusSlaveIf<BUSWIDTH>::targetSocket.name());
      BusSlaveIf<BUSWIDTH>::outputLogAtRecep( &r );
    }
#endif //BUSIF_TST

    unsigned int num_bytes;
    BusTime_t t = SC_ZERO_TIME;
    unsigned int offsetAddress = 
      (unsigned int)(r.get_address()-BusSlaveIf<BUSWIDTH>::targetSocket.getBaseAddress());
    
    num_bytes = r.get_data_length();
    
    if( r.get_command() == tlm::TLM_WRITE_COMMAND ){
      BusSlaveIf<BUSWIDTH>::mpFuncModule->write( offsetAddress,
			   r,
			   &t,
			   true );
    }
    else{
      BusSlaveIf<BUSWIDTH>::mpFuncModule->read( offsetAddress,
			  r,
			  &t,
			  true );
    }
    
    r.set_response_status(tlm::TLM_OK_RESPONSE);
    
    return num_bytes;
  }
  
private:
  /// <summary>notify response preprocess event</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  void notifyRespPreEvent( TlmTransactionType &trans, sc_time &t )
  {
  }
  
  /// <summary>check address</summary>
  /// <param name="trans">transaction</param>
  /// <returns>offset address</returns>
  unsigned int checkAddress( TlmTransactionType &trans )
  {
    unsigned int offsetAddress = (unsigned int)(trans.get_address() -
      BusSlaveIf<BUSWIDTH>::targetSocket.getBaseAddress());
    return ( offsetAddress );
  }

public:  
  /// <summary>notify trancastion completion</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  void notifyTransComp( TlmTransactionType &trans, 
#if 0 // notify error to core if necessary
			tlm::tlm_response_status status,
#endif// notify error to core if necessary
			BusTime_t &t )
  {
#ifdef BUSIF_TST
    if(BusSlaveIf<BUSWIDTH>::tstFp){
      fprintf(BusSlaveIf<BUSWIDTH>::tstFp,"%s:",
	      sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
      fprintf(BusSlaveIf<BUSWIDTH>::tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(BusSlaveIf<BUSWIDTH>::tstFp,
	      "BusBridgeSlaveIf::notifyTransComp() is called\n" );
      BusSlaveIf<BUSWIDTH>::outputLogAtResp( &trans );
      fprintf(BusSlaveIf<BUSWIDTH>::tstFp,"        t =%s\n",
	      t.to_string().c_str());
    }
#endif //BUSIF_TST
    BusSlaveIf<BUSWIDTH>::notifyRespPreEvent( trans, t );
  }

};

#endif //__BUSBRIDGESLAVEIF__
