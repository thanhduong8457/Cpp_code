/*************************************************************************
 *
 *  OSCI2DCDR_WITH_ERRSIG_bool.h
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

#ifndef __OSCI2DCDR_WITH_ERRSIG_bool_H__
#define __OSCI2DCDR_WITH_ERRSIG_bool_H__

#include "OSCI2DCDR_WITH_ERRSIG_BASE.h"
#include <tlm_utils/peq_with_get.h>

extern sc_time_unit glb_resolution_unit;

template<unsigned int BUSWIDTH>
class OSCI2DCDR_WITH_ERRSIG_bool :
public OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, bool>
{
 public:
  SC_HAS_PROCESS(OSCI2DCDR_WITH_ERRSIG_bool);
  /// <summary>constructer</summary>
  /// <param name="moduleName">name of sc_module_name</param>
  /// <param name="pMapFileName">map file name</param>
#if defined(BUSIF_TST)
  OSCI2DCDR_WITH_ERRSIG_bool( sc_module_name  moduleName,
			      const char     *pMapFileName  = 0,
			      FILE           *fp = 0 );
#else // BUSIF_TST
  OSCI2DCDR_WITH_ERRSIG_bool( sc_module_name  moduleName,
                              const char     *pMapFileName  = 0 );
#endif// BUSIF_TST

  /// <summary>destructor</summary>
  ~OSCI2DCDR_WITH_ERRSIG_bool( void );
  
  /// <summary>initialize resperr port</summary>
  void resperr_initialize();

  /// <summary>set resperr port</summary>
  /// <param name="trans">transaction</param>
  virtual void resperr_set(TlmTransactionType& trans);

  /// <summary>clear resperr port</summary>
  virtual void resperr_clear();
};


template<unsigned int BUSWIDTH>
OSCI2DCDR_WITH_ERRSIG_bool<BUSWIDTH>::
#if defined(BUSIF_TST)
OSCI2DCDR_WITH_ERRSIG_bool( sc_module_name  moduleName,
				  const char     *pMapFileName,
				  FILE           *fp ) :
  OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, bool>( moduleName, pMapFileName, fp )
#else // BUSIF_TST
OSCI2DCDR_WITH_ERRSIG_bool( sc_module_name  moduleName,
				  const char     *pMapFileName ) :
  OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, bool>( moduleName, pMapFileName )
#endif// BUSIF_TST
{
}


template<unsigned int BUSWIDTH>
OSCI2DCDR_WITH_ERRSIG_bool<BUSWIDTH>::~OSCI2DCDR_WITH_ERRSIG_bool( void )
{
}


template<unsigned int BUSWIDTH>
void
OSCI2DCDR_WITH_ERRSIG_bool<BUSWIDTH>::resperr_initialize()
{
  OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, bool>::resperr.initialize(false);
}


template<unsigned int BUSWIDTH>
void
OSCI2DCDR_WITH_ERRSIG_bool<BUSWIDTH>::resperr_set(TlmTransactionType& trans)
{
#ifdef TEST_CORE_DBG
  sc_dt::uint64  dbg_addr = trans.get_address();
  unsigned char  peid     = 0;
  TlmG3mExtension *g3mExt = (TlmG3mExtension *)0;
  trans.get_extension( g3mExt );
  if(g3mExt) peid = g3mExt->getPeId();
  printf("%s %s::resperr_set::peid=%d, address=0x%08x, error=%d\n", 
	 sc_time_stamp().to_string().c_str(), this->name(), peid, (unsigned int)dbg_addr, 1);
  fflush(stdout);
#endif
  OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, bool>::resperr.write(true);
}


template<unsigned int BUSWIDTH>
void
OSCI2DCDR_WITH_ERRSIG_bool<BUSWIDTH>::resperr_clear()
{
#ifdef TEST_CORE_DBG
  printf("%s %s::resperr_clear::error=%d\n", 
	 sc_time_stamp().to_string().c_str(), this->name(), 0);
  fflush(stdout);
#endif
  OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, bool>::resperr.write(false);
}

#endif // !__OSCI2DCDR_WITH_ERRSIG_bool_H__
