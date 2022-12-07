/*
* Copyright(c) 2008-2017 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation.
*/

#ifndef OSCI2_H
#define OSCI2_H

#include "systemc.h"
#include "tlm.h"

typedef unsigned int                    ADDRESS_TYPE;

// bus protocol
typedef enum
{
  BUS_APB = 0, //APB
  BUS_AHB = 1, //AHB
  BUS_AXI = 2, //AXI
  BUS_VPI = 3, //VPI
  BUS_VCI = 4  //VCI
} BusProtocol_t;

//backward transport I/F
typedef tlm::tlm_bw_transport_if<> 	TlmBwTransportIf;
//forward transport I/F
typedef tlm::tlm_fw_transport_if<> 	TlmFwTransportIf;
//TLM generic payload
typedef tlm::tlm_generic_payload       	TlmTransactionType;
//TLM phase
typedef tlm::tlm_phase                  TlmPhase;
//TLM synchronization enum
typedef tlm::tlm_sync_enum              TlmSyncEnum;
//TLM generic payload for debug access
typedef tlm::tlm_generic_payload        TlmDebugTransactionType;
//TLM generic payload for normal access
typedef tlm::tlm_generic_payload        TlmBasicPayload;

//sc_time
typedef sc_time                         BusTime_t;
//sc_time_unit
typedef sc_time_unit                    BusTimeUnit_t;

template<unsigned int BUSWIDTH> class TlmTargetSocket;
template<unsigned int BUSWIDTH> class TlmInitiatorSocket;

/*--- class TlmTargetSocket ---*/
template<unsigned int BUSWIDTH>
class TlmTargetSocket :
public tlm::tlm_target_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>
{
  typedef void (sc_module::*tFuncPtr)();
public:
  /// <summary>constructor</summary>
  /// <param name="name">socket name</param>
  TlmTargetSocket(const char* name):
    tlm::tlm_target_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>(name),
    mBaseAddress((ADDRESS_TYPE)0),
    mBaseSize((ADDRESS_TYPE)0),
    mMod((sc_module *)0),
    mFuncPtrBaseAddressSize((tFuncPtr)0)
  {
    mTargetSocketList.erase_all();//for decoder
    mPureTlmSocketList.erase_all();//for pure TLM connection
  }
  
  /// <summary>bind function</summary>
  /// <param name="s">pure TLM target socket</param>
  void bind(tlm::tlm_base_target_socket_b<BUSWIDTH>& s){
    tlm::tlm_target_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>::bind(s);
    mTargetSocketList.push_back((TlmTargetSocket<BUSWIDTH> *)&s);//for decoder
    mPureTlmSocketList.push_back((int)1);//for pure TLM connection
  }
  
  /// <summary>operator()</summary>
  /// <param name="s">pure TLM target socket</param>
  void operator() (tlm::tlm_base_target_socket_b<BUSWIDTH>& s){
    bind(s);
  }
  
  /// <summary>bind function</summary>
  /// <param name="s">target socket</param>
  void bind(TlmTargetSocket<BUSWIDTH> &s){
    tlm::tlm_target_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>::bind(s);
    mTargetSocketList.push_back(&s);//for decoder
    mPureTlmSocketList.push_back((int)0);//for pure TLM connection
  }
  
  /// <summary>operator()</summary>
  /// <param name="s">target socket</param>
  void operator() (TlmTargetSocket<BUSWIDTH> &s){
    bind(s);
  }
  
  /// <summary>operator()</summary>
  /// <param name="s">forward transport I/F</param>
  void operator() (TlmFwTransportIf& s){
    tlm::tlm_target_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>::bind(s);
  }
  
  /// <summary>set base address and size</summary>
  /// <param name="address">address</param>
  /// <param name="size">size</param>
  void setBaseAddressSize(ADDRESS_TYPE address, ADDRESS_TYPE size) {
    if ( getTSocketListSize() ){
      return(getTSocket(0)->setBaseAddressSize(address, size));
    }
    else {
      mBaseAddress = address;
      mBaseSize =size;
      if ((mMod != 0) && (mFuncPtrBaseAddressSize !=0)) {
	(mMod->*mFuncPtrBaseAddressSize)();
      }
    }
  }
  
  /// <summary>get base address</summary>
  /// <returns>base address</returns>
  ADDRESS_TYPE getBaseAddress(void){
    return(mBaseAddress);
  }
  
  /// <summary>get memory size</summary>
  /// <returns>memory size</returns>
  ADDRESS_TYPE getBaseSize(void) {
    return(mBaseSize);
  }

  /// <summary>set function for base addres and size setting</summary>
  /// <param name="mod">class</param>
  /// <param name="func">size</param>
  template< typename MOD >
  void setFuncPtrBaseAddressSize(MOD *mod, void (MOD::*func)()){
    mMod  = mod;
    mFuncPtrBaseAddressSize = static_cast<tFuncPtr>(func);
  }
  
  /// <summary>get target socket</summary>
  /// <param name="target_id">target socket ID</param>
  TlmTargetSocket<BUSWIDTH> *getTSocket(int target_id){    //for decoder
    return( mTargetSocketList.fetch( target_id ) );
  }

  /// <summary>get target socket list size</summary>
  /// <returns>target socket list size</returns>
  int getTSocketListSize(void){//for decoder
    return( mTargetSocketList.size() );
  }

  /// <summary>return if pure TLM target socket is</summary>
  /// <param name="socket_id">socket ID</param>
  /// <returns>if pure TLM target socket is</returns>
  int isPureTlmSocket(int socket_id){    //for pure TLM connection
    return( mPureTlmSocketList.fetch( socket_id ) );
  }

 public:
  ADDRESS_TYPE    mBaseAddress; //base address
  ADDRESS_TYPE    mBaseSize; //memory size
  sc_module       *mMod; //class
  tFuncPtr        mFuncPtrBaseAddressSize; // funciton pointer
  sc_pvector< TlmTargetSocket<BUSWIDTH> * > mTargetSocketList; //vector of target socket
  sc_pvector< int > mPureTlmSocketList; //if pure TLM socket
};


/*--- class TlmInitiatorSocket ---*/
template<unsigned int BUSWIDTH>
class TlmInitiatorSocket : 
public tlm::tlm_initiator_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>
{
 public:
  /// <summary>constructor</summary>
  /// <param name="name">socket name</param>
  TlmInitiatorSocket(const char* name) :
  tlm::tlm_initiator_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>(name){
    mTargetSocketList.erase_all();//for decoder
    mInitiatorSocketList.erase_all();//for decoder
    mPureTlmSocketList.erase_all();//for pure TLM connection
  }
  
  /// <summary>bind function</summary>
  /// <param name="s">pure TLM target socket</param>
  void bind(tlm::tlm_base_target_socket_b<BUSWIDTH>& s){
    tlm::tlm_initiator_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>::bind(s);
    mTargetSocketList.push_back((TlmTargetSocket<BUSWIDTH> *)&s);//for decoder
    mInitiatorSocketList.push_back((TlmInitiatorSocket<BUSWIDTH> *)0);//for decoder
    mPureTlmSocketList.push_back((int)1);//for pure TLM connection
  }
  
  /// <summary>operator()</summary>
  /// <param name="s">pure TLM target socket</param>
  void operator() (tlm::tlm_base_target_socket_b<BUSWIDTH>& s){
    bind(s);
  }
  
  /// <summary>bind function</summary>
  /// <param name="s">target socket</param>
  void bind(TlmTargetSocket<BUSWIDTH> &s){
    tlm::tlm_initiator_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>::bind(s);
    mTargetSocketList.push_back(&s);//for decoder
    mInitiatorSocketList.push_back((TlmInitiatorSocket<BUSWIDTH> *)0);//for decoder
    mPureTlmSocketList.push_back((int)0);//for pure TLM connection
  }
  
  /// <summary>operator()</summary>
  /// <param name="s">target socket</param>
  void operator() (TlmTargetSocket<BUSWIDTH> &s){
    bind(s);
  }
  
  /// <summary>bind function</summary>
  /// <param name="s">pure TLM initiator socket</param>
  void bind(tlm::tlm_base_initiator_socket_b<BUSWIDTH>& s){
    tlm::tlm_initiator_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>::bind(s);
    mInitiatorSocketList.push_back((TlmInitiatorSocket<BUSWIDTH> *)&s);//for decoder
    mTargetSocketList.push_back((TlmTargetSocket<BUSWIDTH> *)0);//for decoder
    mPureTlmSocketList.push_back((int)1);//for pure TLM connection
  }
  
  /// <summary>operator()</summary>
  /// <param name="s">pure TLM initiator socket</param>
  void operator() (tlm::tlm_base_initiator_socket_b<BUSWIDTH>& s){
    bind(s);
  }
  
  /// <summary>bind function</summary>
  /// <param name="s">initiator socket</param>
  void bind(TlmInitiatorSocket<BUSWIDTH> &s){
    tlm::tlm_initiator_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>::bind(s);
    mInitiatorSocketList.push_back(&s);//for decoder
    mTargetSocketList.push_back((TlmTargetSocket<BUSWIDTH> *)0);//for decoder
    mPureTlmSocketList.push_back((int)0);//for pure TLM connection
  }
  
  /// <summary>operator()</summary>
  /// <param name="s">initiator socket</param>
  void operator() (TlmInitiatorSocket<BUSWIDTH> &s){
    bind(s);
  }
  
  /// <summary>operator()</summary>
  /// <param name="s">backward transport I/F</param>
  void operator() (TlmBwTransportIf& s){
    tlm::tlm_initiator_socket<BUSWIDTH,tlm::tlm_base_protocol_types,0>::bind(s);
  }
  
  /// <summary>get target socket</summary>
  /// <param name="target_id">target socket ID</param>
  TlmTargetSocket<BUSWIDTH> *getTSocket(int target_id){ //for decoder
    return( mTargetSocketList.fetch( target_id ) );
  }

  /// <summary>get initiator socket</summary>
  /// <param name="initiator_id">initiator socket ID</param>
  TlmInitiatorSocket<BUSWIDTH> *getISocket(int initiator_id){ //for decoder
    return( mInitiatorSocketList.fetch( initiator_id ) );
  }

  /// <summary>get target socket list size</summary>
  /// <returns>target socket list size</returns>
  int getTSocketListSize(void){ //for decoder
    return( mTargetSocketList.size() );
  }

  /// <summary>return if pure TLM target socket is</summary>
  /// <param name="socket_id">socket ID</param>
  /// <returns>if pure TLM target socket is</returns>
  int isPureTlmSocket(int socket_id){    //for pure TLM connection
    return( mPureTlmSocketList.fetch( socket_id ) );
  }

 public:
  sc_pvector< TlmInitiatorSocket<BUSWIDTH> * > mInitiatorSocketList;//vector of initiator socket
  sc_pvector< TlmTargetSocket<BUSWIDTH> * > mTargetSocketList;//vector of target socket
  sc_pvector< int > mPureTlmSocketList;//vector of information if pure TLM socket is
};


/*--- class TlmG3mExtension ---*/
class TlmG3mExtension : 
public tlm::tlm_extension<TlmG3mExtension>
{
 public:
  /// <summary>constructor</summary>
  TlmG3mExtension(void) : 
    mDmaAccess(false),
    mVmId((unsigned char)0),
    mTcId((unsigned char)0),
    mPeId((unsigned char)0),
    mSpId((unsigned char)0),
    mUserMode(false),
    mVirtualMode(false),
    mSpcLd(false),
    mSecure(false),
    mpIfPointer((TlmBwTransportIf *)0){}

  /// <summary>destructor</summary>
  ~TlmG3mExtension(void){}

  /// <summary>create clone</summary>
  /// <returns>pointer of clone</returns>
  tlm::tlm_extension_base* clone(void) const{
    TlmG3mExtension *t = new TlmG3mExtension;
    t->mDmaAccess = this->mDmaAccess;
    t->mVmId = this->mVmId;
    t->mTcId = this->mTcId;
    t->mPeId = this->mPeId;
    t->mSpId = this->mSpId;
    t->mUserMode = this->mUserMode;
    t->mVirtualMode = this->mVirtualMode;
    t->mSpcLd = this->mSpcLd;
    t->mSecure = this->mSecure;
    t->mpIfPointer = this->mpIfPointer;
    return t;
  }

  /// <summary>copy member</summary>
  /// <param name="extension">origin of copy</param>
  void copy_from( const tlm_extension_base &extension ){
    mDmaAccess
      = static_cast<TlmG3mExtension const &>(extension).mDmaAccess;
    mVmId
      = static_cast<TlmG3mExtension const &>(extension).mVmId;
    mTcId
      = static_cast<TlmG3mExtension const &>(extension).mTcId;
    mPeId
      = static_cast<TlmG3mExtension const &>(extension).mPeId;
    mSpId
      = static_cast<TlmG3mExtension const &>(extension).mSpId;
    mUserMode
      = static_cast<TlmG3mExtension const &>(extension).mUserMode;
    mVirtualMode
      = static_cast<TlmG3mExtension const &>(extension).mVirtualMode;
    mSpcLd
      = static_cast<TlmG3mExtension const &>(extension).mSpcLd;
    mSecure
      = static_cast<TlmG3mExtension const &>(extension).mSecure;
    mpIfPointer
      = static_cast<TlmG3mExtension const &>(extension).mpIfPointer;
  }

  /// <summary>set function for DMA access</summary>
  /// <param name="dmaAccess">if DMA access is</param>
  void setDmaAccess( bool dmaAccess ){ mDmaAccess = dmaAccess; }
  /// <summary>get function for DMA access</summary>
  /// <returns>return if DMA access is</returns>
  bool isDmaAccess( void ){ return mDmaAccess; }
  /// <summary>set function for VM ID</summary>
  /// <param name="vmId">VM ID</param>
  void setVmId( unsigned char vmId ){ mVmId = vmId; }
  /// <summary>get function for VM ID</summary>
  /// <returns">VM ID</param>
  unsigned char getVmId( void ){ return mVmId; }
  /// <summary>set function for TC ID</summary>
  /// <param name="tcId">TC ID</param>
  void setTcId( unsigned char tcId ){ mTcId = tcId; }
  /// <summary>get function for TC ID</summary>
  /// <returns">TC ID</param>
  unsigned char getTcId( void ){ return mTcId; }
  /// <summary>set function for PE ID</summary>
  /// <param name="peId">PE ID</param>
  void setPeId( unsigned char peId ){ mPeId = peId; }
  /// <summary>get function for PE ID</summary>
  /// <returns">PE ID<returns>
  unsigned char getPeId( void ){ return mPeId; }
  /// <summary>set function for SP ID</summary>
  /// <param name="spId">SP ID</param>
  void setSpId( unsigned char spId ){ mSpId = spId; }
  /// <summary>get function for SP ID</summary>
  /// <returns">SP ID</returns>
  unsigned char getSpId( void ){ return mSpId; }
  /// <summary>set function for user mode information</summary>
  /// <param name="userMode">user mode</param>
  void setUserMode( bool userMode ){ mUserMode = userMode; }
  /// <summary>get function for user mode information</summary>
  /// <returns">if user mode is</returns>
  bool isUserMode( void ){ return mUserMode; }
  /// <summary>set function for virtual mode information</summary>
  /// <param name="virtualMode">virtual mode</param>
  void setVirtualMode( bool virtualMode ){ mVirtualMode = virtualMode; }
  /// <summary>get function for virtual mode information</summary>
  /// <returns">if virtual mode is</returns>
  bool isVirtualMode( void ){ return mVirtualMode; }
  /// <summary>set function for speculation load flag</summary>
  /// <param name="spcLd">speculation load flag</param>
  void setSpcLd( bool spcLd ){ mSpcLd = spcLd; }
  /// <summary>get function for speculation load flag</summary>
  /// <returns">if speculation load is</returns>
  bool isSpcLd( void ){ return mSpcLd; }
  /// <summary>set function for secure master request information</summary>
  /// <param name="secure">secure master request information</param>
  void setSecure( bool secure ){ mSecure = secure; }
  /// <summary>get function for secure master request information</summary>
  /// <returns">if secure master request is</returns>
  bool isSecure( void ){ return mSecure; }
  /// <summary>set function for I/F pointer</summary>
  /// <param name="pIf">I/F pointer</param>
  void setIfPointer( TlmBwTransportIf *pIf ){ mpIfPointer = pIf; }
  /// <summary>get function for I/F pointer</summary>
  /// <returns">I/F pointer</returns>
  TlmBwTransportIf *getIfPointer( void ){ return mpIfPointer; }

 private:
  bool             mDmaAccess; // DMA access
  unsigned char    mVmId; //VM ID
  unsigned char    mTcId; //TC ID
  unsigned char    mPeId; //PE ID
  unsigned char    mSpId; //SP ID
  bool             mUserMode; //user mode
  bool             mVirtualMode; //virtual mode
  bool             mSpcLd; //speculation load flag
  bool             mSecure; //secure master request
  TlmBwTransportIf *mpIfPointer; //I/F pointer

};


/*--- class TlmAxiExtension ---*/
class TlmAxiExtension : 
public tlm::tlm_extension<TlmAxiExtension>
{
 public:

  // Burst type
  typedef enum{ BURST_FIXED=0, 
		BURST_INCR=1, 
		BURST_WRAP=2
	      }AxiBurst_t;

  // Bit operation type
  typedef enum{ BITOP_NON=0, 
		BITOP_SET1=1, 
		BITOP_CLR1=2, 
		BITOP_NOT1=3 
	      }AxiBitop_t;

  /// <summary>constructor</summary>
  TlmAxiExtension(void) : 
    mBurstType(BURST_FIXED),
    mLock(false),
    mCachable(false),
    mBufferable(false),
    mSecure((unsigned char)0),
    mTransId((unsigned int)0),
    mBitOpType(BITOP_NON),
    mBitOpPos((unsigned char)0){}

  /// <summary>destructor</summary>
  ~TlmAxiExtension(void){}

  /// <summary>create clone</summary>
  /// <returns>pointer of clone</returns>
  tlm::tlm_extension_base* clone(void) const{
    TlmAxiExtension *t = new TlmAxiExtension;
    t->mBurstType = this->mBurstType;
    t->mLock = this->mLock;
    t->mCachable = this->mCachable;
    t->mBufferable = this->mBufferable;
    t->mSecure = this->mSecure;
    t->mTransId = this->mTransId;
    t->mBitOpType = this->mBitOpType;
    t->mBitOpPos = this->mBitOpPos;
    return t;
  }

  /// <summary>copy member</summary>
  /// <param name="extension">origin of copy</param>
  void copy_from( const tlm_extension_base &extension ){
    mBurstType
      = static_cast<TlmAxiExtension const &>(extension).mBurstType;
    mLock
      = static_cast<TlmAxiExtension const &>(extension).mLock;
    mCachable
      = static_cast<TlmAxiExtension const &>(extension).mCachable;
    mBufferable
      = static_cast<TlmAxiExtension const &>(extension).mBufferable;
    mSecure
      = static_cast<TlmAxiExtension const &>(extension).mSecure;
    mTransId
      = static_cast<TlmAxiExtension const &>(extension).mTransId;
    mBitOpType
      = static_cast<TlmAxiExtension const &>(extension).mBitOpType;
    mBitOpPos
      = static_cast<TlmAxiExtension const &>(extension).mBitOpPos;
  }

  /// <summary>set function for burst type</summary>
  /// <param name="burstType">burst type</param>
  void setBurstType( AxiBurst_t burstType ){ mBurstType = burstType; }
  /// <summary>get function for burst type</summary>
  /// <returns>burst type</returns>
  AxiBurst_t getBurstType( void ){ return mBurstType; }
  /// <summary>set function for lock information</summary>
  /// <param name="lock">lock information</param>
  void setLock( bool lock ){ mLock = lock; }
  /// <summary>get function for lock information</summary>
  /// <returns>lock information</returns>
  bool isLock( void ){ return mLock; }
  /// <summary>set function for cachable</summary>
  /// <param name="cachable">if cachable</param>
  void setCachable( bool cachable ){ mCachable = cachable; }
  /// <summary>get function for cachable</summary>
  /// <returns>if cachable</returns>
  bool isCachable( void ){ return mCachable; }
  /// <summary>set function for bufferable</summary>
  /// <param name="bufferable">if bufferable</param>
  void setBufferable( bool bufferable ){ mBufferable = bufferable; }
  /// <summary>get function for bufferable</summary>
  /// <returns>if bufferable</returns>
  bool isBufferable( void ){ return mBufferable; }
  /// <summary>set function for secure information</summary>
  /// <param name="secure">secure information</param>
  void setSecure( unsigned char secure ){ mSecure = secure; }
  /// <summary>get function for secure information</summary>
  /// <returns>secure information</returns>
  unsigned char getSecure( void ){ return mSecure; }
  /// <summary>set function for transaction ID</summary>
  /// <param name="transId">transaction ID</param>
  void setTransId( unsigned int transId ){ mTransId = transId; }
  /// <summary>get function for transaction ID</summary>
  /// <returns>transaction ID</returns>
  unsigned int getTransId( void ){ return mTransId; }
  /// <summary>set function for bit operation type</summary>
  /// <param name="transId">transaction ID</param>
  void setBitOpType( AxiBitop_t bitOpType ){ mBitOpType = bitOpType; }
  /// <summary>get function for bit operation type</summary>
  /// <returns>bit operation type</returns>
  AxiBitop_t getBitOpType( void ){ return mBitOpType; }
  /// <summary>set function for bit position of BitOP</summary>
  /// <param name="transId">bit operation</param>
  void setBitOpPos( unsigned char bitOpPos ){ mBitOpPos = bitOpPos; }
  /// <summary>get function for bit position of BitOP</summary>
  /// <returns>bit position</returns>
  unsigned char getBitOpPos( void ){ return mBitOpPos; }

 private:
  AxiBurst_t    mBurstType; //burst type
  bool          mLock; // lock information
  bool          mCachable; //cachable
  bool          mBufferable; //bufferable
  unsigned char mSecure; //secure informaiton
  unsigned int  mTransId; //transaction ID
  AxiBitop_t    mBitOpType; //bit operation type
  unsigned char mBitOpPos; //bit positin of BitOP

};


/*--- class TlmVpiExtension ---*/
class TlmVpiExtension : 
public tlm::tlm_extension<TlmVpiExtension>
{
 public:

  // request type
  typedef enum
    {
      CMD_READ=0,
      CMD_WRITE=1,
      CMD_BITOP_SET=2,
      CMD_BITOP_AND=3,
      CMD_BITOP_OR=4,
      CMD_BITOP_XOR=5,
      CMD_SYNC_BUS=6,
      CMD_SYNC_MASTER=7
    }VpiRequest_t;

  /// <summary>constructor</summary>
  TlmVpiExtension(void) : 
    mMasterId((unsigned int)0),
    mPacketId((unsigned int)0),
    mSlaveId((unsigned int)0),
    mRequestType(CMD_READ){}

  /// <summary>destructor</summary>
  ~TlmVpiExtension(void){}

  /// <summary>create clone</summary>
  /// <returns>pointer of clone</returns>
  tlm::tlm_extension_base* clone(void) const{
    TlmVpiExtension *t = new TlmVpiExtension;
    t->mMasterId = this->mMasterId;
    t->mPacketId = this->mPacketId;
    t->mSlaveId = this->mSlaveId;
    t->mRequestType = this->mRequestType;
    return t;
  }

  /// <summary>copy member</summary>
  /// <param name="extension">origin of copy</param>
  void copy_from( const tlm_extension_base &extension ){
    mMasterId
      = static_cast<TlmVpiExtension const &>(extension).mMasterId;
    mPacketId
      = static_cast<TlmVpiExtension const &>(extension).mPacketId;
    mSlaveId
      = static_cast<TlmVpiExtension const &>(extension).mSlaveId;
    mRequestType
      = static_cast<TlmVpiExtension const &>(extension).mRequestType;
  }

  /// <summary>set function for master ID</summary>
  /// <param name="tcId">master ID</param>
  void setMasterId( unsigned int masterId ){ mMasterId = masterId; }
  /// <summary>get function for master ID</summary>
  /// <returns>master ID</returns>
  unsigned int getMasterId( void ){ return mMasterId; }
  /// <summary>set function for packet ID</summary>
  /// <param name="packetId">packet ID</param>
  void setPacketId( unsigned int packetId ){ mPacketId = packetId; }
  /// <summary>get function for packet ID</summary>
  /// <returns>packet ID</returns>
  unsigned int getPacketId( void ){ return mPacketId; }
  /// <summary>set function for slave ID</summary>
  /// <param name="slaveId">slave ID</param>
  void setSlaveId( unsigned int slaveId ){ mSlaveId = slaveId; }
  /// <summary>get function for slave ID</summary>
  /// <returns>slave ID</returns>
  unsigned int getSlaveId( void ){ return mSlaveId; }
  /// <summary>set function for request type</summary>
  /// <param name="requestType">request type</param>
  void setRequestType( VpiRequest_t requestType ){ mRequestType = requestType; }
  /// <summary>get function for request type</summary>
  /// <returns>request type</returns>
  VpiRequest_t getRequestType( void ){ return mRequestType; }

 private:
  unsigned int  mMasterId; //master ID
  unsigned int  mPacketId; //packet ID
  unsigned int  mSlaveId; //slave ID
  VpiRequest_t mRequestType; //request ID

};


/*--- class TlmAhbExtension ---*/
class TlmAhbExtension : 
public tlm::tlm_extension<TlmAhbExtension>
{
 public:

  // Burst type
  typedef enum{ BURST_FIXED=0, 
		BURST_INCR=1, 
		BURST_WRAP=2 
	      }AhbBurst_t;

  // access to data or OP code
  typedef enum{ ACC_DATA=0, 
		ACC_OP_CODE=1 
	      }AhbDataOrOp_t;

  /// <summary>constructor</summary>
  TlmAhbExtension(void) : 
    mBurstType(BURST_FIXED),
    mLock(false),
    mCachable(false),
    mBufferable(false),
    mPrivilege(false),
    mDataOrOp(ACC_DATA){}

  /// <summary>destructor</summary>
  ~TlmAhbExtension(void){}

  /// <summary>create clone</summary>
  /// <returns>pointer of clone</returns>
  tlm::tlm_extension_base* clone(void) const{
    TlmAhbExtension *t = new TlmAhbExtension;
    t->mBurstType = this->mBurstType;
    t->mLock = this->mLock;
    t->mCachable = this->mCachable;
    t->mBufferable = this->mBufferable;
    t->mPrivilege = this->mPrivilege;
    t->mDataOrOp = this->mDataOrOp;
    return t;
  }

  /// <summary>copy member</summary>
  /// <param name="extension">origin of copy</param>
  void copy_from( const tlm_extension_base &extension ){
    mBurstType
      = static_cast<TlmAhbExtension const &>(extension).mBurstType;
    mLock
      = static_cast<TlmAhbExtension const &>(extension).mLock;
    mCachable
      = static_cast<TlmAhbExtension const &>(extension).mCachable;
    mBufferable
      = static_cast<TlmAhbExtension const &>(extension).mBufferable;
    mPrivilege
      = static_cast<TlmAhbExtension const &>(extension).mPrivilege;
    mDataOrOp
      = static_cast<TlmAhbExtension const &>(extension).mDataOrOp;
  }

  /// <summary>set function for burst type</summary>
  /// <param name="burstType">burst type</param>
  void setBurstType( AhbBurst_t burstType ){ mBurstType = burstType; }
  /// <summary>get function for burst type</summary>
  /// <returns>burst type</returns>
  AhbBurst_t getBurstType( void ){ return mBurstType; }
  /// <summary>set function for lock information</summary>
  /// <param name="lock">lock information</param>
  void setLock( bool lock ){ mLock = lock; }
  /// <summary>get function for lock information</summary>
  /// <returns>lock information</returns>
  bool isLock( void ){ return mLock; }
  /// <summary>set function for cachable</summary>
  /// <param name="cachable">if cachable</param>
  void setCachable( bool cachable ){ mCachable = cachable; }
  /// <summary>get function for cachable</summary>
  /// <returns>if cachable</returns>
  bool isCachable( void ){ return mCachable; }
  /// <summary>set function for bufferable</summary>
  /// <param name="bufferable">if bufferable</param>
  void setBufferable( bool bufferable ){ mBufferable = bufferable; }
  /// <summary>get function for bufferable</summary>
  /// <returns>if bufferable</returns>
  bool isBufferable( void ){ return mBufferable; }
  /// <summary>set function for privilege information</summary>
  /// <param name="privilege">privilege information</param>
  void setPrivilege( bool privilege ){ mPrivilege = privilege; }
  /// <summary>get function for privilege information</summary>
  /// <returns>privilege information</returns>
  bool isPrivilege( void ){ return mPrivilege; }
  /// <summary>set data access or OP code access</summary>
  /// <param name="DataOrOp">data or OP code</param>
  void setDataOrOp( AhbDataOrOp_t dataOrOp ){ mDataOrOp = dataOrOp; }
  /// <summary> get information which data or OP code access</summary>
  /// <returns>data or OP code</returns>
  AhbDataOrOp_t getDataOrOp( void ){ return mDataOrOp; }

 private:
  AhbBurst_t    mBurstType; // burst type
  bool          mLock; // lock
  bool          mCachable; //cachable
  bool          mBufferable; //bufferable
  bool          mPrivilege; //privilege
  AhbDataOrOp_t mDataOrOp; //data or OP code

};


/*--- class TlmApbExtension ---*/
class TlmApbExtension : 
public tlm::tlm_extension<TlmApbExtension>
{
 public:
  /// <summary>constructor</summary>
  TlmApbExtension(void) : 
    mLock(false),
    mExclusion(false){}

  /// <summary>destructor</summary>
  ~TlmApbExtension(void){}

  /// <summary>create clone</summary>
  /// <returns>pointer of clone</returns>
  tlm::tlm_extension_base* clone(void) const{
    TlmApbExtension *t = new TlmApbExtension;
    t->mLock = this->mLock;
    t->mExclusion = this->mExclusion;
    return t;
  }

  /// <summary>copy member</summary>
  /// <param name="extension">origin of copy</param>
  void copy_from( const tlm_extension_base &extension ){
    mLock
      = static_cast<TlmApbExtension const &>(extension).mLock;
    mExclusion
      = static_cast<TlmApbExtension const &>(extension).mExclusion;
  }

  /// <summary>set function for lock information</summary>
  /// <param name="lock">lock information</param>
  void setLock( bool lock ){ mLock = lock; }
  /// <summary>get function for lock information</summary>
  /// <returns>lock information</returns>
  bool isLock( void ){ return mLock; }
  /// <summary>set function for exclusion information</summary>
  /// <param name="exclusion">exclusion information</param>
  void setExclusion( bool exclusion ){ mExclusion = exclusion; }
  /// <summary>get function for exclusion information</summary>
  /// <returns>exlcution information</returns>
  bool isExclusion( void ){ return mExclusion; }

 private:
  bool mLock; //lock
  bool mExclusion; //exclusion

};


/*--- class TlmVciExtension ---*/
class TlmVciExtension : 
public tlm::tlm_extension<TlmVciExtension>
{
 public:

  // request type
  typedef enum
    {
      CMD_READ=0,
      CMD_WRITE=1,
      CMD_BITOP_SET=2,
      CMD_BITOP_AND=3,
      CMD_BITOP_OR=4,
      CMD_BITOP_XOR=5,
      CMD_SYNC_BUS=6,     //not used for GVCI
      CMD_SYNC_MASTER=7,  //not used for GVCI
      CMD_CAXI=8,
      CMD_EXCL_LDLW=9,
      CMD_EXCL_STCW=10
    }VciRequest_t;

  /// <summary>constructor</summary>
  TlmVciExtension(void) : 
    mRequestType(CMD_READ){}

  /// <summary>destructor</summary>
  ~TlmVciExtension(void){}

  /// <summary>create clone</summary>
  /// <returns>pointer of clone</returns>
  tlm::tlm_extension_base* clone(void) const{
    TlmVciExtension *t = new TlmVciExtension;
    t->mRequestType = this->mRequestType;
    return t;
  }

  /// <summary>copy member</summary>
  /// <param name="extension">origin of copy</param>
  void copy_from( const tlm_extension_base &extension ){
    mRequestType
      = static_cast<TlmVciExtension const &>(extension).mRequestType;
  }

  /// <summary>set function for request type</summary>
  /// <param name="requestType">request type</param>
  void setRequestType( VciRequest_t requestType ){ mRequestType = requestType; }
  /// <summary>get function for request type</summary>
  /// <returns>request type</returns>
  VciRequest_t getRequestType( void ){ return mRequestType; }

 private:
  VciRequest_t mRequestType; //request ID

};


/// <summary>bus access break class</summary>
class BusAccessBreak {

 public:

  // request type
  typedef enum {
    CMD_OTHERS=0,
    CMD_READ=1,
    CMD_WRITE=2,
    CMD_RD_WT=3
  } COMMAND_T;
    
 private:

  // structure of break address
  struct breakAddress_t{
    ADDRESS_TYPE startAddress;
    ADDRESS_TYPE endAddress;
    COMMAND_T    command;
  };

  std::vector< breakAddress_t* > mBreakAddressVector;  // break address vector
  unsigned int                   mAddressVectorSize;   // address vector size
  ADDRESS_TYPE                   mHitAddress;          // break hit address
  
  /// <summary>insert area</summary>
  /// <param name="&area">added address area</param>
  /// <param name="&areaVect">break address vector</param>
  void insertArea( breakAddress_t &area, 
		  std::vector<breakAddress_t*> &areaVect )
  {
    unsigned int areaIndex, areaSize = areaVect.size();
    typename std::vector<breakAddress_t*>::iterator  itr = areaVect.begin();
    
    for( areaIndex=0; areaIndex<areaSize; areaIndex++,itr++ ){
      if( area.startAddress < areaVect[areaIndex]->startAddress ){
	areaVect.insert( itr, &area );
      }
    }
    areaVect.push_back( &area );
  }

  /// <summary>insert area</summary>
  /// <param name="&area">added address area</param>
  bool checkBreak( TlmTransactionType& trans )
  {
    ADDRESS_TYPE addr  = trans.get_address();
    COMMAND_T command  = CMD_OTHERS;
    if( trans.is_read() ){
      command = CMD_READ;
    }
    else{
      command = CMD_WRITE;
    }
    bool         isHit = false;
    
    int min = 0;
    int max = mAddressVectorSize - 1;
    int mid;
    
    while( min <= max ){
      mid = (min+max)/2;
      if( mid == max ){
	if( addr >= mBreakAddressVector[mid]->startAddress &&
	    addr <= mBreakAddressVector[mid]->endAddress   &&
	    command&mBreakAddressVector[mid]->command ){
	  mHitAddress = addr;
	  isHit = true;
	}
	break;
      }
      else if( addr < mBreakAddressVector[mid]->startAddress ){
	max = mid - 1;
      }
      else if( addr >= mBreakAddressVector[mid+1]->startAddress ){
	min = mid + 1;
      }
      else{
	if( addr <= mBreakAddressVector[mid]->endAddress &&
	    command&mBreakAddressVector[mid]->command ){
	  mHitAddress = addr;
	  isHit = true;
	}
	break;
      }
    }
    return isHit;
  }

 public:

  /// <summary>constructer</summary>
  /// <param name="startAddress">start address</param>
  BusAccessBreak( void ) :
  mAddressVectorSize(0),
  mHitAddress(0)
  {}

  /// <summary>destructor</summary>
  ~BusAccessBreak( void )
  {
    for( unsigned int i=0; i<mAddressVectorSize; i++ ){
      delete mBreakAddressVector[i];
    }
    //    mBreakAddressVector.clear();
    mAddressVectorSize = 0;
  }

  /// <summary>set address break point</summary>
  /// <param name="startAddress">start address</param>
  /// <param name="endAddress">end address</param>
  /// <param name="command">command(RD, WT or RD&WT)</param>
  void setBreakAddress( ADDRESS_TYPE startAddress,
			ADDRESS_TYPE endAddress,
			COMMAND_T    command = CMD_RD_WT )
  {
    BusAccessBreak::breakAddress_t *bkAdr = new breakAddress_t();
    bkAdr->startAddress = startAddress;
    bkAdr->endAddress   = endAddress;
    bkAdr->command = command;
    insertArea( *bkAdr, mBreakAddressVector );
    mAddressVectorSize++;
  }
    
  /// <summary>set address break point</summary>
  /// <param name="startAddress">start address</param>
  /// <param name="endAddress">end address</param>
  /// <param name="command">command(RD, WT or RD&WT)</param>
  /// <returns>deleting is done or not</returns>
  bool deleteBreakAddress( ADDRESS_TYPE startAddress,
			   ADDRESS_TYPE endAddress,
			   COMMAND_T    command = CMD_RD_WT )
  {
    bool erase_done = false;
    unsigned int areaIndex, areaSize = mBreakAddressVector.size();
    typename std::vector<breakAddress_t*>::iterator itr = mBreakAddressVector.begin();
    
    for( areaIndex=0; areaIndex<areaSize; areaIndex++,itr++ ){
      if( startAddress == mBreakAddressVector[areaIndex]->startAddress ||
	  endAddress   == mBreakAddressVector[areaIndex]->endAddress   ||
	  command      == mBreakAddressVector[areaIndex]->command){
	mBreakAddressVector.erase( itr );
	mAddressVectorSize--;
	erase_done = true;
      }
    }
    return erase_done;
  }
    
  /// <summary>check address break </summary>
  void checkHitBreakAddress( TlmTransactionType& trans )
  {
    if( mAddressVectorSize ){
      if( checkBreak( trans ) ){
	sc_pause();
      }
    }
  }

  /// <summary>set address break point</summary>
  ADDRESS_TYPE getBreakHitAddress( void )
  {
    ADDRESS_TYPE addr = mHitAddress;
    mHitAddress = 0;
    return( addr );
  }

};
    


#endif // OSCI2_H
