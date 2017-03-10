#ifndef _hardiic_h
#define _hardiic_h

#ifdef __cplusplus
	extern "C"{
#endif		

#include "sys.h"

#define AT24Cxx_PageSize 8  //存储器块大小

void Hard_IIC_Init                               (I2C_TypeDef* I2Cx, u8 SlaveAdd);  //硬件IIC初始化
void Hard_IICWriteOneByte                        (I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 WriteAdd, u8 Data, u8 * err);  //硬件IIC发送一个字节数据
u8   Hard_IIC_ReadOneByte                        (I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 * err);  //硬件IIC读取一个字节数据
void Hard_IICWaiteStandby                        (I2C_TypeDef* IICx, uint8_t SlaveAdd);  //硬件IIC等待从设备内部操作完成
void Hard_IIC_WriteNByte                         (I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err);  //硬件IIC发送多个字节数据
void Hard_IIC_PageRead                           (I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 NumToRead, u8 * pBuffer, u8 * err);  //硬件IIC读取多个字节数据
void Hard_IIC_PageWrite                          (I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err);  //AT24C02页写函数

extern volatile unsigned short IIC_ERROR_CODE;
#ifdef __cplusplus
	}
#endif


#endif  /* end hardiic.h */
