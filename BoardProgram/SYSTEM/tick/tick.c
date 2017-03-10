#include "tick.h"
#include "sys.h"

volatile unsigned int tick;

void initTick( unsigned int frequencyMHZ ) {
    
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    // Init variable
    tick = 0;
    
    // Init Timer3
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    
    TIM_TimeBaseStructure.TIM_Period = frequencyMHZ - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 499;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
    
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_Cmd(TIM3,ENABLE);
    
}

unsigned int getTick( void ) {
    return tick;
}

void TIM3_IRQHandler(void){
    if(TIM_GetITStatus(TIM3, TIM_IT_Update)==SET) {
        tick++;
    }
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}
