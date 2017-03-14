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

/**
 * @deprecated
 */
#define UNLOCK 0x00

/**
 * @deprecated
 */
volatile unsigned char sync_lock = UNLOCK;

/**
 * @deprecated
 */
unsigned char getSynchronize() {
    return sync_lock;
}

/**
 * @deprecated
 */
#define WAIT_MS 50
void waitSynchronize() {
    while(sync_lock!=UNLOCK){
        delay_ms(WAIT_MS);
    }
}

/**
 * @deprecated
 */
void lockFileSystem(unsigned char LockerID) {
    waitSynchronize();
    sync_lock = LockerID;
}

/**
 * @function Unlock file-system
 */
void unlockFileSystem(unsigned char LockerID) {
    if (sync_lock==LockerID){
        sync_lock = UNLOCK;
    }
}

/**
 * @function write byte buffer to file
 */
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

/**
 * @function Write a delimiter(0x7E) to file.
 */
void writeDelimiter( unsigned int fpIndex ){
    f_putc(DELIMITER, fp_cluster + fpIndex);
}

/**
 * @function Flush file buffers to SD card.
 */
void syncFiles( void ) {
    unsigned int fpIndex;
    for (fpIndex = 0; fpIndex<4; fpIndex++){
        f_sync(fp_cluster + fpIndex);
    }
}

/**
 * @function get GPS binary file pointer
 */
FIL* get_fp_gps_bin( void ) {
    return fp_gps_bin;
}

/**
 * @function get acceletration sensor binary file pointer
 */
FIL* get_fp_mpu_bin( void ) {
    return fp_mpu_bin;
}

/**
 * @function get real-time-clock log csv file pointer
 */
FIL* get_fp_rtc_csv( void ) {
    return fp_rtc_csv;
}

/**
 * @function get CAN bus binary file pointer
 */
FIL* get_fp_can_bin( void ) {
    return fp_can_bin;
}

/**
 * @function judge this module is initialized
 */
unsigned char isInited( void ) {
    return initialized;
}

/**
 * @function mount TF-Card to 0:
 */
void mountSDCard( void ) {
    unsigned char res = FR_OK;
    res = f_mount(&tfCardFs, "0:", 1);
	while (res != FR_OK) {
        delay_ms(200);
        printf("Failed while mounting TF-Card, retrying...\r\n");
        res = f_mount(&tfCardFs, "0:", 1);
    }
    printf("Mount TF-Card successfully.\r\n");
}


/**
 * @function create/open files and set file pointer
 */
void initFileCluster( void ) {
    unsigned char res = 0;
    char fn[255] = {0};

    // Initialize rtc.log.csv and write as append mode
    while(1){
        res = f_open( fp_rtc_csv, "rtc.log.csv", FA_OPEN_ALWAYS | FA_WRITE);
        if (res != FR_OK) {
            printf("Error while opening rtc.log.csv, error code: %d, retrying...\r\n", res);
            delay_ms(1000);
        }else{
            break;
        }
    }
    f_lseek(fp_rtc_csv, f_size(fp_rtc_csv));
    
    sprintf(fn, "%08x.gps.bin", getBootCount());
    while(1){
        res = f_open(fp_gps_bin , fn, FA_CREATE_ALWAYS | FA_WRITE);
        if (res != FR_OK) {
            printf("Error while create gps binary file %s, error code: %d, retrying...\r\n", fn, res);
            delay_ms(1000);
        }else{
            break;
        }
    }
    
    sprintf(fn, "%08x.mpu.bin", getBootCount());
    while(1){
        res = f_open(fp_mpu_bin , fn, FA_CREATE_ALWAYS | FA_WRITE);
        if (res != FR_OK) {
            printf("Error while create gps binary file %s, error code: %d, retrying...\r\n", fn, res);
            delay_ms(1000);
        }else{
            
            break;
        }
    }
    
    sprintf(fn, "%08x.can.bin", getBootCount());
    while(1){
        res = f_open(fp_can_bin , fn, FA_CREATE_ALWAYS | FA_WRITE);
        if (res != FR_OK) {
            printf("Error while create gps binary file %s, error code: %d, retrying...\r\n", fn, res);
            delay_ms(1000);
        }else{

            break;
        }
    }
    
    initialized = 0xff;
    
}

