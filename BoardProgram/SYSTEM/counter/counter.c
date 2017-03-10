#include "counter.h"
#include "bin_writer.h"
#include "w25qxx.h"

#define BOOT_COUNTER_ADDRESS 0x00
volatile unsigned int bootCounter;

void initBootCount( void ) {
    byte_decoder bd;
	W25QXX_Read(bd.bytes, BOOT_COUNTER_ADDRESS, 4);
	bootCounter = bd.uival;
    bd.uival += 1;
    W25QXX_Write(bd.bytes, BOOT_COUNTER_ADDRESS, 4);
}


unsigned int getBootCount(void){
	return bootCounter;
}


