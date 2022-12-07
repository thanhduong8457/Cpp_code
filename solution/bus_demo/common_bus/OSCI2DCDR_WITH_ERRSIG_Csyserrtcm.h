/*************************************************************************
 *
 *  OSCI2DCDR_WITH_ERRSIG_Csyserrtcm.h
 *
 *  Copyright(c) 2014 Renesas Electronics Corporation
 *  RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 *  This program must be used solely for the purpose for which
 *  it was furnished by Renesas Electronics Corporation. No part of this
 *  program may be reproduced or disclosed to others, in any
 *  form, without the prior written permission of Renesas Electronics
 *  Corporation.
 *
 *************************************************************************/

#ifndef __OSCI2DCDR_WITH_ERRSIG_Csyserrtcm_H__
#define __OSCI2DCDR_WITH_ERRSIG_Csyserrtcm_H__

#include "OSCI2DCDR_WITH_ERRSIG_BASE.h"
#include "SysErrTCM.h"
#include <tlm_utils/peq_with_get.h>

extern sc_time_unit glb_resolution_unit;

template<unsigned int BUSWIDTH>
class OSCI2DCDR_WITH_ERRSIG_Csyserrtcm :
public OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, Csyserrtcm>
{
 private:
  enum CoreType_T {
    CORE_G3M30,    // CC-G3M 3.0
    CORE_G3M20,    // G3M 2.0
    CORE_G3M10,    // G3M 1.0
    CORE_G3K_PCU   // G3K for PCU
  };

  Csyserrtcm mErrVal;   // resperr port's value
  CoreType_T mCoreType; // core type

 public:
  SC_HAS_PROCESS(OSCI2DCDR_WITH_ERRSIG_Csyserrtcm);
  /// <summary>constructer</summary>
  /// <param name="moduleName">name of sc_module_name</param>
  /// <param name="pMapFileName">map file name</param>
#if defined(BUSIF_TST)
  OSCI2DCDR_WITH_ERRSIG_Csyserrtcm( sc_module_name  moduleName,
			      const char     *pMapFileName  = 0,
			      FILE           *fp = 0 );
#else // BUSIF_TST
  OSCI2DCDR_WITH_ERRSIG_Csyserrtcm( sc_module_name  moduleName,
                              const char     *pMapFileName  = 0 );
#endif// BUSIF_TST

  /// <summary>destructor</summary>
  ~OSCI2DCDR_WITH_ERRSIG_Csyserrtcm( void );
  
  /// <summary>initialize resperr port</summary>
  void resperr_initialize();

  /// <summary>set resperr port</summary>
  /// <param name="trans">transaction</param>
  virtual void resperr_set(TlmTransactionType& trans);

  /// <summary>clear resperr port</summary>
  virtual void resperr_clear();

  /// <summary>set core type</summary>
  /// <param name="type">core type</param>
  void set_cpu_type(CoreType_T type);
};


template<unsigned int BUSWIDTH>
OSCI2DCDR_WITH_ERRSIG_Csyserrtcm<BUSWIDTH>::
#if defined(BUSIF_TST)
OSCI2DCDR_WITH_ERRSIG_Csyserrtcm( sc_module_name  moduleName,
				  const char     *pMapFileName,
				  FILE           *fp ) :
  OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, Csyserrtcm>( moduleName, pMapFileName, fp ),
#else // BUSIF_TST
OSCI2DCDR_WITH_ERRSIG_Csyserrtcm( sc_module_name  moduleName,
				  const char     *pMapFileName ) :
  OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, Csyserrtcm>( moduleName, pMapFileName ),
#endif// BUSIF_TST
  mCoreType(CORE_G3M30)
{
}


template<unsigned int BUSWIDTH>
OSCI2DCDR_WITH_ERRSIG_Csyserrtcm<BUSWIDTH>::~OSCI2DCDR_WITH_ERRSIG_Csyserrtcm( void )
{
}


template<unsigned int BUSWIDTH>
void 
OSCI2DCDR_WITH_ERRSIG_Csyserrtcm<BUSWIDTH>::
resperr_initialize()
{
  mErrVal.Initialize();
  OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, Csyserrtcm>::resperr.write(mErrVal);
}

template<unsigned int BUSWIDTH>
void 
OSCI2DCDR_WITH_ERRSIG_Csyserrtcm<BUSWIDTH>::
resperr_set(TlmTransactionType& trans)
{
  TlmG3mExtension *G3mExt = NULL;
  trans.get_extension( G3mExt );
  TlmVciExtension *VciExt = NULL;
  trans.get_extension( VciExt );
  
  unsigned int peid = (unsigned int)G3mExt->getPeId();
  unsigned int tcid = (unsigned int)G3mExt->getTcId();
  unsigned int vcid = (unsigned int)G3mExt->getVmId();
  unsigned int spid = (unsigned int)G3mExt->getSpId();
  unsigned int isVM = (unsigned int)G3mExt->isVirtualMode();
  unsigned int isUM = (unsigned int)G3mExt->isUserMode();
  unsigned int sb   = 0;
  unsigned int rtype_ev = 0;
  
  if(mCoreType == CORE_G3M30){
    sb = ((peid & 0x7) << 13) | 
      ((tcid & 0x7) << 10) |
      ((vcid & 0x7) << 7) |
      ((spid & 0x1f) << 2) |
      ((isUM & 0x1) << 1) |
      (isVM &  0x1);
  }else{
    sb = ((tcid & 0x3f) << 10) | 
      ((vcid & 0x7) << 7) |
      ((peid & 0x7) << 4) |
      ((spid & 0x3) << 2) |
      ((isUM & 0x1) << 1) |
      (isVM &  0x1);
    }
  
  switch(trans.get_data_length()){
  case 1:
    rtype_ev = 0x00000000;
    break;
  case 2:
    rtype_ev = 0x00010000;
    break;
  case 4:
    rtype_ev = 0x00020000;
    break;
  case 8:
    rtype_ev = 0x00030000;
    break;
  default:
    rtype_ev = 0x00020000;
    break;
  }
  
  switch(VciExt->getRequestType()){
  case TlmVciExtension::CMD_READ: 
    break;
  case TlmVciExtension::CMD_WRITE:
    rtype_ev = rtype_ev | 0x00040000;
    break;
  case TlmVciExtension::CMD_BITOP_SET:
    rtype_ev = 0x00080000;
    break;
  case TlmVciExtension::CMD_BITOP_AND:
    rtype_ev = 0x00090000;
    break;
  case TlmVciExtension::CMD_BITOP_OR:
    rtype_ev = 0x000a0000;
    break;
  case TlmVciExtension::CMD_BITOP_XOR:
    rtype_ev = 0x000b0000;
    break;
  case TlmVciExtension::CMD_SYNC_BUS:
    rtype_ev = 0x000f0000;
    break;
  case TlmVciExtension::CMD_SYNC_MASTER:
    rtype_ev = 0x000e0000;
    break;
  case TlmVciExtension::CMD_CAXI:;
    rtype_ev = 0x00100000;
    break;
  case TlmVciExtension::CMD_EXCL_LDLW:
    rtype_ev = 0x00140000;
    break;
  case TlmVciExtension::CMD_EXCL_STCW:
    rtype_ev = 0x00150000;
    break;
  default:
    rtype_ev = 0x00000000;
    break;
  }

  mErrVal.SysErrTCM     = true;
  mErrVal.SysErrTCMVld  = true;
  mErrVal.SysErrTCMInfo = 0x20000000 | rtype_ev | (sb & 0xffff);
  mErrVal.SysErrTCMAdr  = (unsigned int)trans.get_address();
  OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, Csyserrtcm>::resperr.write(mErrVal);
}

template<unsigned int BUSWIDTH>
void 
OSCI2DCDR_WITH_ERRSIG_Csyserrtcm<BUSWIDTH>::
resperr_clear()
{
  mErrVal.Initialize();
  OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, Csyserrtcm>::resperr.write(mErrVal);
}

template<unsigned int BUSWIDTH>
void
OSCI2DCDR_WITH_ERRSIG_Csyserrtcm<BUSWIDTH>::
set_cpu_type(CoreType_T type)
{
  mCoreType = type;
}


#endif // !__OSCI2DCDR_WITH_ERRSIG_Csyserrtcm_H__
