/*
* Copyright(c) 2011 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*/

#ifndef __BUS_SLAVE_FUNC_IF_H__
#define __BUS_SLAVE_FUNC_IF_H__

#include "OSCI2.h"     // class for User Modeling environment based on TLM2.0
#include "CmErrMsg.h"  // for error message

class BusSlaveFuncIf
{

 public:
  /// <summary>constructer</summary>
  BusSlaveFuncIf( void ){}

  /// <summary>destructer</summary>
  ~BusSlaveFuncIf( void ){}

  /// <summary>read</summary>
  /// <param name="offsetAddress">offset address</param>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  /// <param name="debug">debug access</param>
  virtual void read( unsigned int     offsetAddress, 
		     TlmBasicPayload& trans, 
		     BusTime_t*       t,
		     bool             debug=false ) = 0;

  /// <summary>write</summary>
  /// <param name="offsetAddress">offset address</param>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  /// <param name="debug">debug access</param>
  virtual void write( unsigned int     offsetAddress, 
		      TlmBasicPayload& trans, 
		      BusTime_t*       t,
		      bool             debug=false ) = 0;

#if 0 //DMI is not supported
  virtual unsigned char *getMemPtr(void){} //use only for DMI
#endif//DMI is not supported

  /// <summary>write</summary>
  /// <param name="size">setting function for base address and size</param>
  virtual void setfunc( ADDRESS_TYPE size ){}

};

#endif //__BUS_SLAVE_FUNC_IF_H__
