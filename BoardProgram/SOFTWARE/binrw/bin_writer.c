// THIS CODE IS GENERATED FROM PYTHON GENERATOR, DO NOT FIX THIS FILE.

#include "bin_writer.h"
#include <malloc.h>
#include <string.h>

//process escape chars
unsigned int escape_decode(unsigned char *byte_in, unsigned int count_in, unsigned char *byte_out, unsigned int max_offset){
	unsigned int i = 0;
	unsigned int offset = 0;
	unsigned int err_code = 0xffffffff;
	for (i = 0; i < count_in; i++){
		if (byte_in[i] == ESCAPER && (i + 1) < count_in) {
			if (byte_in[i + 1] == DELIMITER_APPENDER){
				i++;
				byte_out[offset] = DELIMITER;
			}
			else if (byte_in[i + 1] == ESCAPER_APPENDER){
				i++;
				byte_out[offset] = ESCAPER;
			}
			else{
				//Error if executing here
				byte_out[offset] = ESCAPER;
				err_code = 0;
			}
		}
		else{
			byte_out[offset] = byte_in[i];
		}
		offset++;
		if (offset>max_offset) {// Error
			return 0x00;
		}
	}
	if (err_code == 0){// Error
		return 0x00;
	}
	else{
		return offset;
	}
}

void escape_encode(
	unsigned char *byte_in, unsigned int count_in,
	unsigned char *byte_out, unsigned int *count_out
	){
	unsigned int i;
	unsigned int offset = 0;
	for (i = 0; i < count_in; ++i){
		if (byte_in[i] == DELIMITER){
			byte_out[i + offset] = ESCAPER;
			byte_out[i + offset + 1] = DELIMITER_APPENDER;
			offset++;
		}
		else if (byte_in[i] == ESCAPER){
			byte_out[i + offset] = ESCAPER;
			byte_out[i + offset + 1] = ESCAPER_APPENDER;
			offset++;
		}
		else {
			byte_out[i + offset] = byte_in[i];
		}
	}
	count_out[0] = count_in + offset;
}

unsigned char posture_info_decoder(posture_info *data, unsigned char byte_buffer[88], unsigned int valid_length) {

	unsigned short i = 0;
	unsigned int j = 0;
	unsigned int offset = 0;
	unsigned char byte_array[44] = { 0 };
	byte_decoder decoder;
	offset = escape_decode(byte_buffer, valid_length, byte_array, 44);
	if (offset != 44) return 0x01;
	offset = 0;
	//decoder of tick struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->tick = decoder.uival;


	//decoder of ax struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->ax = decoder.fval;


	//decoder of ay struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->ay = decoder.fval;


	//decoder of az struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->az = decoder.fval;


	//decoder of gx struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->gx = decoder.fval;


	//decoder of gy struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->gy = decoder.fval;


	//decoder of gz struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->gz = decoder.fval;


	//decoder of pitch struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->pitch = decoder.fval;


	//decoder of roll struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->roll = decoder.fval;


	//decoder of yaw struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->yaw = decoder.fval;


	//decoder of temperature struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->temperature = decoder.fval;


	return 0x00;

}


void posture_info_encoder(posture_info data, unsigned char byte_array[88], unsigned int *valid_length) {
	unsigned char raw_info[44] = { 0x00 };

	byte_decoder decoder;
	unsigned short i = 0;
	unsigned int j = 0;
	unsigned int offset = 0;
	unsigned int delimiter_count = 0;
	//encoder of tick struct from byte array
	decoder.uival = data.tick;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of ax struct from byte array
	decoder.fval = data.ax;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of ay struct from byte array
	decoder.fval = data.ay;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of az struct from byte array
	decoder.fval = data.az;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of gx struct from byte array
	decoder.fval = data.gx;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of gy struct from byte array
	decoder.fval = data.gy;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of gz struct from byte array
	decoder.fval = data.gz;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of pitch struct from byte array
	decoder.fval = data.pitch;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of roll struct from byte array
	decoder.fval = data.roll;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of yaw struct from byte array
	decoder.fval = data.yaw;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of temperature struct from byte array
	decoder.fval = data.temperature;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	escape_encode(raw_info, 44, byte_array, valid_length);
}


#ifndef __MCU_CODE__
void print_posture_info(posture_info st_input) {
	unsigned int i = 0;
	printf("posture_info visualiz\n");
	printf("  tick:\t%d\n", st_input.tick);
	printf("  ax:\t%f\n", st_input.ax);
	printf("  ay:\t%f\n", st_input.ay);
	printf("  az:\t%f\n", st_input.az);
	printf("  gx:\t%f\n", st_input.gx);
	printf("  gy:\t%f\n", st_input.gy);
	printf("  gz:\t%f\n", st_input.gz);
	printf("  pitch:\t%f\n", st_input.pitch);
	printf("  roll:\t%f\n", st_input.roll);
	printf("  yaw:\t%f\n", st_input.yaw);
	printf("  temperature:\t%f\n", st_input.temperature);
}
#endif


#ifndef __MCU_CODE__
int convert_posture_info(char *bin_filename, char *csv_filename){

	int readbuf;
	unsigned char buffer[1024] = { 0 };
	unsigned int i = 0;
	FILE *fb = fopen(bin_filename, "rb");
	FILE *fw = fopen(csv_filename, "w");
	if (fb == NULL || fw == NULL){
		printf("Fail to open file: %s or %s\n", bin_filename, csv_filename);
		return -1;
	}
	printf("Open file successfully.\n");
	/*Print csv head code*/
	fprintf(fw, "tick,ax,ay,az,gx,gy,gz,pitch,roll,yaw,temperature\n");
	readbuf = fgetc(fb);
	while (readbuf != EOF){
		if (readbuf == DELIMITER) {
			if (i >= 44 && i <= (44 * 2)) {
				unsigned int j = 0;
				posture_info st_input;
				if (posture_info_decoder(&st_input, buffer, i) == 0x00){
					fprintf(fw, "%d,", st_input.tick);
					fprintf(fw, "%f,", st_input.ax);
					fprintf(fw, "%f,", st_input.ay);
					fprintf(fw, "%f,", st_input.az);
					fprintf(fw, "%f,", st_input.gx);
					fprintf(fw, "%f,", st_input.gy);
					fprintf(fw, "%f,", st_input.gz);
					fprintf(fw, "%f,", st_input.pitch);
					fprintf(fw, "%f,", st_input.roll);
					fprintf(fw, "%f,", st_input.yaw);
					fprintf(fw, "%f,", st_input.temperature);

					/*Print csv code*/
					fprintf(fw, "\n");
				}

			}
			i = 0;
		}
		else {
			buffer[i] = (unsigned char)readbuf;
			if (i<1023) i++;
		}
		readbuf = fgetc(fb);
	}
	fclose(fb);
	fb = NULL;
	fclose(fw);
	fw = NULL;
	return 0;
}
#endif


unsigned char gps_info_decoder(gps_info *data, unsigned char byte_buffer[24], unsigned int valid_length) {

	unsigned short i = 0;
	unsigned int j = 0;
	unsigned int offset = 0;
	unsigned char byte_array[12] = { 0 };
	byte_decoder decoder;
	offset = escape_decode(byte_buffer, valid_length, byte_array, 12);
	if (offset != 12) return 0x01;
	offset = 0;
	//decoder of tick struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->tick = decoder.uival;


	//decoder of lng struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->lng = decoder.fval;


	//decoder of lat struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->lat = decoder.fval;


	return 0x00;

}


void gps_info_encoder(gps_info data, unsigned char byte_array[24], unsigned int *valid_length) {
	unsigned char raw_info[12] = { 0x00 };

	byte_decoder decoder;
	unsigned short i = 0;
	unsigned int j = 0;
	unsigned int offset = 0;
	unsigned int delimiter_count = 0;
	//encoder of tick struct from byte array
	decoder.uival = data.tick;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of lng struct from byte array
	decoder.fval = data.lng;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of lat struct from byte array
	decoder.fval = data.lat;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	escape_encode(raw_info, 12, byte_array, valid_length);
}


#ifndef __MCU_CODE__
void print_gps_info(gps_info st_input) {
	unsigned int i = 0;
	printf("gps_info visualiz\n");
	printf("  tick:\t%d\n", st_input.tick);
	printf("  lng:\t%f\n", st_input.lng);
	printf("  lat:\t%f\n", st_input.lat);
}
#endif


#ifndef __MCU_CODE__
int convert_gps_info(char *bin_filename, char *csv_filename){

	int readbuf;
	unsigned char buffer[1024] = { 0 };
	unsigned int i = 0;
	FILE *fb = fopen(bin_filename, "rb");
	FILE *fw = fopen(csv_filename, "w");
	if (fb == NULL || fw == NULL){
		printf("Fail to open file: %s or %s\n", bin_filename, csv_filename);
		return -1;
	}
	printf("Open file successfully.\n");
	/*Print csv head code*/
	fprintf(fw, "tick,lng,lat\n");
	readbuf = fgetc(fb);
	while (readbuf != EOF){
		if (readbuf == DELIMITER) {
			if (i >= 12 && i <= (12 * 2)) {
				unsigned int j = 0;
				gps_info st_input;
				if (gps_info_decoder(&st_input, buffer, i) == 0x00){
					fprintf(fw, "%d,", st_input.tick);
					fprintf(fw, "%f,", st_input.lng);
					fprintf(fw, "%f,", st_input.lat);

					/*Print csv code*/
					fprintf(fw, "\n");
				}

			}
			i = 0;
		}
		else {
			buffer[i] = (unsigned char)readbuf;
			if (i<1023) i++;
		}
		readbuf = fgetc(fb);
	}
	fclose(fb);
	fb = NULL;
	fclose(fw);
	fw = NULL;
	return 0;
}
#endif


unsigned char can_info_decoder(can_info *data, unsigned char byte_buffer[48], unsigned int valid_length) {

	unsigned short i = 0;
	unsigned int j = 0;
	unsigned int offset = 0;
	unsigned char byte_array[24] = { 0 };
	byte_decoder decoder;
	offset = escape_decode(byte_buffer, valid_length, byte_array, 24);
	if (offset != 24) return 0x01;
	offset = 0;
	//decoder of tick struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->tick = decoder.uival;


	//decoder of std_id struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->std_id = decoder.uival;


	//decoder of ext_id struct from byte array

	for (i = 0; i < 4; ++i){
		decoder.bytes[i] = byte_array[i + offset];
	}
	offset += 4;
	data->ext_id = decoder.uival;


	//decoder of ide struct from byte array
	data->ide = byte_array[offset];
	offset++;

	//decoder of rtr struct from byte array
	data->rtr = byte_array[offset];
	offset++;

	//decoder of dlc struct from byte array
	data->dlc = byte_array[offset];
	offset++;

	//decoder of data struct from byte array

	for (i = 0; i < 8; i++){
		data->data[i] = byte_array[i + offset];
	}
	offset += 8;


	//decoder of fmi struct from byte array
	data->fmi = byte_array[offset];
	offset++;

	return 0x00;

}


void can_info_encoder(can_info data, unsigned char byte_array[48], unsigned int *valid_length) {
	unsigned char raw_info[24] = { 0x00 };

	byte_decoder decoder;
	unsigned short i = 0;
	unsigned int j = 0;
	unsigned int offset = 0;
	unsigned int delimiter_count = 0;
	//encoder of tick struct from byte array
	decoder.uival = data.tick;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of std_id struct from byte array
	decoder.uival = data.std_id;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of ext_id struct from byte array
	decoder.uival = data.ext_id;

	for (i = 0; i < 4; ++i){
		raw_info[i + offset] = decoder.bytes[i];
	}
	offset += 4;

	//encoder of ide struct from byte array
	raw_info[offset] = data.ide;
	offset++;
	//encoder of rtr struct from byte array
	raw_info[offset] = data.rtr;
	offset++;
	//encoder of dlc struct from byte array
	raw_info[offset] = data.dlc;
	offset++;
	//encoder of data struct from byte array

	for (i = 0; i < 8; i++){
		raw_info[i + offset] = data.data[i];
	}
	offset += 8;

	//encoder of fmi struct from byte array
	raw_info[offset] = data.fmi;
	offset++;
	escape_encode(raw_info, 24, byte_array, valid_length);
}


#ifndef __MCU_CODE__
void print_can_info(can_info st_input) {
	unsigned int i = 0;
	printf("can_info visualiz\n");
	printf("  tick:\t%d\n", st_input.tick);
	printf("  std_id:\t%d\n", st_input.std_id);
	printf("  ext_id:\t%d\n", st_input.ext_id);
	printf("  ide:\t%d\n", st_input.ide);
	printf("  rtr:\t%d\n", st_input.rtr);
	printf("  dlc:\t%d\n", st_input.dlc);
	for (i = 0; i<8; ++i) printf("  data[%d]:\t%d\n", i, st_input.data[i]);
	printf("  fmi:\t%d\n", st_input.fmi);
}
#endif


#ifndef __MCU_CODE__
int convert_can_info(char *bin_filename, char *csv_filename){

	int readbuf;
	unsigned char buffer[1024] = { 0 };
	unsigned int i = 0;
	FILE *fb = fopen(bin_filename, "rb");
	FILE *fw = fopen(csv_filename, "w");
	if (fb == NULL || fw == NULL){
		printf("Fail to open file: %s or %s\n", bin_filename, csv_filename);
		return -1;
	}
	printf("Open file successfully.\n");
	/*Print csv head code*/
	fprintf(fw, "tick,std_id,ext_id,ide,rtr,dlc,data,fmi\n");
	readbuf = fgetc(fb);
	while (readbuf != EOF){
		if (readbuf == DELIMITER) {
			if (i >= 24 && i <= (24 * 2)) {
				unsigned int j = 0;
				can_info st_input;
				if (can_info_decoder(&st_input, buffer, i) == 0x00){
					fprintf(fw, "%d,", st_input.tick);
					fprintf(fw, "%d,", st_input.std_id);
					fprintf(fw, "%d,", st_input.ext_id);
					fprintf(fw, "%d,", st_input.ide);
					fprintf(fw, "%d,", st_input.rtr);
					fprintf(fw, "%d,", st_input.dlc);
					for (j = 0; j<8; ++j) fprintf(fw, "%d ", st_input.data[j]);
					fprintf(fw, ",");
					fprintf(fw, "%d,", st_input.fmi);

					/*Print csv code*/
					fprintf(fw, "\n");
				}

			}
			i = 0;
		}
		else {
			buffer[i] = (unsigned char)readbuf;
			if (i<1023) i++;
		}
		readbuf = fgetc(fb);
	}
	fclose(fb);
	fb = NULL;
	fclose(fw);
	fw = NULL;
	return 0;
}
#endif


