/*************************************************************************
 *
 *  APISS.h
 *
 *  Copyright(c) 2015 Renesas Electronics Corporation
 *  RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 *  This program must be used solely for the purpose for which
 *  it was furnished by Renesas Electronics Corporation. No part of this
 *  program may be reproduced or disclosed to others, in any
 *  form, without the prior written permission of Renesas Electronics
 *  Corporation.
 *
 *************************************************************************/

#ifndef APISS_H
#define APISS_H

#include "global.h"

template<unsigned int> class BACKWARD_DECODER;
template<unsigned int> class OSCI2DCDR;

class APISS : 
public sc_module
{

 public:
  //TLM2 initiator socket
  TlmInitiatorSocket<BUS_WIDTH_VCI> is;
  //TLM2 targeet socket
  TlmTargetSocket<BUS_WIDTH_VCI>    ts;

  APISS( sc_module_name moduleName,
	 const char     *pMapFileName );

  ~APISS( void );

 private:
  BACKWARD_DECODER<BUS_WIDTH_VCI> *BW_DECODER;
  OSCI2DCDR<BUS_WIDTH_VCI>        *DECODER;
  
  
#ifdef BUS_TST
  FILE *fp;
#endif //BUS_TST

};

#endif
