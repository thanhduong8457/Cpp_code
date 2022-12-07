#include<iostream>
#include "systemc.h"

void HPB_Thread()
{
//Create transaction object
tlm::tlm_generic_payload hpb_transport;

//Set information for Generic payload
hpb_transport.set_address(addr);
hpb_transport.set_command(tlm::TLM_READ_COMMAND);
hpb_transport.set_data_length(dsize);
hpb_transport.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

// Call b_tranport
sc_time delay_time = SC_ZERO_TIME;
ini_socket->b_transport(hpb_transport, delay_time);
}

int main()
{

    return 0;
}
