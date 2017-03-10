#include "nmea_0183.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
// GPS decode settings
#define GNRMC_VALID_OFFSET 18
#define GNRMC_LATITUDE_OFFSET 20
#define GNRMC_NORTH_SOUTH_OFFSET 30
#define GNRMC_LONGITUDE_OFFSET 32
#define GNRMC_EAST_WEST_OFFSET 43
#define GNRMC_TIME_OFFSET 7
#define GNRMC_DATE_OFFSET 57
#define VALID_TAG 'A'


unsigned char GNRMCFormatModule[72] = "$GNRMC,dddddd.ddd,c,dddd.dddd,c,ddddd.dddd,c,ddd.d,ddd.d,dddddd,,,****";

unsigned char notNumberical(unsigned char c) {
    return c>='0' && c<='9' ? 0x00:0xFF;
}

unsigned char notChar(unsigned char c) {
    return ((c>='a' && c<='z')||(c>='A' && c<='Z')) ? 0x00:0xFF;
}

unsigned char isGNRMC(unsigned char *buffer) {
    unsigned short i = 0;
    for (i = 0; i<70 ;++i) {
        if (GNRMCFormatModule[i]=='*'){
            //pass
        }else if(GNRMCFormatModule[i]=='d'){
            if (notNumberical(buffer[i])) return 0;
        }else if(GNRMCFormatModule[i]=='c'){
            if (notChar(buffer[i])) return 0;
        }else{
            if (GNRMCFormatModule[i]!=buffer[i]) return 0;
        }
    }
    return 0xff;
}

void strrepc(
	unsigned char *buffer_input,
	unsigned int buffer_size,
	unsigned char old_char,
	unsigned char new_char
	){
	unsigned int i = 0;
	for (i = 0; i < buffer_size; i++){
		if (old_char == buffer_input[i]){
			buffer_input[i] = new_char;
		}
	}
}


unsigned char process_gps_buffer(
	unsigned char *buffer_input,
	unsigned int buffer_size,
	RMCInfo *rtn_data,
	unsigned int tick
) {

	//Decode Info
	unsigned char dateYear = 0;
	unsigned char dateMonth = 0;
	unsigned char dateDay = 0;
	unsigned char timeHour = 0;
	unsigned char timeMin = 0;
	unsigned char timeSec = 0;
	unsigned int timeMilsec = 0;

	float gpsLongitude = 0.0;
	float gpsLatitude = 0.0;

	//Used variables
	int is_rmc_info = strncmp((char *)buffer_input, "$GNRMC", 6);
	float srcTime = 0.0;
	unsigned int srcDate = 0;

	if (is_rmc_info == 0){
		//Process RMC Info
		strrepc(buffer_input, buffer_size, ',', '\0');
        gpsLongitude = atof((char *)buffer_input + GNRMC_LONGITUDE_OFFSET);
		if (buffer_input[GNRMC_EAST_WEST_OFFSET] == 'W'){
			gpsLongitude = -gpsLongitude;
		}
        
        gpsLatitude = atof((char *)buffer_input + GNRMC_LATITUDE_OFFSET);
		if (buffer_input[GNRMC_NORTH_SOUTH_OFFSET] == 'S'){
			gpsLatitude = -gpsLatitude;
		}
		// Write result here
        srcTime = atof((char *)buffer_input + GNRMC_TIME_OFFSET);
        srcDate = atoi((char *)buffer_input + GNRMC_DATE_OFFSET);
		timeHour = (unsigned char)floor(srcTime / 10000.0f);
		timeMin = (unsigned char)floor((srcTime - timeHour * 10000) / 100);
		timeSec = (unsigned char)floor(srcTime - timeHour * 10000.0 - timeMin*100.0);
		timeMilsec = (unsigned int)((srcTime - floor(srcTime)) * 1000);

		dateDay = (unsigned char)floor(srcDate / 10000);
		dateMonth = (unsigned char)floor((srcDate - dateDay * 10000) / 100);
		dateYear = (unsigned char)floor(srcDate - dateDay * 10000 - dateMonth * 100);

		rtn_data->tick = tick;
		rtn_data->lng = gpsLongitude / 100.0f;
		rtn_data->lat = gpsLatitude /100.0f;
		rtn_data->year = dateYear;
		rtn_data->month = dateMonth;
		rtn_data->day = dateDay;
		rtn_data->hour = timeHour;
		rtn_data->minute = timeMin;
		rtn_data->second = timeSec;
		rtn_data->ms = timeMilsec;
        
        // IF NOT VALID -> EXIT PROCESSING
		if (buffer_input[GNRMC_VALID_OFFSET] != VALID_TAG){
            if (dateYear >= 17){
                return VALID_TIME_ONLY;
            }else{
                return INVALID_RMCInfo;
            }
		}else{
            return VALID_RMCInfo;
        }
	}else{
        rtn_data->year = 0x00;
        return INVALID_RMCInfo;
    }
}


void printRMCInfo(RMCInfo data){
	printf("Time:%x\t", data.tick);
	printf("Lng:%f\t", data.lng);
	printf("Lat:%f\t", data.lat);
	printf("Time:20%02d-%02d-%02d %02d:%02d:%02d.%03d\r\n",
		data.year, data.month, data.day,
		data.hour, data.minute, data.second, data.ms);
}
