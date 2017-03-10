#include <stdio.h>
#include <string.h>
#include "bin_writer.h"
#define TYPE_MATCHED 0x00
// #define DEBUG_MODE
#ifdef DEBUG_MODE
void Test(){
	convert_posture_info("0000042a.mpu.bin", "csv/0000042a.mpu.csv");
	convert_can_info("0000042a.can.bin", "csv/0000042a.can.csv");
	convert_gps_info("0000042a.gps.bin", "csv/0000042a.gps.csv");
}
#endif
int main(int argc, char **argv) {
	/*
	argv define:
	selfname, convert type code, input filename, output filename 
	*/
#ifdef DEBUG_MODE
	Test();
#endif
	if (argc < 4) {
		printf("Usage: bin2csv [type] [input-filename] [output-filename]\n");
		printf("\t[type]: can,gps or mpu\n");
		printf("\t[input-filename]: bin type file\n");
		printf("\t[output-filename]: csv type file\n");
	}
	else {
		printf("%s-(%s)->%s converting...\n", argv[2], argv[1], argv[3]);
		if (strncmp(argv[1], "can", 3) == TYPE_MATCHED) {
			convert_can_info(argv[2], argv[3]);
		}
		else if (strncmp(argv[1], "gps", 3) == TYPE_MATCHED) {
			convert_gps_info(argv[2], argv[3]);
		}
		else if (strncmp(argv[1], "mpu", 3) == TYPE_MATCHED) {
			convert_posture_info(argv[2], argv[3]);
		}
		else{
			printf("No type fetched in your [type]\n");
		}
	}
	return 0x00;
}