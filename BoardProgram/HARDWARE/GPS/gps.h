#ifndef _GPS_DRIVER_HEAD_
#define _GPS_DRIVER_HEAD_
#include "nmea_0183.h"

#define DMA_USART_BUFFER_SIZE 512
typedef struct st_ld_node{
    char buffer[DMA_USART_BUFFER_SIZE];
    unsigned int gotTick;
    struct st_ld_node *next;
} linkedDMANode;


typedef struct st_lg_node{
    RMCInfo Data;
    unsigned char validFlag;
    unsigned int gotTick;
    struct st_lg_node *next;
} linkedGPSNode;

extern volatile linkedGPSNode * nowWriteGPRMC;
extern volatile linkedGPSNode * nowProcessedGPRMC;

#define GPRMC_BUFFER_SIZE 128
void processDMABuffer( void );
void initGPS( void );
#endif
