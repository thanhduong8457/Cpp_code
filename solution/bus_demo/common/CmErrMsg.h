/*
* Copyright(c) 2011-2012 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*/

/* $Id:$ */

#ifndef CM_ERR_MSG_H
#define CM_ERR_MSG_H

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <systemc.h>

/*************************************
 *        namespace  CmErrMsg        *
 *************************************/

extern int scstopDoneFlag;

namespace CmErrMsg{
  
  void CmErrMsgInitSet(const char*);
  void printErrorMessage(const char*, const char*, const char*);
  void printWarningMessage(const char*, const char*, const char*);
  void CmErrMsgSetToFile(bool flag);
  
}


#endif
