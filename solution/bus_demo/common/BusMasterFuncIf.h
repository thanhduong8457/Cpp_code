/*
* Copyright(c) 2011 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*/

#ifndef __BUS_MASTER_FUNC_IF_H__
#define __BUS_MASTER_FUNC_IF_H__

#include "OSCI2.h"     // class for User Modeling environment based on TLM2.0
#include "CmErrMsg.h"  // for error message

class BusMasterFuncIf
{

 public:
  /// <summary>constructer</summary>
  BusMasterFuncIf( void ){}

  /// <summary>destructer</summary>
  ~BusMasterFuncIf( void ){}

  /// <summary>notify transaction sending</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  virtual void notifyTransSend( TlmTransactionType &trans, 
#if 0 // notify error to core if necessary
				tlm::tlm_response_status status,
#endif// notify error to core if necessary
				BusTime_t &t ){};

  /// <summary>notify transaction completion</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  virtual void notifyTransComp( TlmTransactionType &trans, 
#if 0 // notify error to core if necessary
				tlm::tlm_response_status status,
#endif// notify error to core if necessary
				BusTime_t &t ){};

};

#endif //__BUS_MASTER_FUNC_IF_H__
