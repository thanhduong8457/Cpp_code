#include "MemoryFunction.h"

MemoryFunction::MemoryFunction(void)
{

}

MemoryFunction::~MemoryFunction(void)
{

}

void MemoryFunction::read(unsigned int offsetAddress, TlmBasicPayload& trans, BusTime_t* t, bool debug)
{
  //memcpy(trans.get_data_ptr, &mMem[trans.get_address], trans.get_data_length );
}

void MemoryFunction::write(unsigned int offsetAddress, TlmBasicPayload& trans, BusTime_t* t, bool debug)
{
  //memcpy( &mMem[trans.get_data_ptr], trans.get_address, trans.get_data_length);
}

void MemoryFunction::setfunc( ADDRESS_TYPE size )
{
  mMem = new unsigned char [size];
  for( int i=0; i<size; i++ )  mMem[i] = 0;
}
