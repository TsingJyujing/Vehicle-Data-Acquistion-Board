#ifndef __NMEA_0183_HEADFILE__
#define __NMEA_0183_HEADFILE__

#define VALID_RMCInfo 0x01
#define INVALID_RMCInfo 0x00
#define VALID_TIME_ONLY 0x02
typedef struct {
	unsigned int tick;
	float lng;
	float lat;
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned int ms;
} RMCInfo;
unsigned char process_gps_buffer(
	unsigned char *buffer_input,
	unsigned int buffer_size,
	RMCInfo *rtn_data,
	unsigned int tick
);
unsigned char isGNRMC(unsigned char *buffer);
void printRMCInfo(RMCInfo data);

void strrepc(
	unsigned char *buffer_input,
	unsigned int buffer_size,
	unsigned char old_char,
	unsigned char new_char
	);
#endif
