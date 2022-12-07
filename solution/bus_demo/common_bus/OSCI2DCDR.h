/*************************************************************************
 *
 *  OSCI2DCDR.h
 *
 *  Copyright(c) 2014-2020 Renesas Electronics Corporation
 *  RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 *  This program must be used solely for the purpose for which
 *  it was furnished by Renesas Electronics Corporation. No part of this
 *  program may be reproduced or disclosed to others, in any
 *  form, without the prior written permission of Renesas Electronics
 *  Corporation.
 *
 *************************************************************************/

#ifndef __OSCI2DCDR_H__
#define __OSCI2DCDR_H__

#include "OSCI2.h"
#include "CmErrMsg.h"
#include "BusMonitorBase.h"
#include <set>
#include <tlm_utils/peq_with_get.h>

template<unsigned int BUSWIDTH>
class OSCI2DCDR :
public sc_module,
public virtual TlmFwTransportIf,
public virtual TlmBwTransportIf,
public BusMonitorBase
{
 private:
  const static ADDRESS_TYPE ADR_MSK = 0xFFFFFFFF;

  typedef tlm::tlm_initiator_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0> 
    I_SOCKET_TYPE;

  /// <summary>address map class</summary>
  class addressMap_t {
  public:
    /// <summary>constructor</summary>
    addressMap_t( TlmFwTransportIf *pInterface,
		  const char       *pSocketName,
		  ADDRESS_TYPE     startAddress,
		  ADDRESS_TYPE     endAddress,
		  unsigned char    *pStub = 0 );
    /// <summary>destructor</summary>
    ~addressMap_t( void );
    /// <summary>get interface</summary>
    /// <returns>interface pointer</returns>
    TlmFwTransportIf *getInterface( void );
    /// <summary>get socket name</summary>
    /// <returns>socket name</returns>
    const char *getSocketName( void );
    /// <summary>get start address</summary>
    /// <returns>start address</returns>
    ADDRESS_TYPE getStartAddr( void );
    /// <summary>get end address</summary>
    /// <returns>end address</returns>
    ADDRESS_TYPE getEndAddr( void );
    /// <summary>decode</summary>
    /// <param name="address">address</param>
    /// <returns>success or failure</returns>
    bool decode( ADDRESS_TYPE address );
    /// <summary>get stub pointer</summary>
    /// <returns>stub pointer</returns>
    unsigned char* getStub( void );
  protected:
    TlmFwTransportIf *mpInterface; //interface pointer
    std::string      mSocketName;  //socket name
    ADDRESS_TYPE     mStartAddress;//start address
    ADDRESS_TYPE     mEndAddress;  //end address
    unsigned char    *mpStub;      //Stub pointer
  };
  
 public:
  TlmInitiatorSocket<BUSWIDTH> is;  //TLM2 initiator socket
  TlmTargetSocket<BUSWIDTH>    ts;  //TLM2 target socket
  
  SC_HAS_PROCESS(OSCI2DCDR);
#if defined(BUSIF_TST)
  /// <summary>constructer</summary>
  /// <param name="moduleName">name of sc_module_name</param>
  /// <param name="pMapFileName">map file name</param>
  /// <param name="fp">debug log file pointer</param>
  OSCI2DCDR( sc_module_name  moduleName,
	     const char     *pMapFileName  = 0,
	     FILE           *fp = 0 );
#else // BUSIF_TST
  /// <summary>constructer</summary>
  /// <param name="moduleName">name of sc_module_name</param>
  /// <param name="pMapFileName">map file name</param>
  OSCI2DCDR( sc_module_name  moduleName,
	     const char     *pMapFileName  = 0 );
#endif// BUSIF_TST
  /// <summary>destructor</summary>
  ~OSCI2DCDR( void );

  /// <summary>transport API for forward pass of AT</summary>
  /// <param name="trans">transaction</param>
  /// <param name="phase">TLM phase</param>
  /// <param name="t">time</param>
  /// <returns>synchronization information</returns>
  virtual TlmSyncEnum nb_transport_fw( TlmTransactionType& trans,
				       TlmPhase& phase, sc_core::sc_time& t);
  /// <summary>transport API for backward pass of AT</summary>
  /// <param name="trans">transaction</param>
  /// <param name="phase">TLM phase</param>
  /// <param name="t">time</param>
  /// <returns>synchronization information</returns>
  TlmSyncEnum nb_transport_bw( TlmTransactionType& trans,
			       TlmPhase& phase, sc_core::sc_time& t);
  /// <summary>transport API for debug access</summary>
  /// <param name="r">transaction</param>
  /// <returns>data number</returns>
  unsigned int transport_dbg( TlmTransactionType& r );
  /// <summary>dummy</summary>
  void invalidate_direct_mem_ptr( sc_dt::uint64 startRange,
				  sc_dt::uint64 endRange );
  /// <summary>dummy</summary>
  bool get_direct_mem_ptr( TlmTransactionType& trans,
			   tlm::tlm_dmi&       dmiData );
  /// <summary>transport API for LT</summary>
  /// <param name="trans">transaction</param>
  /// <param name="t">time</param>
  virtual void b_transport( TlmTransactionType& trans, sc_core::sc_time& t );
  
  /// <summary>decode</summary>
  /// <param name="transaction">transaction</param>
  /// <returns>success or failure</returns>
  virtual bool decode( TlmTransactionType& trans );

  /// <summary>end of elaboration</summary>
  void end_of_elaboration( void );

  /// <summary>access(read/write) to Stub</summary>
  /// <param name="transaction">transaction</param>
  void stubAccess( TlmTransactionType& trans );

  /// <summary>response process</summary>
  void stubRespProcess( void );

 private:
  /// <summary>insert address map</summary>
  /// <param name="map">address map structure</param>
  /// <param name="mapVect">address map structure vector</param>
  /// <returns>success or failure</returns>
  bool insertMap( addressMap_t &map, std::vector<addressMap_t*> &mapVect );

 protected:
  /// <summary>load map</summary>
  /// <returns>success or failure</returns>
  bool loadMap( void );
  /// <summary>register terminal socket</summary>
  /// <param name="pSocket">initiator socket pointer</param>
  void registTerminalSocket( TlmInitiatorSocket<BUSWIDTH> *pSocket );
  /// <summary>register terminal socket</summary>
  /// <param name="pSocket">target socket pointer</param>
  void registTerminalSocket( TlmTargetSocket<BUSWIDTH> *pSocket );
  
  char                             *mpMapFileName; //map file name
  TlmFwTransportIf                 *mpInterface; //interface pointer
  std::vector< addressMap_t* >      mpInterfaceMap; //interface map
  unsigned int                     mpInterfaceMapSize; //interface map size
  ADDRESS_TYPE                     mAddressMask; //address mask
  std::vector< TlmTargetSocket<BUSWIDTH> * >   mTerminalSocketList; // terminal socket list
  std::vector< I_SOCKET_TYPE * >    mPureTlmInitiatorSocketList; // pure TLM initiator socket list
  std::vector< int >                mPureTlmTargetSocketList; //pure TLM initiator socket list
  unsigned char                    *mpStub; //Stub pointer
  unsigned int                     mpOffsetAddress; // offset address for Stub

 public:
  tlm_utils::peq_with_get< TlmTransactionType > mStubRespEventPEQ;//error queue

 protected:
#if defined(BUSIF_TST)
  FILE *tstFp;
#endif // BUSIF_TST

};


template< unsigned int BUSWIDTH >
OSCI2DCDR<BUSWIDTH>::
addressMap_t::addressMap_t( TlmFwTransportIf *pInterface,
			    const char       *pSocketName,
			    ADDRESS_TYPE     startAddress,
			    ADDRESS_TYPE     endAddress,
			    unsigned char    *pStub ) :
  mpInterface( pInterface ),
  mSocketName( pSocketName ),
  mStartAddress( startAddress ),
  mEndAddress( endAddress ),
  mpStub( pStub )
{
}


template< unsigned int BUSWIDTH >
OSCI2DCDR<BUSWIDTH>::
addressMap_t::~addressMap_t( void )
{
  delete mpStub;
}


template< unsigned int BUSWIDTH >
TlmFwTransportIf * 
OSCI2DCDR<BUSWIDTH>::
addressMap_t::getInterface( void )
{
  return mpInterface;
}


template< unsigned int BUSWIDTH >
const char * 
OSCI2DCDR<BUSWIDTH>::
addressMap_t::getSocketName( void )
{
  return mSocketName.c_str();
}


template< unsigned int BUSWIDTH >
ADDRESS_TYPE 
OSCI2DCDR<BUSWIDTH>::
addressMap_t::getStartAddr( void )
{
  return mStartAddress;
}


template< unsigned int BUSWIDTH >
ADDRESS_TYPE 
OSCI2DCDR<BUSWIDTH>::
addressMap_t::getEndAddr( void )
{
  return mEndAddress;
}


template< unsigned int BUSWIDTH >
bool
OSCI2DCDR<BUSWIDTH>::
addressMap_t::decode( ADDRESS_TYPE address )
{
  return (( address >= mStartAddress ) && ( address <= mEndAddress ));
}


template< unsigned int BUSWIDTH >
unsigned char *
OSCI2DCDR<BUSWIDTH>::
addressMap_t::getStub( void )
{
  return ( mpStub );
}


template< unsigned int BUSWIDTH >
OSCI2DCDR<BUSWIDTH>::
#if defined(BUSIF_TST)
OSCI2DCDR( sc_module_name  moduleName,
	   const char     *pMapFileName,
	   FILE           *fp )
#else // BUSIF_TST
OSCI2DCDR( sc_module_name  moduleName,
	   const char     *pMapFileName )
#endif// BUSIF_TST
  : sc_module( moduleName ),
    is( "is" ),
    ts( "ts" ),
    mpMapFileName( ( char *)pMapFileName ),
    mpInterface( (TlmFwTransportIf *)0 ),
    mpInterfaceMapSize( 0 ),
    mAddressMask( ADR_MSK ),
    mpStub( 0 ),
    mpOffsetAddress( 0 ),
    mStubRespEventPEQ( "mStubRespEventPEQ" )
#if defined(BUSIF_TST)
    ,tstFp( fp )
#endif// BUSIF_TST
{
  is( *this );
  ts( *this );

  SC_METHOD( stubRespProcess );
  OSCI2DCDR<BUSWIDTH>::sensitive << mStubRespEventPEQ.get_event();
  OSCI2DCDR<BUSWIDTH>::dont_initialize();
}


template< unsigned int BUSWIDTH >
OSCI2DCDR<BUSWIDTH>::~OSCI2DCDR( void )
{
  for( int i = mpInterfaceMap.size(); i > 0 ; i-- )
    delete mpInterfaceMap[i-1];
  std::vector< addressMap_t * >().swap(mpInterfaceMap);
  std::vector< TlmTargetSocket<BUSWIDTH> * >().swap(mTerminalSocketList);
  std::vector< I_SOCKET_TYPE * >().swap(mPureTlmInitiatorSocketList);
  std::vector< int >().swap(mPureTlmTargetSocketList);
}


template< unsigned int BUSWIDTH >
void 
OSCI2DCDR<BUSWIDTH>::end_of_elaboration( void )
{
  std::vector< TlmTargetSocket<BUSWIDTH> * >().swap(mTerminalSocketList);
  std::vector< I_SOCKET_TYPE * >().swap(mPureTlmInitiatorSocketList);
  std::vector< int >().swap(mPureTlmTargetSocketList);

  loadMap();

#if defined(BUSIF_TST)
#if defined(PLTFRM_TST)
  tstFp = stdout;
#endif //PLTFRM_TST
#endif //BUSIF_TST

}

template< unsigned int BUSWIDTH >
void 
OSCI2DCDR<BUSWIDTH>::registTerminalSocket( TlmInitiatorSocket<BUSWIDTH> *pSocket )
{

  int size = pSocket->getTSocketListSize();

  if( !size ){
    //error "initiator socket is not bound."
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf,
	     "Initiator socket was not bound to any target." );
    CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    return;
  }

  for( int i=0; i<size; i++ ){
    TlmTargetSocket<BUSWIDTH> *targetSocket = pSocket->getTSocket(i);
    if( targetSocket == (TlmTargetSocket<BUSWIDTH> *)0) {
      TlmInitiatorSocket<BUSWIDTH> *boundInitiatorSocket 
	= pSocket->getISocket(i);
      if( boundInitiatorSocket == (TlmInitiatorSocket<BUSWIDTH> *)0 ){
	//error
	char errorMessageBuf[1024];
	sprintf( errorMessageBuf,
		 "Internal bind error." );
	CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
      }
      else if( pSocket->isPureTlmSocket(i) ){//for pure TLM connection
        mPureTlmInitiatorSocketList.push_back(boundInitiatorSocket);
      }
      else {
	registTerminalSocket(boundInitiatorSocket);
      }
    }
    else if( pSocket->isPureTlmSocket(i) ){//for pure TLM connection
      mTerminalSocketList.push_back(targetSocket);
      mPureTlmTargetSocketList.push_back((int)1);
    }
    else {
      registTerminalSocket(targetSocket);
    }
  }
}

template< unsigned int BUSWIDTH >
void 
OSCI2DCDR<BUSWIDTH>::registTerminalSocket( TlmTargetSocket<BUSWIDTH> *pSocket )
{

  int size = pSocket->getTSocketListSize();

  if( !size ){
    mTerminalSocketList.push_back(pSocket);
    mPureTlmTargetSocketList.push_back((int)0);//for pure TLM connection
    return;
  }

  for( int i=0; i<size; i++ ){
    TlmTargetSocket<BUSWIDTH> *targetSocket = pSocket->getTSocket(i);
    if( targetSocket == (TlmTargetSocket<BUSWIDTH> *)0) {
	//error "target socket is bound to initiator socket."
	char errorMessageBuf[1024];
	sprintf( errorMessageBuf,
		 "Internal bind error." );
	CmErrMsg::printErrorMessage( this->name(), 0, errorMessageBuf );
    }
    else if( pSocket->isPureTlmSocket(i) ){//for pure TLM connection
      mTerminalSocketList.push_back(targetSocket);
      mPureTlmTargetSocketList.push_back((int)1);
    }
    else {
      registTerminalSocket(targetSocket);
    }
  }
}


template< unsigned int BUSWIDTH >
bool
OSCI2DCDR<BUSWIDTH>::loadMap( void )
{

  registTerminalSocket(&is);
  
  mpInterfaceMapSize = 0;
  
  FILE *pMapfileHandle = fopen( mpMapFileName, "r" );
  if( !pMapfileHandle ){
    //
    // map file open error
    //
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf, 
	     "Address map file(\"%s\") cannot be opened.",
	     mpMapFileName );
    CmErrMsg::printErrorMessage( this->name(), "loadMap", errorMessageBuf );
    return( false );
  }
  else {
    std::vector< addressMap_t * >().swap(mpInterfaceMap);
    //
    // read line in map file 
    //
    while( !feof( pMapfileHandle )){
      char pMapLine[ 1024 ];
      if( fgets( pMapLine, 1024, pMapfileHandle ) ){
	std::string mapLine = pMapLine;
	if ( ( mapLine[ 0 ] == ';' ) || 
	     ( mapLine[ 0 ] == ' ' ) ||
	     ( mapLine[ 0 ] == '\n' ) ||
	     ( mapLine[ 0 ] == '\t' ) ||
	     mapLine.length() == 0) { 
	  //
	  // invalid line
	  //
	}
	else if( mapLine[ 0 ] == '#' ){
	  std::string strTag, strMask;
	  char chTag[1024] = "";
	  char chMask[1024] = "";
	  sscanf( mapLine.c_str(), "%s\t%s", chTag, chMask );
	  strTag = chTag; strMask = chMask;
	  if( strMask.find( "0x" ) != std::string::npos || 
	      strMask.find( "0X" ) != std::string::npos )
	    mAddressMask = strtoull( ( char* )strMask.c_str(), ( char** )NULL,16 );
	  else
	    mAddressMask = strtoull( ( char* )strMask.c_str(), ( char** )NULL,10 );
	}
	else {
	  //
	  // Extract mapping info from mapfile line
	  //
	  unsigned long long scanfAddr, scanfSize;
	  char chName[1024] = "";
	  char chAddr[1024] = "";
	  char chSize[1024] = "";
	  char chStubBus[1024] = "";
	  std::string strName, strAddr, strSize, strStubBus;
	  sscanf(mapLine.c_str(),"%s\t%s\t%s\t%s",chName,chAddr,chSize,chStubBus);
	  strName=chName; strAddr=chAddr; strSize=chSize; strStubBus=chStubBus;
	  if( strAddr.find( "0x" ) != std::string::npos || 
	      strAddr.find( "0X" ) != std::string::npos )
	    scanfAddr = strtoull( ( char* )strAddr.c_str(), ( char** )NULL,16 );
	  else
	    scanfAddr = strtoull( ( char* )strAddr.c_str(), ( char** )NULL,10 );
	  if( strSize.length() == 0 ){
	    //
	    // Error : area size is not specified
	    //
	    char errorMessageBuf[1024];
	    sprintf( errorMessageBuf, 
		     "Area size is not specified in address map %s",
		     mpMapFileName );
	    CmErrMsg::printErrorMessage( this->name(), "loadMap", errorMessageBuf );
	    fclose( pMapfileHandle );
	    return false;
	  }
	  else if( strSize.find( "0x" ) != std::string::npos || 
		   strSize.find( "0X" ) != std::string::npos )
	    scanfSize = strtoul( ( char* )strSize.c_str(), ( char** )NULL,16 );
	  else
	    scanfSize = strtoul( ( char* )strSize.c_str(), ( char** )NULL,10 );
	  ADDRESS_TYPE addr = (ADDRESS_TYPE)scanfAddr;
	  ADDRESS_TYPE size = (ADDRESS_TYPE)scanfSize;
	  bool registered = false;
	  
	  if( !( size > 0 ) ){
	    //
	    // Error : area size is 0
	    //
	    char errorMessageBuf[1024];
	    sprintf( errorMessageBuf, 
		     "Area size is 0 in address map %s",
		     mpMapFileName );
	    CmErrMsg::printErrorMessage( this->name(), "loadMap", errorMessageBuf );
	    fclose( pMapfileHandle );
	    return false;
	  }	  
	  
	  //
	  // For all connected target Socket, compare Socket name with name from mapfile line 
	  //
	  int slaveNumber = mTerminalSocketList.size();
	  for( int i = 0; i < slaveNumber; i++ ){
	    std::string n( mTerminalSocketList[i]->name() );
	    //
	    // Name matches
	    //
	    if( n == strName ) {
	      //
	      // New map entry
	      //
	      addressMap_t * map = 
		new addressMap_t( is[i],        
				  mTerminalSocketList[i]->name(),
				  addr,
				  addr + size - 1 );
	      //
	      // Register map entry
	      //
	      if(!insertMap(*map,(std::vector<addressMap_t*>&)mpInterfaceMap)){
		fclose( pMapfileHandle );
		return( false );
	      }
	      //
	      // Set base address and size
	      //
	      if( !mPureTlmTargetSocketList[i] )
		mTerminalSocketList[i]->setBaseAddressSize( addr, size );
	      registered = true;
	      break;
	    }
	  }
	  if( !registered ){
	    slaveNumber = mPureTlmInitiatorSocketList.size();
	    for( int i = 0; i < slaveNumber; i++ ){
	      std::string n( mPureTlmInitiatorSocketList[i]->name() );
	      //
	      // Name matches
	      //
	      if( n == strName ) {
		//
		// New map entry
		//
		addressMap_t * map = 
		  new addressMap_t( is[i],
				    mPureTlmInitiatorSocketList[i]->name(),
				    addr,
				    addr + size - 1 );
		//
		// Register map entry
		//
		if(!insertMap(*map,(std::vector<addressMap_t*>&)mpInterfaceMap)){
		  fclose( pMapfileHandle );
		  return( false );
		}
		registered = true;
		break;
	      }
	    }
	  }
	  // automatic stub creation
	  if( !registered ){
	    if( strStubBus == this->name() ){
	      unsigned char *mem = new unsigned char [size];
	      for( unsigned int i=0; i<size; i++ )
		mem[i] = 0;
	      addressMap_t * map = 
		new addressMap_t( 0,
				  chName,
				  addr,
				  addr + size - 1,
				  mem );
	      if(!insertMap(*map,(std::vector<addressMap_t*>&)mpInterfaceMap)){
		fclose( pMapfileHandle );
		return( false );
	      }
	    }
	  }

	  mpInterfaceMapSize = mpInterfaceMap.size();

	}
      }
    }
  } 
  fclose( pMapfileHandle );
  return( true );
}


template< unsigned int BUSWIDTH >
bool
OSCI2DCDR<BUSWIDTH>::
insertMap( addressMap_t &map, std::vector<addressMap_t*> &mapVect )
{
  int mapIndex, mapSize = mapVect.size();
  typename std::vector<addressMap_t*>::iterator  itr = mapVect.begin();

  for( mapIndex=0; mapIndex<mapSize; mapIndex++,itr++ ){
    if( map.getEndAddr() < mapVect[mapIndex]->getStartAddr() ){
      if( mapIndex > 0 ){
	if( map.getStartAddr() <= mapVect[mapIndex-1]->getEndAddr() ){
	  char errorMessageBuf[1024];
	  sprintf( errorMessageBuf, 
		   "Domain of %s overlaps with %s.",
		   map.getSocketName(),
		   mapVect[mapIndex-1]->getSocketName());
	  CmErrMsg::printErrorMessage( this->name(), "loadMap", 
			     errorMessageBuf );
	  return( false );
	}
      }
      mapVect.insert( itr, &map );
      return( true );
    }
  }
  if( mapIndex > 0 ){
    if( map.getStartAddr() <= mapVect[mapSize-1]->getEndAddr() ){
      char errorMessageBuf[1024];
      sprintf( errorMessageBuf, 
	       "Domain of %s overlaps with %s.",
	       map.getSocketName(),
	       mapVect[mapIndex-1]->getSocketName());
      CmErrMsg::printErrorMessage( this->name(), "loadMap", 
			 errorMessageBuf );
      return( false );
    }
  }
  mapVect.push_back( &map );
  return( true );
}

// add debugAccess checking 20090721
template< unsigned int BUSWIDTH >
bool
OSCI2DCDR<BUSWIDTH>::decode( TlmTransactionType& trans )
{
  ADDRESS_TYPE addr        = trans.get_address() & mAddressMask;
  bool         targetReply = false;

  int min = 0;
  int max = mpInterfaceMapSize - 1;
  int mid;

  while( min <= max ){
    mid = (min+max)/2;
    if( mid == max ){
      if( addr >= mpInterfaceMap[mid]->getStartAddr() &&
	  addr <= mpInterfaceMap[mid]->getEndAddr() ){
	mpInterface     = mpInterfaceMap[mid]->getInterface();
	mpStub          = mpInterfaceMap[mid]->getStub();
	mpOffsetAddress = 
	  (unsigned int)(addr-mpInterfaceMap[mid]->getStartAddr());
	targetReply = true;
      }
      break;
    }
    else if( addr < mpInterfaceMap[mid]->getStartAddr() ){
      max = mid - 1;
    }
    else if( addr >= mpInterfaceMap[mid+1]->getStartAddr() ){
      min = mid + 1;
    }
    else{
      if( addr <= mpInterfaceMap[mid]->getEndAddr() ){
	mpInterface     = mpInterfaceMap[mid]->getInterface();
	mpStub          = mpInterfaceMap[mid]->getStub();
	mpOffsetAddress = 
	  (unsigned int)(addr-mpInterfaceMap[mid]->getStartAddr());
	targetReply = true;
      }
      break;
    }
  }
  return targetReply;
}


template< unsigned int BUSWIDTH >
TlmSyncEnum 
OSCI2DCDR<BUSWIDTH>::
nb_transport_fw(TlmTransactionType& trans, TlmPhase& phase, sc_core::sc_time& t)
{
  outputLog( &trans, this, TRANS_AT_REQ, &t );
#if defined(BUSIF_TST)
  if(tstFp){
    fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
    fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
    fprintf(tstFp,"DECODER:nb_transport_fw() invoked\n");
    if( trans.get_command() == tlm::TLM_READ_COMMAND )
      fprintf(tstFp,"        TLM_READ_COMMAND\n");
    else
      fprintf(tstFp,"        TLM_WRITE_COMMAND\n");
    fprintf(tstFp,"        address     =0x%llx\n",
	    trans.get_address());
    int dataLength = trans.get_data_length();
    fprintf(tstFp,"        dataLength  =%d\n",
	    dataLength);
    unsigned char *data = trans.get_data_ptr();
    for(int i=0; i<dataLength; i++)
      fprintf(tstFp,"        data[%d]     =0x%x\n",
	      i,data[i]);
    fprintf(tstFp,"        streamWidth=%d\n",
	    trans.get_streaming_width());
    if( trans.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE )
      fprintf(tstFp,"        TLM_INCOMPLETE_RESPONSE\n");
    else
      fprintf(tstFp,"        responseStatus error\n");
    TlmG3mExtension *pG3mExtension = NULL;
    trans.get_extension( pG3mExtension );
    if( pG3mExtension ){
      fprintf(tstFp,"        IfPointer   =0x%llx\n",
	      pG3mExtension->getIfPointer());
    }
  }
#endif // BUSIF_TST

  if( !decode(trans) ){
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf, "There is no slave at 0x%x.",
	     (ADDRESS_TYPE)(trans.get_address()&mAddressMask) );
    CmErrMsg::printErrorMessage( this->name(), "decode", errorMessageBuf );
    return( tlm::TLM_ACCEPTED );
  }
  if( mpInterface ){
    return(mpInterface->nb_transport_fw(trans,phase,t));
  }
  else{
    stubAccess( trans );
    phase = tlm::END_REQ;
    mStubRespEventPEQ.notify( trans );
    return(tlm::TLM_UPDATED);
  }
}


template< unsigned int BUSWIDTH >
TlmSyncEnum 
OSCI2DCDR<BUSWIDTH>::
nb_transport_bw(TlmTransactionType& trans, TlmPhase& phase, sc_core::sc_time& t)
{
#if defined(BUSIF_TST)
    if(tstFp){
      fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
      fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,"DECORDER:nb_transport_bw() invoked\n");
    }
#endif //BUSIF_TST
  outputLog( &trans, this, TRANS_AT_RES, &t );
  return( ts->nb_transport_bw(trans, phase, t) );
}


template< unsigned int BUSWIDTH >
unsigned int
OSCI2DCDR<BUSWIDTH>::
transport_dbg( TlmTransactionType& r )
{

  outputLog( &r, this, TRANS_DBG_REQ );
#if defined(BUSIF_TST)
  fflush(stdout);
  if(tstFp){
    fprintf(tstFp,"%s:",sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
    fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
    fprintf(tstFp,"DECORDER:transport_dbg() invoked\n");
    if( r.get_command() == tlm::TLM_READ_COMMAND )
      fprintf(tstFp,"        TLM_READ_COMMAND\n");
    else
      fprintf(tstFp,"        TLM_WRITE_COMMAND\n");
    fprintf(tstFp,"        address     =0x%llx\n",
	    r.get_address());
    int dataLength = r.get_data_length();
    fprintf(tstFp,"        dataLength  =%d\n",
	    dataLength);
    unsigned char *data = r.get_data_ptr();
    for(int i=0; i<dataLength; i++)
      fprintf(tstFp,"        data[%d]     =0x%x\n",
	      i,data[i]);
    fprintf(tstFp,"        streamWidth=%d\n",
	    r.get_streaming_width());
    if( r.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE )
      fprintf(tstFp,"        TLM_INCOMPLETE_RESPONSE\n");
    else
      fprintf(tstFp,"        responseStatus error\n");
    TlmG3mExtension *pG3mExtension = NULL;
    r.get_extension( pG3mExtension );
    if( pG3mExtension ){
      fprintf(tstFp,"        IfPointer   =0x%llx\n",
	      pG3mExtension->getIfPointer());
    }
  }
  fflush(stdout);
#endif // BUSIF_TST

  if( !decode(r) ){
    r.set_response_status(tlm::TLM_OK_RESPONSE);
    return( r.get_data_length() );
  }

  unsigned int retVal = 0;

  if( mpInterface ){
    retVal = mpInterface->transport_dbg( r );
  }
  else{
    stubAccess( r );
    r.set_response_status(tlm::TLM_OK_RESPONSE);
    retVal = r.get_data_length();
  }

  outputLog( &r, this, TRANS_DBG_RES );
  return( retVal );

}


template< unsigned int BUSWIDTH >
void 
OSCI2DCDR<BUSWIDTH>::
invalidate_direct_mem_ptr( sc_dt::uint64 startRange,
			   sc_dt::uint64 endRange ){
#if 0 // DMI is not supported
  ts->invalidate_direct_mem_ptr(startRange, endRange);
#endif// DMI is not supported
}


template< unsigned int BUSWIDTH >
bool
OSCI2DCDR<BUSWIDTH>::
get_direct_mem_ptr( TlmTransactionType& trans, tlm::tlm_dmi&  dmiData )
{
#if 0 // DMI is not supportd
  if( !decode(trans) ){
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf, "There is no slave at 0x%x.",
	     (ADDRESS_TYPE)(trans.get_address()&mAddressMask) );
    CmErrMsg::printErrorMessage( this->name(), "decode", errorMessageBuf );
    return( false );
  }
  return( mpInterface->get_direct_mem_ptr(trans,dmiData) );
#else // DMI is not supported
  return( false );
#endif// DMI is not supported
}


template< unsigned int BUSWIDTH >
void
OSCI2DCDR<BUSWIDTH>::
b_transport( TlmTransactionType& trans, sc_core::sc_time& t )
{
  outputLog( &trans, this, TRANS_LT_REQ, &t );
#if defined(BUSIF_TST)
  fflush(stdout);
  if(tstFp){
    fprintf(tstFp,"%s:DECODER:b_transport() invoked\n",
	    sc_time_stamp().to_string().c_str());
    if( trans.get_command() == tlm::TLM_READ_COMMAND )
      fprintf(tstFp,"        TLM_READ_COMMAND\n");
    else
      fprintf(tstFp,"        TLM_WRITE_COMMAND\n");
    fprintf(tstFp,"        address     =0x%llx\n",
	    trans.get_address());
    int dataLength = trans.get_data_length();
    fprintf(tstFp,"        dataLength  =%d\n",
	    dataLength);
    unsigned char *data = trans.get_data_ptr();
    for(int i=0; i<dataLength; i++)
      fprintf(tstFp,"        data[%d]     =0x%x\n",
	      i,data[i]);
    fprintf(tstFp,"        streamWidth=%d\n",
	    trans.get_streaming_width());
    if( trans.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE )
      fprintf(tstFp,"        TLM_INCOMPLETE_RESPONSE\n");
    else
      fprintf(tstFp,"        responseStatus error\n");
    TlmG3mExtension *pG3mExtension = NULL;
    trans.get_extension( pG3mExtension );
    if( pG3mExtension ){
      fprintf(tstFp,"        IfPointer   =0x%llx\n",
	      pG3mExtension->getIfPointer());
    }
  }
  fflush(stdout);
#endif // BUSIF_TST

  if( !decode(trans) ){
    char errorMessageBuf[1024];
    sprintf( errorMessageBuf, "There is no slave at 0x%x.",
	     (ADDRESS_TYPE)(trans.get_address()&mAddressMask) );
    CmErrMsg::printErrorMessage( this->name(), "decode", errorMessageBuf );
    return;
  }

  if( mpInterface ){
    mpInterface->b_transport(trans, t);
  }
  else{
    stubAccess( trans );
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    trans.set_dmi_allowed(false);
  }

  outputLog( &trans, this, TRANS_LT_RES, &t );

}


template< unsigned int BUSWIDTH >
void
OSCI2DCDR<BUSWIDTH>::
stubAccess( TlmTransactionType& trans )
{
  if( trans.get_command() == tlm::TLM_WRITE_COMMAND ){
    memcpy( &mpStub[mpOffsetAddress], 
	    trans.get_data_ptr(), 
	    trans.get_data_length() );
  }
  else{
    memcpy( trans.get_data_ptr(),
	    &mpStub[mpOffsetAddress], 
	    trans.get_data_length() );
  }
}


template< unsigned int BUSWIDTH >
void
OSCI2DCDR<BUSWIDTH>::
stubRespProcess( void )
{
  TlmTransactionType *trans;
  TlmPhase            phase;
  sc_time             t;
  while ((trans = mStubRespEventPEQ.get_next_transaction()) != 0) {
    phase = tlm::BEGIN_RESP;
    t     = SC_ZERO_TIME;
    trans->set_response_status(tlm::TLM_OK_RESPONSE);
#ifdef BUSIF_TST
    if(tstFp){
      fprintf(tstFp,"%s:",
	      sc_time_stamp().to_string().c_str());
#if defined(PLTFRM_TST)
      fprintf(tstFp,"%s\n        ",this->name());
#endif //PLTFRM_TST
      fprintf(tstFp,
	      "DECODER::Call nb_transport_bw() from %s\n",
	      ts.name());
      if( trans->get_command() == tlm::TLM_READ_COMMAND )
	fprintf(tstFp,"        TLM_READ_COMMAND\n");
      else
	fprintf(tstFp,"        TLM_WRITE_COMMAND\n");
      fprintf(tstFp,"        address     =0x%llx\n",
	    trans->get_address());
      int dataLength = trans->get_data_length();
      fprintf(tstFp,"        dataLength  =%d\n",
	      dataLength);
      unsigned char *data = trans->get_data_ptr();
      for(int i=0; i<dataLength; i++)
	fprintf(tstFp,"        data[%d]     =0x%x\n",
		i,data[i]);
      fprintf(tstFp,"        streamWidth=%d\n",
	      trans->get_streaming_width());
      if( phase == tlm::BEGIN_RESP ){
	fprintf(tstFp,
		"             phase =tlm::BEGIN_RESP\n");
      }
      else{
	fprintf(tstFp,
		"             phase =other\n");
      }
      fprintf(tstFp,"        t =%s\n",
	      t.to_string().c_str());
    }
#endif //BUSIF_TST
    ts->nb_transport_bw( *trans, phase, t );
  }
}


#endif // !__OSCI2DCDR_H__
