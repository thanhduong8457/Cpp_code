#include "BusSlaveFuncIf.h"

class MemoryFunction : public BusSlaveFuncIf
{
 public:
  MemoryFunction(void);
  ~MemoryFunction(void);
  //Virtual functions of BusSlaveFuncIf class
  void read(unsigned int offsetAddress, TlmBasicPayload& trans, BusTime_t* t, bool debug);
  void write(unsigned int offsetAddress, TlmBasicPayload& trans, BusTime_t* t, bool debug);

  void setfunc( ADDRESS_TYPE size );
 private:
  unsigned char *mMem;
};
