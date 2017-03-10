import string

type_dict = {
    "u32":"unsigned int",
    "i32":"signed int",
    "f32":"float",
    "c08":"unsigned char"
}

decode_dict = {
    "u32":"uival",
    "i32":"ival",
    "f32":"fval",
}

def calc_type_bytes(type_define):
    sum_bits = 0
    # Check sumall bit
    for elem in type_define:
        # get type defined bit count
        type_size = string.atoi(elem[1][-2:])
        if elem[2]!=0:
            sum_bits += type_size*elem[2]
        else:
            sum_bits += type_size
            
    return sum_bits / 8
    
    
def generate_structs(type_name, type_define):
    head_info = "typedef struct {\n"
    for elem in type_define:
        head_info += "    " + type_dict[elem[1]] + " " + elem[0]
        if elem[2]!=0:
            head_info += "[%d]" % elem[2]
        head_info += ";\n"
    head_info += "} %s;\n\n" % type_name
    return head_info

    
def generate_encode_field_code(elem):
    head_info = ""
    name = elem[0]
    type = elem[1]
    length = elem[2] 
    if type in ['u32','f32','i32']:
        decode_type = decode_dict[type]
        if length==0:
            head_info += "    decoder.%s = data.%s;\n" % (decode_type, name)
            head_info += """
        for (i = 0; i < 4; ++i){
            raw_info[i + offset] = decoder.bytes[i];
        }
        offset += 4;
        """
        else:
            head_info += "for (j = 0;j<%d;++j){" % length
            head_info += "decoder.%s = data.%s[j];\n" % (decode_type, name)
            head_info += """
        for (i = 0; i < 4; ++i){
            raw_info[i + offset] = decoder.bytes[i];
        }
        offset += 4;
        }\n
            """
    elif type=="c08":
        if length==0:
            # Don't write to raw_info[offset] = data.%s;
            head_info += "    raw_info[offset] = data.%s;\n    offset++;" % name
        else:
            module = """
            for (i = 0; i < %d; i++){
                raw_info[i + offset] = data.%s[i]; 
            }
            offset+=%d;
            """
            head_info += module % (length, name, length)
    return head_info

    
def generate_encode_code(type_name, type_define):
    sum_byte = calc_type_bytes(type_define)
    c_info = "void %s_encoder(%s data, unsigned char byte_array[%d], unsigned int *valid_length) {\n" % (type_name, type_name, sum_byte*2)
    c_info += "\tunsigned char raw_info[%d] = { 0x00 };\n" % sum_byte
    c_info += """
    byte_decoder decoder;
    unsigned short i = 0;
    unsigned int j = 0;
    unsigned int offset = 0;
    unsigned int delimiter_count = 0;
    """
    for elem in type_define:
        c_info += "    //encoder of %s struct from byte array\n" % elem[0]
        c_info += generate_encode_field_code(elem) + "\n"
    
    c_info += "escape_encode(raw_info, %d, byte_array, valid_length);\n" % sum_byte
    c_info += "}\n"
    return c_info

    
def generate_decode_field_code(elem):
    head_info = ""
    name = elem[0]
    type = elem[1]
    length = elem[2] 
    if type in ['u32','f32','i32']:
        decode_type = decode_dict[type]
        if length==0:
            head_info += """
    for (i = 0; i < 4; ++i){
        decoder.bytes[i] = byte_array[i + offset];
    }
    offset += 4;
    data->%s = decoder.%s;
            """ % (name, decode_type)
        else:
            head_info += "    for (j = 0; j<%d; ++j){" % length
            head_info += """
        for (i = 0; i < 4; ++i){
            decoder.bytes[i] = byte_array[i + offset];
        }
        offset += 4;
        data->%s[j] = decoder.%s;
        }
            """ % (name, decode_type)
    elif type=="c08":
        if length==0:
            head_info += "    data->%s = byte_array[offset];\n    offset++;" % name
        else:
            module = """
            for (i = 0; i < %d; i++){
                data->%s[i] = byte_array[i + offset];
            }
            offset+=%d;
            """
            head_info += module % (length, name, length)
    return head_info

def generate_print_code_extern(type_name, type_define):
    func  = "#ifndef __MCU_CODE__\n"
    func += "extern void print_%s(%s st_input);\n" % (type_name, type_name)
    func += "#endif\n"
    return func
    
def generate_print_code(type_name, type_define):
    func = "#ifndef __MCU_CODE__\n"
    func += "void print_%s(%s st_input) { \n    unsigned int i = 0;\n" % (type_name, type_name)
    func += "printf(\"%s visualiz\\n\");\n" % (type_name)
    for elem in type_define:
        elem_name = elem[0]
        elem_type = elem[1]
        elem_size = elem[2]
        if elem_type=="u32" or elem_type=="i32" or elem_type=="c08":
            if elem_size==0:
                func += "printf(\"  %s:\\t%%d\\n\", st_input.%s);\n" % (elem_name, elem_name)
            else:
                func += "for(i = 0;i<%d;++i) printf(\"  %s[%%d]:\\t%%d\\n\", i, st_input.%s[i]);\n" % (elem_size, elem_name, elem_name)
        elif elem_type=="f32":
            if elem_size==0:
                func += "printf(\"  %s:\\t%%f\\n\", st_input.%s);\n" % (elem_name, elem_name)
            else:
                func += "for(i = 0;i<%d;++i) printf(\"  %s[%%d]:\\t%%f\\n\", i, st_input.%s[i]);\n" % (elem_size, elem_name, elem_name)
    func += "}\n"
    func += "#endif\n"
    return func

def generate_convert_file_code_extern(type_name, type_define):
    func  = "#ifndef __MCU_CODE__\n"
    func += "extern int convert_%s(char *bin_filename, char *csv_filename);\n" % (type_name)
    func += "#endif\n"
    return func
    
def generate_convert_file_code(type_name, type_define):
    sum_byte = calc_type_bytes(type_define)
    type_subnames = [x[0] for x in type_define]
    codeseg_printhead = "fprintf(fw,\"%s\\n\");" % ",".join(type_subnames)
    codeseg_printcsv = ""
    for elem in type_define:
        elem_name = elem[0]
        elem_type = elem[1]
        elem_size = elem[2]
        if elem_type=="u32" or elem_type=="i32" or elem_type=="c08":
            if elem_size==0:
                codeseg_printcsv += "fprintf(fw,\"%%d,\", st_input.%s);\n" % (elem_name)
            else:
                codeseg_printcsv += "for(j = 0; j<%d; ++j) fprintf(fw,\"%%d \", st_input.%s[j]);\n" % (elem_size, elem_name)
                codeseg_printcsv += "fprintf(fw,\",\");\n"
        elif elem_type=="f32":
            if elem_size==0:
                codeseg_printcsv += "fprintf(fw,\"%%f,\", st_input.%s);\n" % (elem_name)
            else:
                codeseg_printcsv += "for(j = 0; j<%d; ++j) fprintf(fw,\"%%f \", st_input.%s[j]);\n" % (elem_size, elem_name)
                codeseg_printcsv += "fprintf(fw,\",\");\n"
    func  = "#ifndef __MCU_CODE__\n"
    func += "int convert_%s(char *bin_filename, char *csv_filename){\n" % (type_name)
    func += """
	int readbuf;
	unsigned char buffer[1024] = {0};
	unsigned int i = 0;
	FILE *fb = fopen(bin_filename, "rb");
	FILE *fw = fopen(csv_filename, "w");
    if (fb == NULL || fw == NULL){
		printf("Fail to open file: %%s or %%s\\n", bin_filename, csv_filename);
		return -1;
	}
	printf("Open file successfully.\\n");
    /*Print csv head code*/
    %s
	readbuf = fgetc(fb);
	while (readbuf != EOF){
		if (readbuf == DELIMITER) {
            if (i>=%d && i<=(%d*2)) {
                unsigned int j = 0; 
                %s st_input;
                if(%s_decoder(&st_input, buffer, i)==0x00){
                    %s
                    /*Print csv code*/
                    fprintf(fw, "\\n");
                }
                
            }
			i = 0;
		} else {
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
    """ % (codeseg_printhead, sum_byte, sum_byte, type_name, type_name, codeseg_printcsv)
    func += "}\n"
    func += "#endif\n"
    return func
    
def generate_decode_code(type_name, type_define):
    sum_byte = calc_type_bytes(type_define)
    c_info = "unsigned char %s_decoder(%s *data, unsigned char byte_buffer[%d], unsigned int valid_length) {\n" % (type_name, type_name, sum_byte*2)
    c_info += """
    unsigned short i = 0;
    unsigned int j = 0;
    unsigned int offset = 0;
    unsigned char byte_array[%d] = {0};
    byte_decoder decoder;
    offset = escape_decode(byte_buffer, valid_length, byte_array, %d);
    if (offset!=%d) return 0x01;
    offset = 0;
    """ % (sum_byte, sum_byte, sum_byte)
    for elem in type_define:
        c_info += "    //decoder of %s struct from byte array\n" % elem[0]
        c_info += generate_decode_field_code(elem) + "\n\n"
    c_info += "return 0x00;\n"    
    c_info += "\n}\n"
    return c_info
    
def generate_extern_encode(type_name, type_define):
    return "extern void %s_encoder(%s data, unsigned char byte_array[%d], unsigned int *valid_length);\n" % (type_name, type_name, calc_type_bytes(type_define)*2)    

def generate_extern_decode(type_name, type_define):
    return "extern unsigned char %s_decoder(%s *data, unsigned char byte_array[%d], unsigned int valid_length);\n" % (type_name, type_name, calc_type_bytes(type_define)*2)

    
if __name__=="__main__":
    ptlist = []
    ptdefine = [
        ("tick", "u32", 0),
        ("ax", "f32", 0),
        ("ay", "f32", 0),
        ("az", "f32", 0),
        ("gx", "f32", 0),
        ("gy", "f32", 0),
        ("gz", "f32", 0),
        ("pitch", "f32", 0),
        ("roll", "f32", 0),
        ("yaw", "f32", 0),
        ("temperature","f32",0)
    ]
    ptname = "posture_info"
    ptlist.append((ptname, ptdefine))
    
    ptdefine = [
        ("tick", "u32", 0),
        ("lng", "f32", 0),
        ("lat", "f32", 0)
    ]
    ptname = "gps_info"
    ptlist.append((ptname, ptdefine))
    
    ptdefine = [
        ("tick", "u32", 0),
        ("std_id", "u32", 0),
        ("ext_id", "u32", 0),
        ("ide", "c08", 0),
        ("rtr", "c08", 0),
        ("dlc", "c08", 0),
        ("data", "c08", 8),
        ("fmi", "c08", 0),
    ]
    ptname = "can_info"
    ptlist.append((ptname, ptdefine))
    
    fp = open("bin_writer.h","w")
    fp.write("// THIS CODE IS GENERATED FROM PYTHON GENERATOR, DO NOT FIX THIS FILE.\n")
    fp.write("""
#ifndef _BIN_WRITER_HEAD_
#define _BIN_WRITER_HEAD_
#define DELIMITER 0x7E
#define DELIMITER_APPENDER 0x02
#define ESCAPER 0x7D
#define ESCAPER_APPENDER 0x01

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

    """)
    
    for ptname, ptdefine in ptlist:
        fp.write(generate_structs(ptname, ptdefine))
        fp.write("\n")
        fp.write("#define %s_SIZE %d\n" % (ptname.upper(), calc_type_bytes(ptdefine)))
        fp.write("\n")
        fp.write(generate_extern_encode(ptname,ptdefine))
        fp.write(generate_extern_decode(ptname,ptdefine))
        fp.write(generate_print_code_extern(ptname,ptdefine))        
        fp.write(generate_convert_file_code_extern(ptname,ptdefine))
        fp.write("\n\n")
    fp.write("#endif\n")
    fp.close()
    
    
    fp = open("bin_writer.c","w")
    fp.write("// THIS CODE IS GENERATED FROM PYTHON GENERATOR, DO NOT FIX THIS FILE.\n")
    fp.write("""
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
            }else if (byte_in[i + 1] == ESCAPER_APPENDER){
                i++;
                byte_out[offset] = ESCAPER;
            }else{
                //Error if executing here
                byte_out[offset] = ESCAPER;
                err_code = 0;
            }
        }else{
            byte_out[offset] = byte_in[i];
        }
        offset++;
        if (offset>max_offset) {// Error
            return 0x00;
        }
    }
    if (err_code==0){// Error
        return 0x00;
    }else{
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

    """)
    for ptname, ptdefine in ptlist:
        fp.write(generate_decode_code(ptname, ptdefine))
        fp.write("\n\n")
        fp.write(generate_encode_code(ptname, ptdefine))
        fp.write("\n\n")
        fp.write(generate_print_code(ptname,ptdefine))
        fp.write("\n\n")        
        fp.write(generate_convert_file_code(ptname,ptdefine))
        fp.write("\n\n")
    fp.close()
    
    