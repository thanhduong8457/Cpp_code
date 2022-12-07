/*************************************************************************
 *
 *  BACKWARD_DECODER.h
 *
 *  Copyright(c) 2011-2017 Renesas Electronics Corporation
 *  RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 *  This program must be used solely for the purpose for which
 *  it was furnished by Renesas Electronics Corporation. No part of this
 *  program may be reproduced or disclosed to others, in any
 *  form, without the prior written permission of Renesas Electronics
 *  Corporation.
 *
 *************************************************************************/

#ifndef BACKWARD_DECODER_H
#define BACKWARD_DECODER_H

#include "CmErrMsg.h"
#include "OSCI2.h"

template< unsigned int BUSWIDTH >
class BACKWARD_DECODER : 
public sc_module,
public virtual TlmFwTransportIf,
public virtual TlmBwTransportIf
{

 public:
  TlmInitiatorSocket<BUSWIDTH> is;  //initiator socket
  TlmTargetSocket<BUSWIDTH>    ts;  //target socket

  /// <summary>constructor</summary>
  /// <param name="moduleName">name of sc_module</param>
  BACKWARD_DECODER( sc_module_name moduleName );

  /// <summary>destructor</summary>
  ~BACKWARD_DECODER( void );

  /// <summary>transport API for forward pass of AT</summary>
  /// <param name="trans">transaction</param>
  /// <param name="phase">TLM phase</param>
  /// <param name="t">time</param>
  /// <returns>synchronization information</returns>
  TlmSyncEnum nb_transport_fw( TlmTransactionType& trans,
			       TlmPhase& phase,
			       sc_time& t );

  /// <summary>transport API for backward pass of AT</summary>
  /// <param name="trans">transaction</param>
  /// <param name="phase">TLM phase</param>
  /// <param name="t">time</param>
  /// <returns>synchronization information</returns>
  TlmSyncEnum nb_transport_bw( TlmTransactionType& trans,
			       TlmPhase& phase,
			       sc_time& t );

  /// <summary>transport API for debug access</summary>
  /// <param name="r">transaction</param>
  /// <returns>data number</returns>
  unsigned int transport_dbg( TlmTransactionType& r );

  /// <summary>Dummy function (DMI access is not supported)</summary>
  /// <param name="startRange">start range for DMI access</param>
  /// <param name="endRange">end rang for DMI access</param>
  void invalidate_direct_mem_ptr( sc_dt::uint64 startRange,
				  sc_dt::uint64 endRange );

  /// <summary>Dummy function (DMI access is not supported)</summary>
  /// <param name="trans">transaction</param>
  /// <param name="dmiData">permissible access(read or(and) write)</param>
  /// <returns>accept or reject</returns>
  bool get_direct_mem_ptr( TlmTransactionType& trans, tlm::tlm_dmi&  dmiData );

  /// <summary>transport API for LT</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  /// <returns>data number</returns>
  void b_transport(TlmTransactionType& trans, sc_time& t);

};

template< unsigned int BUSWIDTH >
BACKWARD_DECODER<BUSWIDTH>::
BACKWARD_DECODER( sc_module_name moduleName ) :
  sc_module( moduleName ),
  is( "is" ),
  ts( "ts" )
{
  is( *this );
  ts( *this );
}

template< unsigned int BUSWIDTH >
BACKWARD_DECODER<BUSWIDTH>::
~BACKWARD_DECODER( void )
{}

template< unsigned int BUSWIDTH >
TlmSyncEnum 
BACKWARD_DECODER<BUSWIDTH>::
nb_transport_fw( TlmTransactionType& trans,
		 TlmPhase& phase,
		 sc_time& t )
{
  return( is->nb_transport_fw( trans, phase, t ) );
}

template< unsigned int BUSWIDTH >
TlmSyncEnum 
BACKWARD_DECODER<BUSWIDTH>::
nb_transport_bw( TlmTransactionType& trans,
		 TlmPhase& phase,
		 sc_time& t )
{
  TlmG3mExtension *pG3mExtension = (TlmG3mExtension *)0;
  trans.get_extension( pG3mExtension );

  if( !pG3mExtension ){
    // error (not support TlmG3mExtension)
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf,
	     "Received transaction doesn't support the TlmG3mExtension." );
    CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    return tlm::TLM_ACCEPTED;
  }

  return( pG3mExtension->getIfPointer()->nb_transport_bw(trans, phase, t) );
}

template< unsigned int BUSWIDTH >
unsigned int
BACKWARD_DECODER<BUSWIDTH>::
transport_dbg( TlmTransactionType& r )
{
  return( is->transport_dbg( r ) );
}

template< unsigned int BUSWIDTH >
void 
BACKWARD_DECODER<BUSWIDTH>::
invalidate_direct_mem_ptr( sc_dt::uint64 startRange,
			   sc_dt::uint64 endRange )
{
}

template< unsigned int BUSWIDTH >
bool
BACKWARD_DECODER<BUSWIDTH>::
get_direct_mem_ptr( TlmTransactionType& trans, tlm::tlm_dmi&  dmiData )
{
  return false;
}

template< unsigned int BUSWIDTH >
void
BACKWARD_DECODER<BUSWIDTH>::
b_transport(TlmTransactionType& trans, sc_time& t)
{
  return( is->b_transport( trans, t ) );
}

#endif
