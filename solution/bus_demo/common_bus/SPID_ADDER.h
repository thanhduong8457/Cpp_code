/**********************************************************************
 *
 * SPID_ADDER.h
 *
 * Copyright(c) 2013 Renesas Electronics Corporation
 * RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 * This program must be used solely for the purpose for which
 * it was furnished by Renesas Electronics Corporation. No part of this
 * program may be reproduced or disclosed to others, in any
 * form, without the prior written permission of Renesas Electronics
 * Corporation.
 *
 **********************************************************************/

#ifndef __SPID_ADDER_H__
#define __SPID_ADDER_H__


#include "OSCI2.h"     // class for TLM communication based on TLM2.0
#include "CmErrMsg.h"  // for error message

#define EXTERNAL_MASTER_PEID 4
#define EXTERNAL_MASTER_VM   true
#define DEFAULT_SPID         2


template<unsigned int BUSWIDTH>
class SPID_ADDER :
  public sc_module,
  public virtual TlmFwTransportIf,
  public virtual TlmBwTransportIf
{
 public:
  /// <summary>constructor</summary>
  SPID_ADDER( sc_module_name  name ):
    sc_module( name ),
    spid_bit0( 0 ),
    spid_bit1( 0 ),
    spid_bit2( 0 ),
    spid_bit3( 0 ),
    spid_bit4( 0 ),
    ts("ts"),
    is("is")
#ifdef BUSIF_TST
    ,tstFp( (FILE *)0 )
#endif //BUSIF_TST
  {
    ts(*this);
    is(*this);
    spid_bit0 = new sc_in<bool>( "spid_bit0" );
    spid_bit1 = new sc_in<bool>( "spid_bit1" );
    spid_bit2 = new sc_in<bool>( "spid_bit2" );
    spid_bit3 = new sc_in<bool>( "spid_bit3" );
    spid_bit4 = new sc_in<bool>( "spid_bit4" );
  }

  /// <summary>destructor</summary>
  ~SPID_ADDER(void)
  {
    delete spid_bit0;
    delete spid_bit1;
    delete spid_bit2;
    delete spid_bit3;
    delete spid_bit4;
  }

  /// <summary>forward pass of non blocking transport</summary>
  /// <param name="trans">transaction</param>
  /// <param name="phase">TLM phase</param>
  /// <param name="t">time</param>
  /// <returns>synchronization word</returns>
  TlmSyncEnum nb_transport_fw( TlmTransactionType& trans, 
			       TlmPhase&           phase, 
			       sc_time&            t )
  {
    setSpId( trans );
    return is->nb_transport_fw( trans, phase, t );
  }

  /// <summary>blocking transport</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  void b_transport( TlmTransactionType& trans, sc_time& t )
  {
    setSpId( trans );
    is->b_transport( trans, t );
    unsetSpId( trans );
  }

  /// <summary>debug transport</summary>
  /// <param name="r">transaction</param>
  /// <returns>treated data byte number</returns>
  unsigned int transport_dbg( TlmTransactionType& r )
  {
    setSpId( r );
    unsigned int nmb = is->transport_dbg( r );
    unsetSpId( r );
    return nmb;
  }
  
  /// <summary>get direct access memory pointer</summary>
  /// <param name="trans">transaction</param>
  /// <param name="dmiData">data</param>
  /// <returns>DMI is allowed or not</returns>
  bool get_direct_mem_ptr( TlmTransactionType& trans,
			   tlm::tlm_dmi&  dmiData )
  {
    return is->get_direct_mem_ptr( trans, dmiData );
  }

  /// <summary>backward pass of non blocking transport</summary>
  /// <param name="trans">transaction</param>
  /// <param name="phase">TLM phase</param>
  /// <param name="t">time</param>
  /// <returns>synchronization word</returns>
  TlmSyncEnum nb_transport_bw( TlmTransactionType& trans, 
			       TlmPhase&           phase, 
			       sc_core::sc_time&   t )
  {
    unsetSpId( trans );
    TlmSyncEnum syncEnum =  ts->nb_transport_bw( trans, phase, t );
    return syncEnum;
  }

  /// <summary>DMI is not supported</summary>
  /// <param name="startRange">start address range</param>
  /// <param name="endRange">end address range</param>
  void invalidate_direct_mem_ptr( uint64 startRange,
				  uint64 endRange ) 
  {
  }

#ifdef BUSIF_TST
  /// <summary>end of elaboration</summary>
  void end_of_elaboration( void )
  {
#ifdef PLTFRM_TST
    tstFp = stdout;
#else  //PLTFRM_TST
    char debug_file_name[1024];
    strcpy(debug_file_name, this->name());
    strcat(debug_file_name, ".log");
    if( (tstFp=fopen(debug_file_name,"w")) == NULL ){
      printf("\n%s:UT test file open error!!\n\n", this->name());
      fflush(stdout);
    }
#endif //PLTFRM_TST
  }
#endif //BUSIF_TST
    


 public:
  sc_in<bool>                 *spid_bit0; //spid bit0 port
  sc_in<bool>                 *spid_bit1; //spid bit1 port
  sc_in<bool>                 *spid_bit2; //spid bit2 port
  sc_in<bool>                 *spid_bit3; //spid bit3 port
  sc_in<bool>                 *spid_bit4; //spid bit4 port
  TlmTargetSocket<BUSWIDTH>    ts;        //target socket
  TlmInitiatorSocket<BUSWIDTH> is;        //initiator socket
#ifdef BUSIF_TST
 private:
  FILE                        *tstFp;
#endif //BUSIF_TST


 private:
  /// <summary>set SPID</summary>
  /// <param name="r">reference of object of generic payload</param>
  void setSpId( TlmTransactionType &trans )
  {
    TlmG3mExtension *g3mExt = new TlmG3mExtension;
    trans.set_extension( g3mExt );

    g3mExt->setIfPointer( this );

    g3mExt->setPeId( EXTERNAL_MASTER_PEID );

    g3mExt->setVirtualMode( EXTERNAL_MASTER_VM );

    unsigned char spId = 
      spid_bit0->read()        +
      (spid_bit1->read() << 1) +
      (spid_bit2->read() << 2) +
      (spid_bit3->read() << 3) +
      (spid_bit4->read() << 4) ;
    if( spId < DEFAULT_SPID ){
      g3mExt->setSpId( DEFAULT_SPID );
    }
    else{
      g3mExt->setSpId( spId );
    }
#if defined(BUSIF_TST)
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
      fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"SPID_ADDER::set SPID\n");
      fprintf(tstFp,"        SPID pin value = %d\n",spId);
    }
#endif //BUSIF_TST
    g3mExt = 0;
    trans.get_extension( g3mExt );
  }

  /// <summary>unset SPID</summary>
  /// <param name="r">reference of object of generic payload</param>
  void unsetSpId( TlmTransactionType &trans )
  {
    TlmG3mExtension *g3mExt = (TlmG3mExtension *)0;
    trans.get_extension( g3mExt );
    trans.release_extension( g3mExt );

#if defined(BUSIF_TST)
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
      fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"SPID_ADDER::unset SPID\n");
    }
#endif //BUSIF_TST

  }

};

#endif //__SPID_ADDER_H_
