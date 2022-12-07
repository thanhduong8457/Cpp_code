#ifndef GLOBAL_H
#define GLOBAL_H


#define PE_MAX_NUM     8
#define CL_MAX_NUM     4
#define CRAM_MAX_NUM     8

#define EI_INTC1_CH_NUM  32
#define EI_ALL_CH_NUM  2048
#define FE_INTC1_CH_NUM  16

#define BUS_WIDTH_FLSH  128
#define BUS_WIDTH_VCI    64
#define BUS_WIDTH_VCI32  32
#define BUS_WIDTH_AXI    64
#define BUS_WIDTH_AHB    32
#define BUS_WIDTH_APB    32
#define BUS_WIDTH_VPI    32
#define BUS_WIDTH_CRAM   64

#define DEBUG_MASTER_BMID 0xFF

// function
double org_sc_simulation_time(void);

typedef enum {
  SIM_MODE_CA,
  SIM_MODE_FAST
} SIM_MODE_T;

typedef enum {
  FASTISS_NONE,
  FASTISS_ASTC
} FASTISS_MODE_T;

typedef enum {
  E2X_PFSS,
  U2A_PFSS
} G4_PFSS_T;
#endif /* GLOBAL_H */
