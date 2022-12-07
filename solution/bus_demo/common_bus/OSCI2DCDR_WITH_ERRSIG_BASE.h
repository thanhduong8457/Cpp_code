/*************************************************************************
 *
 *  OSCI2DCDR_WITH_ERRSIG_BASE.h
 *
 *  Copyright(c) 2014-2016 Renesas Electronics Corporation
 *  RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 *  This program must be used solely for the purpose for which
 *  it was furnished by Renesas Electronics Corporation. No part of this
 *  program may be reproduced or disclosed to others, in any
 *  form, without the prior written permission of Renesas Electronics
 *  Corporation.
 *
 *************************************************************************/

#ifndef __OSCI2DCDR_WITH_ERRSIG_BASE_H__
#define __OSCI2DCDR_WITH_ERRSIG_BASE_H__

#include "OSCI2DCDR.h"
#include <tlm_utils/peq_with_get.h>

extern sc_time_unit glb_resolution_unit;

template<unsigned int BUSWIDTH, class TYPE>
class OSCI2DCDR_WITH_ERRSIG_BASE :
public OSCI2DCDR<BUSWIDTH>
{
 public:
  sc_in<bool>   resetPort; // reset port
  sc_in<uint64> freqPort;  // freqency port for resperr output duration
  sc_out<TYPE>  resperr;   // error signal

  SC_HAS_PROCESS(OSCI2DCDR_WITH_ERRSIG_BASE);
  /// <summary>constructer</summary>
  /// <param name="moduleName">name of sc_module_name</param>
  /// <param name="pMapFileName">map file name</param>
  OSCI2DCDR_WITH_ERRSIG_BASE( sc_module_name  moduleName,
#if defined(BUSIF_TST)
			 const char     *pMapFileName  = 0,
			 FILE           *fp = 0 );
#else // BUSIF_TST
			 const char     *pMapFileName  = 0 );
#endif// BUSIF_TST

  /// <summary>destructor</summary>
  ~OSCI2DCDR_WITH_ERRSIG_BASE( void );

  /// <summary>transport API for forward pass of AT</summary>
  /// <param name="trans">transaction</param>
  /// <param name="phase">TLM phase</param>
  /// <param name="t">time</param>
  /// <returns>synchronization information</returns>
  TlmSyncEnum nb_transport_fw( TlmTransactionType& trans,
			       TlmPhase& phase, sc_core::sc_time& t);

  /// <summary>transport API for LT</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  void b_transport( TlmTransactionType& trans, sc_core::sc_time& t );
  
  /// <summary>response process</summary>
  void respProcess( void );

  /// <summary>error deassert process</summary>
  void errDeassertProcess( void );

  /// <summary>process to calculate clock period</summary>
  void calOnePulseProcess( void );

  /// <summary>reset process</summary>
  void resetProcess( void );

  /// <summary>initialize resperr port</summary>
  virtual void resperr_initialize();

  virtual void resperr_set(TlmTransactionType& trans) = 0;
  virtual void resperr_clear() = 0;

 private:
  tlm_utils::peq_with_get< TlmTransactionType > mRespEventPEQ; //error queue
  sc_event meErrDeassertEvent; //error deassert event
  sc_time  mOnePulse;          //one pulse width for error signal
};


template<unsigned int BUSWIDTH, class TYPE>
OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, TYPE>::
#if defined(BUSIF_TST)
OSCI2DCDR_WITH_ERRSIG_BASE( sc_module_name  moduleName,
		       const char     *pMapFileName,
		       FILE           *fp ) :
  OSCI2DCDR<BUSWIDTH>( moduleName, pMapFileName, fp ),
#else // BUSIF_TST
OSCI2DCDR_WITH_ERRSIG_BASE( sc_module_name  moduleName,
		       const char     *pMapFileName ) :
  OSCI2DCDR<BUSWIDTH>( moduleName, pMapFileName ),
#endif// BUSIF_TST
  resetPort( "resetPort" ),
  freqPort( "freqPort" ),
  resperr( "resperr" ),
  mRespEventPEQ( "mRespEventPEQ" ),
  mOnePulse( SC_ZERO_TIME )
{
  SC_METHOD( respProcess );
  OSCI2DCDR<BUSWIDTH>::sensitive << mRespEventPEQ.get_event();
  OSCI2DCDR<BUSWIDTH>::dont_initialize();

  SC_METHOD( errDeassertProcess );
  OSCI2DCDR<BUSWIDTH>::sensitive << meErrDeassertEvent;
  OSCI2DCDR<BUSWIDTH>::dont_initialize();

  SC_METHOD( calOnePulseProcess );
  OSCI2DCDR<BUSWIDTH>::sensitive << freqPort;
  OSCI2DCDR<BUSWIDTH>::dont_initialize();

  SC_METHOD( resetProcess );
  OSCI2DCDR<BUSWIDTH>::sensitive << resetPort.neg();
  OSCI2DCDR<BUSWIDTH>::dont_initialize();

  resperr_initialize();

}


template<unsigned int BUSWIDTH, class TYPE>
OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, TYPE>::~OSCI2DCDR_WITH_ERRSIG_BASE( void )
{
}


template<unsigned int BUSWIDTH, class TYPE>
TlmSyncEnum 
OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, TYPE>::
nb_transport_fw(TlmTransactionType& trans, TlmPhase& phase, sc_core::sc_time& t)
{
  outputLog( &trans, this, TRANS_AT_REQ, &t );
#if defined(BUSIF_TST)
  if(OSCI2DCDR<BUSWIDTH>::tstFp){
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s:",
	    sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"DECODER:nb_transport_fw() invoked\n");
    if( trans.get_command() == tlm::TLM_READ_COMMAND )
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        TLM_READ_COMMAND\n");
    else
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        TLM_WRITE_COMMAND\n");
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        address     =0x%x\n",
	    trans.get_address());
    int dataLength = trans.get_data_length();
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        dataLength  =%d\n",
	    dataLength);
    unsigned char *data = trans.get_data_ptr();
    for(int i=0; i<dataLength; i++)
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        data[%d]     =0x%x\n",
	      i,data[i]);
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        streamWidth=%d\n",
	    trans.get_streaming_width());
    if( trans.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE )
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        TLM_INCOMPLETE_RESPONSE\n");
    else
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        responseStatus error\n");
    TlmG3mExtension *pG3mExtension = NULL;
    trans.get_extension( pG3mExtension );
    if( pG3mExtension ){
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        IfPointer   =0x%x\n",
	      pG3mExtension->getIfPointer());
    }
    fflush(stdout);
  }
#endif // BUSIF_TST

  if( !(OSCI2DCDR<BUSWIDTH>::decode(trans)) ){
    phase = tlm::END_REQ;
    mRespEventPEQ.notify( trans );
    return( tlm::TLM_ACCEPTED );
  }
  if( (OSCI2DCDR<BUSWIDTH>::mpInterface) ){
    return(OSCI2DCDR<BUSWIDTH>::mpInterface->nb_transport_fw(trans,phase,t));
  }
  else{
    OSCI2DCDR<BUSWIDTH>::stub_access( trans );
    phase = tlm::END_REQ;
    OSCI2DCDR<BUSWIDTH>::mStubRespEventPEQ.notify( trans );
    return(tlm::TLM_UPDATED);
  }
}


template<unsigned int BUSWIDTH, class TYPE>
void
OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, TYPE>::
b_transport(TlmTransactionType& trans, sc_core::sc_time& t)
{
  outputLog( &trans, this, TRANS_LT_REQ, &t );
#if defined(BUSIF_TST)
  if(OSCI2DCDR<BUSWIDTH>::tstFp){
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s:",
	    sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"DECODER:b_transport() invoked\n");
    if( trans.get_command() == tlm::TLM_READ_COMMAND )
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        TLM_READ_COMMAND\n");
    else
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        TLM_WRITE_COMMAND\n");
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        address     =0x%x\n",
	    trans.get_address());
    int dataLength = trans.get_data_length();
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        dataLength  =%d\n",
	    dataLength);
    unsigned char *data = trans.get_data_ptr();
    for(int i=0; i<dataLength; i++)
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        data[%d]     =0x%x\n",
	      i,data[i]);
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        streamWidth=%d\n",
	    trans.get_streaming_width());
    if( trans.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE )
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        TLM_INCOMPLETE_RESPONSE\n");
    else
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        responseStatus error\n");
    TlmG3mExtension *pG3mExtension = NULL;
    trans.get_extension( pG3mExtension );
    if( pG3mExtension ){
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        IfPointer   =0x%x\n",
	      pG3mExtension->getIfPointer());
    }
    fflush(stdout);
  }
#endif // BUSIF_TST

  if( !(OSCI2DCDR<BUSWIDTH>::decode(trans)) ){
#ifdef BUSIF_TST
  if(OSCI2DCDR<BUSWIDTH>::tstFp){
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s:",
	    sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,
	    "DECODER::Assert resperr.\n");
  }
#endif //BUSIF_TST
    resperr_set( trans );
    meErrDeassertEvent.notify( mOnePulse );
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    trans.set_dmi_allowed(false);
    return;
  }

  if( (OSCI2DCDR<BUSWIDTH>::mpInterface) ){
    OSCI2DCDR<BUSWIDTH>::mpInterface->b_transport(trans, t);
  }
  else{
    OSCI2DCDR<BUSWIDTH>::stub_access( trans );
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    trans.set_dmi_allowed(false);
  }

  outputLog( &trans, this, TRANS_LT_RES, &t );

}


template<unsigned int BUSWIDTH, class TYPE>
void
OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, TYPE>::
respProcess( void )
{
#ifdef BUSIF_TST
  if(OSCI2DCDR<BUSWIDTH>::tstFp){
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s:",
	    sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,
	    "DECODER::Assert resperr.\n");
  }
#endif //BUSIF_TST
  
  TlmTransactionType *trans;
  TlmPhase            phase;
  sc_time             t;
  unsigned int        trans_num = 0;
  while ((trans = mRespEventPEQ.get_next_transaction()) != 0) {
    phase = tlm::BEGIN_RESP;
    t     = SC_ZERO_TIME;
    trans->set_response_status(tlm::TLM_OK_RESPONSE);
    if (trans_num < 1){
      resperr_set( *trans );
    }
    trans_num++;
#ifdef BUSIF_TST
    if(OSCI2DCDR<BUSWIDTH>::tstFp){
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s:",
	      sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,
	      "DECODER::Call nb_transport_bw() from %s\n",
	      OSCI2DCDR<BUSWIDTH>::ts.name());
      if( trans->get_command() == tlm::TLM_READ_COMMAND )
	fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        TLM_READ_COMMAND\n");
      else
	fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        TLM_WRITE_COMMAND\n");
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        address     =0x%x\n",
	    trans->get_address());
      int dataLength = trans->get_data_length();
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        dataLength  =%d\n",
	      dataLength);
      unsigned char *data = trans->get_data_ptr();
      for(int i=0; i<dataLength; i++)
	fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        data[%d]     =0x%x\n",
		i,data[i]);
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        streamWidth=%d\n",
	      trans->get_streaming_width());
      if( phase == tlm::BEGIN_RESP ){
	fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,
		"             phase =tlm::BEGIN_RESP\n");
      }
      else{
	fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,
		"             phase =other\n");
      }
      fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"        t =%s\n",
	      t.to_string().c_str());
    }
#endif //BUSIF_TST
    OSCI2DCDR<BUSWIDTH>::ts->nb_transport_bw( *trans, phase, t );
  }
  meErrDeassertEvent.notify( mOnePulse );
}


template<unsigned int BUSWIDTH, class TYPE>
void
OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, TYPE>::
errDeassertProcess( void )
{
#ifdef BUSIF_TST
  if(OSCI2DCDR<BUSWIDTH>::tstFp){
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s:",
	    sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
    fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,
	    "BusSlaveIf::Deassert resperr.\n");
  }
#endif //BUSIF_TST
  resperr_clear( );
}


template<unsigned int BUSWIDTH, class TYPE>
void
OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, TYPE>::
calOnePulseProcess( void )
{
  uint64 onePulse = 0xFFFFFFFFFFFFFFFFULL;

  if( freqPort.read() ){
    switch(glb_resolution_unit){
    case SC_SEC:
      onePulse = (uint64)(1.0)/freqPort.read();
      break;
    case SC_MS:
      onePulse = (uint64)(1.0e+3)/freqPort.read();
      break;
    case SC_US:
      onePulse = (uint64)(1.0e+6)/freqPort.read();
      break;
    case SC_NS:
      onePulse = (uint64)(1.0e+9)/freqPort.read();
      break;
    case SC_PS:
      onePulse = (uint64)(1.0e+12)/freqPort.read();
      break;
    case SC_FS:
      onePulse = (uint64)(1.0e+15)/freqPort.read();
      break;
    }
  }
  mOnePulse = sc_time((double)onePulse,glb_resolution_unit);
}


template<unsigned int BUSWIDTH, class TYPE>
void
OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, TYPE>::
resetProcess( void )
{
  resperr_clear( );
  mRespEventPEQ.cancel_all();
  meErrDeassertEvent.cancel();
#ifdef BUSIF_TST
  fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s:",
	  sc_time_stamp().to_string().c_str());
#ifdef PLTFRM_TST
  fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
  fprintf(OSCI2DCDR<BUSWIDTH>::tstFp,
	  "DECODER::mRespEventPEQ and meErrDeassertEvent were canceled.\n");
#endif //BUSIF_TST
}


template<unsigned int BUSWIDTH, class TYPE>
void
OSCI2DCDR_WITH_ERRSIG_BASE<BUSWIDTH, TYPE>::
resperr_initialize()
{
}


#endif // !__OSCI2DCDR_WITH_ERRSIG_BASE_H__
