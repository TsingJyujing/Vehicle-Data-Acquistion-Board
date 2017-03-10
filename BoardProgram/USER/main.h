#ifndef __MAIN__
#define __MAIN__    
#include <string.h>
#ifdef __cplusplus  
extern "C" {
#endif  
    #include "ff.h"
    #include "sys.h"
    #include "can.h"
    #include "led.h"
    #include "gps.h"
    #include "tick.h"
    #include "iwdg.h"
    #include "delay.h"
    #include "usart.h"
    #include "w25qxx.h"
    #include "exfuns.h"
    #include "mpu6050.h"
    #include "inv_mpu.h"
    #include "counter.h"
    #include "bin_writer.h"
    #include "sdio_sdcard.h"
    #include "mpu_data_acq.h"
    #include "file_cluster.h"
    #include "quaternions.h"
    #include "nmea_0183.h"
    #include <stm32f4xx.h>
#ifdef __cplusplus  
}
#endif
#define SYSTEM_CLOCK_MHZ 168
#define DEBUG_UART1_BAUD_RATE 460800
#define MOTION_RECORD_DELAY_MS 25
#endif
