#include "main.h"

/**
* @author yuanyifan
* @Mail Address  yuanyifan@deewinfl.com
* @Telephone 185-0290-5886
* @Company Shaanxi Skywin Internet of Vehicles Information Technology Co.,Ltd
*/

#define DISPLAY_CAN_INFO 0
#define DISPLAY_GPS_INFO 0
#define DISPLAY_MPU_INFO 1
#define MIN_UPDATE_TIME 60000
#define MIN_SAVING_TIME 10000
#define SAVE_FILE 1
#define TRACE_DATA_PROCESS 1
#define USING_WATCHING_DOG 0

void initAll(void);

int main(void) {
	unsigned int bootCount;
    unsigned int valid_length = 0;
    unsigned char serializeBuffer[255];
    unsigned int lastUpdateRTC = 0;
    unsigned int lastSavingTick = 0;
    unsigned char gotGPS , gotCAN, gotMPU;
	initAll();
	bootCount = getBootCount();
	printf("Boot count: %d\r\n", bootCount);
	while (1) {
        setMPUFlag();
        gotGPS = 0x00; gotCAN = 0x00; gotMPU = 0x00;
        processDMABuffer();
        while (nowWriteGPRMC!=nowProcessedGPRMC){
            gotGPS = 0x01;
            #if DISPLAY_GPS_INFO
            printf("Tick:%f\t",((double)getTick())/1000.0);
            printRMCInfo(nowProcessedGPRMC->Data);
            #endif
            if (nowProcessedGPRMC->validFlag==VALID_RMCInfo){
                gps_info g;
                g.lng = nowProcessedGPRMC->Data.lng;
                g.lat = nowProcessedGPRMC->Data.lat;
                g.tick = nowProcessedGPRMC->gotTick;
                gps_info_encoder(g, serializeBuffer, &valid_length);
                #if SAVE_FILE
                writeDelimiter(fpIndexGPS);
                writeFile(fpIndexGPS, serializeBuffer, valid_length);
                writeDelimiter(fpIndexGPS);
                #endif
            }
            if (lastUpdateRTC==0 || (getTick()-lastUpdateRTC)>MIN_UPDATE_TIME) {
                sprintf((char *)serializeBuffer, "%d,%d,%f,%f,20%02d-%02d-%02d %02d:%02d:%02d.%03d\r\n", 
                    bootCount, nowProcessedGPRMC->Data.tick,
                    nowProcessedGPRMC->Data.lng, nowProcessedGPRMC->Data.lat,
                    nowProcessedGPRMC->Data.year, nowProcessedGPRMC->Data.month, nowProcessedGPRMC->Data.day,
                    nowProcessedGPRMC->Data.hour, nowProcessedGPRMC->Data.minute, nowProcessedGPRMC->Data.second, nowProcessedGPRMC->Data.ms
                );
                #if SAVE_FILE
                writeFile(fpIndexRTC, serializeBuffer, strlen((char *)serializeBuffer));
                #endif
                lastUpdateRTC = getTick();
            }
            nowProcessedGPRMC = nowProcessedGPRMC->next;
        }
        
        while (nowWriteCAN!=nowProcessedCAN) {
            can_info c;
            gotCAN = 0x01;
            // Process CAN info here
            //**************************
            #if DISPLAY_CAN_INFO
            printf("Tick:%f\t",((double)nowProcessedCAN->gotTick)/1000.0);
            
            printCAN( (CanRxMsg *)&((nowProcessedCAN->Data)) );
            #endif
            //**************************
            //×ªÂ¼CANÊý¾Ý
            c.std_id = nowProcessedCAN->Data.StdId;
            c.ext_id = nowProcessedCAN->Data.ExtId;
            c.dlc = nowProcessedCAN->Data.DLC;
            c.fmi = nowProcessedCAN->Data.FMI;
            c.ide = nowProcessedCAN->Data.IDE;
            c.rtr = nowProcessedCAN->Data.RTR;
            c.tick = nowProcessedCAN->gotTick;
            
            memcpy(c.data, (unsigned char *)(nowProcessedCAN->Data.Data), 8);
            can_info_encoder(c, serializeBuffer, &valid_length);
            #if SAVE_FILE
            writeDelimiter(fpIndexCAN);
            writeFile(fpIndexCAN, serializeBuffer, valid_length);
            writeDelimiter(fpIndexCAN);
            #endif
            nowProcessedCAN = nowProcessedCAN->next;
        }
        
        while (nowWriteMPU!=nowProcessedMPU) {
            // Process MPU info here
            //**************************
            posture_info P = MPUBuffer2Posture((unsigned char *)(nowProcessedMPU->Data));
            gotMPU = 0x01;
            P.tick = nowProcessedMPU->gotTick;
            quaternionsUpdate(P.gx, P.gy, P.gz, P.ax, P.ay, P.az);
            getAngles(&(P.pitch), &(P.roll), &(P.yaw));
            #if DISPLAY_MPU_INFO
            printf("%f\t", ((double)P.tick)/1000.0);
            printf("%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\r\n",
                P.gx, P.gy, P.gz, P.ax, P.ay, P.az,
                P.pitch, P.roll, P.yaw, P.temperature);
            #endif
            posture_info_encoder(P, serializeBuffer, &valid_length);
            #if SAVE_FILE
            writeDelimiter(fpIndexMPU);
            writeFile(fpIndexMPU, serializeBuffer, valid_length);
            writeDelimiter(fpIndexMPU);
            #endif
            nowProcessedMPU = nowProcessedMPU->next;
        }
        
        #if TRACE_DATA_PROCESS
        if (gotGPS) {
            printf("P");
        }
        if (gotCAN) {
            printf("C");
        }
        if (gotMPU) {
            printf("M");
        }
        #endif
        
        #if SAVE_FILE
        if ((getTick() - lastSavingTick)>MIN_SAVING_TIME) {
            printf("S");
            syncFiles();
            lastSavingTick = getTick();
            printf("\r\n%d\r\n", lastSavingTick);
        }
        #endif
        
        
    }
}


void initAll(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init(SYSTEM_CLOCK_MHZ);
    initUART(DEBUG_UART1_BAUD_RATE);
    printf("Data Acquistion Board is booting...\r\n");
    W25QXX_Init();
    initBootCount();
    initLED();
    
    mountSDCard(); printf("SD card mounted successfully.\r\nCreating BIN files.\r\n");
    initFileCluster(); printf("File cluster initialized successfully.\r\n");
    
    //Init MPU
    while(MPU_Init()!=0){
        printf("Failed to load MPU-6050, retrying...\r\n");
        delay_ms(200);
    }
    
    initDataGetTimer(SYSTEM_CLOCK_MHZ); printf("Load MPU-6050 successfully.\r\n");
    initGPS(); printf("GPS (@USART2) initialized.\r\n");
    CAN1_Mode_Init(CAN_SJW_1tq, CAN_BS2_6tq, CAN_BS1_7tq, 12 ,CAN_Mode_Normal); printf("CAN BUS initialized.\r\n");
    
    #if USING_WATCHING_DOG
    IWDG_Init(7,2000);printf("Independent watching dog initialized.\r\n");
    #endif
	printf("All devices initialized.\r\n");
    
    initTick(SYSTEM_CLOCK_MHZ);
    printf("Software flag: 2017-3-10 13:30:36\r\n");
}
