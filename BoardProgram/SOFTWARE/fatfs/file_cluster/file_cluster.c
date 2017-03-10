#include "file_cluster.h"
#include "bin_writer.h"
FIL fp_cluster[4];

#define fp_gps_bin (fp_cluster+0)
#define fp_mpu_bin (fp_cluster+1)
#define fp_rtc_csv (fp_cluster+2)
#define fp_can_bin (fp_cluster+3)
#define SYNC_MAX_SIZE 4*1024
volatile unsigned int syncBufferSize[4] = {0};
FATFS tfCardFs;

volatile unsigned char initialized = 0x00;
#define UNLOCK 0x00
volatile unsigned char sync_lock = UNLOCK;

unsigned char getSynchronize() {
    return sync_lock;
}

#define WAIT_MS 50
void waitSynchronize() {
    while(sync_lock!=UNLOCK){
        delay_ms(WAIT_MS);
    }
}

void lockFileSystem(unsigned char LockerID) {
    waitSynchronize();
    sync_lock = LockerID;
}

void unlockFileSystem(unsigned char LockerID) {
    if (sync_lock==LockerID){
        sync_lock = UNLOCK;
    }
}

unsigned char writeFile(unsigned int fpIndex, const void *buffer, unsigned int len) {
    unsigned int result = FR_OK;
    f_write( fp_cluster + fpIndex, buffer, len, &result);
    syncBufferSize[fpIndex]+=(len+2);
    if (result==len){
        return 0x00;
    }else{
        return 0xff;
    }
}

void writeDelimiter( unsigned int fpIndex ){
    f_putc(DELIMITER, fp_cluster + fpIndex);
}

void syncFiles( void ) {
    unsigned int fpIndex;
    for (fpIndex = 0; fpIndex<4; fpIndex++){
        f_sync(fp_cluster + fpIndex);
    }
    // RTC数据少，特殊对待。
    /*
    if (syncBufferSize[fpIndexRTC]>0) {
        f_sync(fp_cluster + fpIndexRTC);
        syncBufferSize[fpIndex] = 0;
    }*/
}

FIL* get_fp_gps_bin( void ) {
    return fp_gps_bin;
}

FIL* get_fp_mpu_bin( void ) {
    return fp_mpu_bin;
}

FIL* get_fp_rtc_csv( void ) {
    return fp_rtc_csv;
}

FIL* get_fp_can_bin( void ) {
    return fp_can_bin;
}

unsigned char isInited( void ) {
    return initialized;
}

void mountSDCard( void ) {
    unsigned char res = FR_OK;
    res = f_mount(&tfCardFs, "0:", 1);
	while (res != FR_OK) {
        delay_ms(500);
        printf("Failed while mounting TF-Card, retrying...\r\n");
        res = f_mount(&tfCardFs, "0:", 1);
    }
    printf("Mount TF-Card successfully.\r\n");
}


void initFileCluster( void ) {
    unsigned char res = 0;
    char fn[255] = {0};

    // Initialize rtc.log.csv and write as append mode
    while(1){
        res = f_open( fp_rtc_csv, "rtc.log.csv", FA_OPEN_ALWAYS | FA_WRITE);
        if (res != FR_OK) {
            printf("Error while opening rtc.log.csv, error code: %d, retrying...\r\n", res);
        }else{
            delay_ms(2000);
            break;
        }
    }
    f_lseek(fp_rtc_csv, f_size(fp_rtc_csv));
    
    sprintf(fn, "%08x.gps.bin", getBootCount());
    while(1){
        res = f_open(fp_gps_bin , fn, FA_CREATE_ALWAYS | FA_WRITE);
         if (res != FR_OK) {
            printf("Error while create gps binary file %s, error code: %d, retrying...\r\n", fn, res);
        }else{
            delay_ms(2000);
            break;
        }
    }
    
    sprintf(fn, "%08x.mpu.bin", getBootCount());
    while(1){
        res = f_open(fp_mpu_bin , fn, FA_CREATE_ALWAYS | FA_WRITE);
         if (res != FR_OK) {
            printf("Error while create gps binary file %s, error code: %d, retrying...\r\n", fn, res);
        }else{
            delay_ms(2000);
            break;
        }
    }
    
    sprintf(fn, "%08x.can.bin", getBootCount());
    while(1){
        res = f_open(fp_can_bin , fn, FA_CREATE_ALWAYS | FA_WRITE);
         if (res != FR_OK) {
            printf("Error while create gps binary file %s, error code: %d, retrying...\r\n", fn, res);
        }else{
            delay_ms(2000);
            break;
        }
    }
    
    initialized = 0xff;
    
}

