#ifndef __MPU_DATA_INTERP__
#define __MPU_DATA_INTERP__
#include "mpu6050.h"

typedef struct st_lp_node{
    unsigned char Data[14];
    unsigned int gotTick;
    struct st_lp_node *next;
} linkedMPUNode;

extern linkedMPUNode *nowWriteMPU;
extern linkedMPUNode *nowProcessedMPU;

// Parameters Settings
#define SAMPLE_MILLSECOND_TIME 20
#define SIGNED_MAX_SHORT 32768

#define MAX_ACCE_VALUE 2*9.8
#define MAX_GYRO_VALUE 2000.0

#define ACCE_GAIN MAX_ACCE_VALUE/SIGNED_MAX_SHORT
#define GYRO_GAIN MAX_GYRO_VALUE/SIGNED_MAX_SHORT

posture_info MPUBuffer2Posture(unsigned char buffer[14]);

#endif

