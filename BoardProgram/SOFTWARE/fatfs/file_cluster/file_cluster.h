#ifndef __FILE_CLUSTER_HEAD__
#define __FILE_CLUSTER_HEAD__
#include "ff.h"
#include "stdio.h"
#include "counter.h"
#include "delay.h"
#define fpIndexGPS 0
#define fpIndexMPU 1
#define fpIndexRTC 2
#define fpIndexCAN 3
FIL* get_fp_gps_bin( void );
FIL* get_fp_mpu_bin( void );
FIL* get_fp_rtc_csv( void );
FIL* get_fp_can_bin( void );
unsigned char writeFile(unsigned int fpIndex, const void *buffer, unsigned int len);
void initFileCluster( void );
unsigned char isInited( void );
void syncFiles( void );
void writeDelimiter( unsigned int fpIndex );
void mountSDCard( void );
#endif

