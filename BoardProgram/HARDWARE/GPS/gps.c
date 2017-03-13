#include <stdio.h>
#include <stm32f4xx_usart.h>
#include "gps.h"
#include "delay.h"
#include "led.h"
#include "sys.h"
#include "bin_writer.h"
#include "tick.h"
#include "ff.h"
#include "counter.h"
#include "file_cluster.h"
#include "iwdg.h"

// GPS module settings
#define GP_DISABLE 0x00
#define GP_ENABLE  0x01
#define GGA_ENABLE GP_ENABLE
#define GSA_ENABLE GP_DISABLE
#define GSV_ENABLE GP_DISABLE
#define GLL_ENABLE GP_DISABLE
#define RMC_ENABLE GP_ENABLE
#define VTG_ENABLE GP_DISABLE
#define ZDA_ENABLE GP_DISABLE
#define GPS_DEVICE_BAUD_RATE 38400

#define SRAM_ONLY 0x00
#define SRAM_AND_FLASH 0x01
#define ATTRIBUTES SRAM_ONLY

#define BUFFER_SIZE 0x80

volatile unsigned int lastUpdateTick = 0;

// Functions defined here
void sendChar( unsigned char data);
void sendString( unsigned char *Array, unsigned int size);
void GPS_ProcessByteData(unsigned char byte_in);

#define DMA_LINKED_LIST_ROUND_SIZE 5
volatile linkedDMANode *nowWriteDMA;
volatile linkedDMANode *nowProcessedDMA;
linkedDMANode dmaBuffer[DMA_LINKED_LIST_ROUND_SIZE];

/**
 * @author: Yuan Yifan
 * @function: Initialize DMA buffer saving circular linked list 
 */
void initDMABuffer(){
    unsigned short i;
    nowWriteDMA = dmaBuffer;
    nowProcessedDMA = dmaBuffer;
    for (i = 0; i<DMA_LINKED_LIST_ROUND_SIZE; ++i) { 
        if (i==(DMA_LINKED_LIST_ROUND_SIZE-1)){
            (dmaBuffer+i)->next = (dmaBuffer+0);
        }else{
            (dmaBuffer+i)->next = (dmaBuffer+i+1);
        }
    }
}

/**
 * @author: Yuan Yifan
 * @function: Init DMA: USART2_RX-->RAM
 */
DMA_InitTypeDef  DMA_InitStructure;
void DMA_UART2_Init() {
    NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);//DMA1时钟使能 
	DMA_DeInit(DMA1_Stream5);
	while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE){}//等待DMA可配置 
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)nowWriteDMA->buffer;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = DMA_USART_BUFFER_SIZE;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);//初始化DMA Stream
        
    DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);          //开启DMA数据发送完成中断
    DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);      //清标志
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
    DMA_Cmd(DMA1_Stream5,ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @author: Yuan Yifan
 * @function: DMA1 Stream5 Interrupt function
 */
void DMA1_Stream5_IRQHandler(void){
    if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5)) { // Has buffer is fully filled
        DMA_Cmd(DMA1_Stream5, DISABLE); // Disable DMA1 Stream5
        DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE); // Enable interrupt
        DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5); // Clear "Fully Filled" Flag
        // Move to next buffer
        nowWriteDMA = nowWriteDMA->next; 
        DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(nowWriteDMA->buffer);
        DMA_Init(DMA1_Stream5, &DMA_InitStructure); // re-init DMA1 Stream5
        DMA_Cmd(DMA1_Stream5, ENABLE); // Enable DMA1 Stream5
     }
}

void initUART2( unsigned int baud_rate ) {
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    
    // 启用相应的外设
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // GPIOA2,3复用为USART2 TX,RX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    
    // USART2的GPIO配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    // USART2初始化设置
	USART_InitStructure.USART_BaudRate = baud_rate;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART2, &USART_InitStructure); //Init usart 2
    USART_Cmd(USART2, ENABLE);  //使能串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断
}


void sendChar( unsigned char ch) {
    while((USART2->SR&0X40)==0);
    USART2->DR = ch;   
}

void sendString( unsigned char *Array, unsigned int size) {
    unsigned int i;
    for (i = 0; i<size; i++) {
        sendChar( Array[i] );
    }
}


#define MAX_WAIT_ITER 10000
int sendGPRMCOnly(){
    unsigned char code[16] = {
        0xA0,0xA1,0x00,0x09,
        0x08,0x00,0x00,0x00,
        0x00,0x01,0x00,0x00,
        0x00,0x09,0x0D,0x0A
    };
    unsigned short i = 0;
    unsigned short j = 0;
    for(i = 0; i<16; ++i){
        for (j = 9; j<MAX_WAIT_ITER; ++j){
            delay_us(10);//延时
            if ((USART2->SR&0X40)!=0) { //送信终了
                break;
            }
        }
        if (j==MAX_WAIT_ITER) {
            return 1;//送信错误
        }else{
            USART2->DR = code[i];//送信寄存器
        }
    }
    return 0;
}



volatile linkedGPSNode * nowWriteGPRMC;
volatile linkedGPSNode * nowProcessedGPRMC;
linkedGPSNode GPRMCBuffer[GPRMC_BUFFER_SIZE];

/**
 * @author: Yuan Yifan
 * @function: Initialize GPRMC info buffer saving circular linked list 
 */
void initGPRMCBuffer(){
    unsigned short i;
    nowWriteGPRMC = GPRMCBuffer;
    nowProcessedGPRMC = GPRMCBuffer;
    for (i = 0; i<GPRMC_BUFFER_SIZE; ++i) { 
        if (i==(GPRMC_BUFFER_SIZE-1)){
            (GPRMCBuffer+i)->next = (GPRMCBuffer+0);
        }else{
            (GPRMCBuffer+i)->next = (GPRMCBuffer+i+1);
        }
    }
}

volatile unsigned int message_tick = 0x0000;
unsigned char byte_buffer[BUFFER_SIZE+4];
volatile unsigned int uart2_buffer_pointer = 0;

void GPS_ProcessByteData(unsigned char byte_in) {
    //printf("%c", byte_in);
    if (byte_in == '$') {
        uart2_buffer_pointer = 0;
        message_tick = getTick();
    }
    
    if (uart2_buffer_pointer >= BUFFER_SIZE){
        //printf("!");
        uart2_buffer_pointer--;
    }
    
    byte_buffer[uart2_buffer_pointer] = byte_in;
    uart2_buffer_pointer++;
    
    if (byte_in == '\n' && byte_buffer[0]=='$') {
        setGPSFlag();//拿去喂狗
        byte_buffer[uart2_buffer_pointer] = 0;
        if (isGNRMC(byte_buffer)!=0) {
            unsigned char transResult = process_gps_buffer(
                byte_buffer, 
                uart2_buffer_pointer, 
                (RMCInfo *)&(nowWriteGPRMC->Data),
                message_tick);
            nowWriteGPRMC->validFlag = transResult;
            if( INVALID_RMCInfo != transResult){
                nowWriteGPRMC = nowWriteGPRMC->next;
            }
        }
        uart2_buffer_pointer = 0;
    }else if(byte_in == '\n'){
        uart2_buffer_pointer = 0;
    }
}

void processDMABuffer( void ){
    while(nowWriteDMA!=nowProcessedDMA){
        unsigned short i = 0;
        for (i = 0;i<DMA_USART_BUFFER_SIZE;i++){
            GPS_ProcessByteData(nowProcessedDMA->buffer[i]);
        }
        nowProcessedDMA = nowProcessedDMA->next;
    }
}

void initGPS( void ){
    unsigned char sendRMCOnlyCode[16] = {
        0xA0,0xA1,0x00,0x09,
        0x08,0x00,0x00,0x00,
        0x00,0x01,0x00,0x00,
        0x00,0x09,0x0D,0x0A
    };
    initUART2(GPS_DEVICE_BAUD_RATE);
    DMA_UART2_Init();
    initDMABuffer();
    initGPRMCBuffer();
    onD2();
    delay_ms(250);
    sendString( sendRMCOnlyCode, 16 );
    offD2();
}

