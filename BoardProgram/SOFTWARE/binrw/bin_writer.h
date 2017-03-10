// THIS CODE IS GENERATED FROM PYTHON GENERATOR, DO NOT FIX THIS FILE.

#ifndef _BIN_WRITER_HEAD_
#define _BIN_WRITER_HEAD_
#define DELIMITER 0x7E
#define DELIMITER_APPENDER 0x02
#define ESCAPER 0x7D
#define ESCAPER_APPENDER 0x01

#define __MCU_CODE__

#ifndef __MCU_CODE__
#include <stdio.h>
#endif
//Union to decode value to bytes
typedef union {
	float fval;
	unsigned int uival;
	signed int ival;
	unsigned char bytes[4];
} byte_decoder;

typedef struct {
	unsigned int tick;
	float ax;
	float ay;
	float az;
	float gx;
	float gy;
	float gz;
	float pitch;
	float roll;
	float yaw;
	float temperature;
} posture_info;


#define POSTURE_INFO_SIZE 44

extern void posture_info_encoder(posture_info data, unsigned char byte_array[88], unsigned int *valid_length);
extern unsigned char posture_info_decoder(posture_info *data, unsigned char byte_array[88], unsigned int valid_length);
#ifndef __MCU_CODE__
extern void print_posture_info(posture_info st_input);
#endif
#ifndef __MCU_CODE__
extern int convert_posture_info(char *bin_filename, char *csv_filename);
#endif


typedef struct {
	unsigned int tick;
	float lng;
	float lat;
} gps_info;


#define GPS_INFO_SIZE 12

extern void gps_info_encoder(gps_info data, unsigned char byte_array[24], unsigned int *valid_length);
extern unsigned char gps_info_decoder(gps_info *data, unsigned char byte_array[24], unsigned int valid_length);
#ifndef __MCU_CODE__
extern void print_gps_info(gps_info st_input);
#endif
#ifndef __MCU_CODE__
extern int convert_gps_info(char *bin_filename, char *csv_filename);
#endif


typedef struct {
	unsigned int tick;
	unsigned int std_id;
	unsigned int ext_id;
	unsigned char ide;
	unsigned char rtr;
	unsigned char dlc;
	unsigned char data[8];
	unsigned char fmi;
} can_info;


#define CAN_INFO_SIZE 24

extern void can_info_encoder(can_info data, unsigned char byte_array[48], unsigned int *valid_length);
extern unsigned char can_info_decoder(can_info *data, unsigned char byte_array[48], unsigned int valid_length);
#ifndef __MCU_CODE__
extern void print_can_info(can_info st_input);
#endif
#ifndef __MCU_CODE__
extern int convert_can_info(char *bin_filename, char *csv_filename);
#endif


#endif
