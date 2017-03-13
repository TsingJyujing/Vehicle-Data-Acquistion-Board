#include "mpu_data_acq.h"
#include "stdio.h"
#include "sys.h"
#include "bin_writer.h"
#include "led.h"
#include "inv_mpu.h"
#include "iwdg.h"
#include "tick.h"


volatile unsigned short error_sleeping = 0;

// 单向环形链表缓冲区
#define MPU_LINKED_LIST_ROUND_SIZE MPU_6050_SAMPLE_FREQUENCY*2
linkedMPUNode *nowWriteMPU;
linkedMPUNode *nowProcessedMPU;
linkedMPUNode lpbuffer[MPU_LINKED_LIST_ROUND_SIZE];

/**
  * @author  Yuan Yifan
  * @function Initialize MPU-6050 posture linked list buffer
  */
void initMPUCircularLinkedListBuffer() {
    unsigned short i = 0;
    nowWriteMPU = lpbuffer;
    nowProcessedMPU = nowWriteMPU;
    for (i = 0; i<MPU_LINKED_LIST_ROUND_SIZE; ++i) { 
        if (i==(MPU_LINKED_LIST_ROUND_SIZE-1)){
            (lpbuffer+i)->next = (lpbuffer+0);
        }else{
            (lpbuffer+i)->next = (lpbuffer+i+1);
        }
    }
}

/**
  * @author  Yuan Yifan
  * @function Initialize buffer and Timer4 settings
  * @param frequencyMHZ System clock frequency(commonly 168MHz)
  */
void initDataGetTimer( unsigned int frequencyMHZ ) {
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    initMPUCircularLinkedListBuffer();
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
    TIM_TimeBaseStructure.TIM_Period = frequencyMHZ - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 500*SAMPLE_MILLSECOND_TIME - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_Cmd(TIM4,ENABLE);
    
}

/**
  * @author  Yuan Yifan
  * @function Timer 4 interrupt handler
  */
void TIM4_IRQHandler(void){
    if(TIM_GetITStatus(TIM4, TIM_IT_Update)==SET) { //Is time up?
        if (error_sleeping>1){ // Error found, sleep and don't do anything
            error_sleeping--;
        }else if(error_sleeping==1){ // Error found, sleep time up and try to re-init MPU 6050
            if (MPU_Init()) { // Init failed
                error_sleeping = 200;
                printf("MPU 6050 reinit failed\r\n"); // Debug version
            }else{ // Init sucessfully
                error_sleeping--;
            }
        }else{ // No error
            unsigned char err = 0xff;
            onD3();
            nowWriteMPU->gotTick = getTick();// get system tick
            err = MPU_Read_Len(0x68 , 0X3B, 14, (unsigned char *)(nowWriteMPU->Data));// Read MPU data
            offD3();
            if (!err) {
                nowWriteMPU = nowWriteMPU->next; // Move the buffer pointer
                setMPUFlag(); // Feed the dog
            }else{
                error_sleeping = 200; // A error found while reading IIC
            }
        }
        
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);// Clear it
    }
}

/**
  * @author  Yuan Yifan
  * @function decode buffer and calculate posture by data
  */
posture_info MPUBuffer2Posture(unsigned char buffer[14]) {
    posture_info p;
    p.ax=(double)((short)(((unsigned short)buffer[0]<<8)|buffer[1]))*ACCE_GAIN;  
	p.ay=(double)((short)(((unsigned short)buffer[2]<<8)|buffer[3]))*ACCE_GAIN;  
	p.az=(double)((short)(((unsigned short)buffer[4]<<8)|buffer[5]))*ACCE_GAIN;  
    p.temperature = 36.53f+((float)(((u16)buffer[0+6]<<8)|buffer[1+6]))/340.0f;  
    p.gx=(double)((short)(((unsigned short)buffer[0+8]<<8)|buffer[1+8]))*GYRO_GAIN;  
    p.gy=(double)((short)(((unsigned short)buffer[2+8]<<8)|buffer[3+8]))*GYRO_GAIN;  
    p.gz=(double)((short)(((unsigned short)buffer[4+8]<<8)|buffer[5+8]))*GYRO_GAIN;  
    return p;
}


