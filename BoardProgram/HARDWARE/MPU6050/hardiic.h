#ifndef _hardiic_h
#define _hardiic_h

#ifdef __cplusplus
	extern "C"{
#endif		

#include "sys.h"

#define AT24Cxx_PageSize 8  //�洢�����С

void Hard_IIC_Init                               (I2C_TypeDef* I2Cx, u8 SlaveAdd);  //Ӳ��IIC��ʼ��
void Hard_IICWriteOneByte                        (I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 WriteAdd, u8 Data, u8 * err);  //Ӳ��IIC����һ���ֽ�����
u8   Hard_IIC_ReadOneByte                        (I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 * err);  //Ӳ��IIC��ȡһ���ֽ�����
void Hard_IICWaiteStandby                        (I2C_TypeDef* IICx, uint8_t SlaveAdd);  //Ӳ��IIC�ȴ����豸�ڲ��������
void Hard_IIC_WriteNByte                         (I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err);  //Ӳ��IIC���Ͷ���ֽ�����
void Hard_IIC_PageRead                           (I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 NumToRead, u8 * pBuffer, u8 * err);  //Ӳ��IIC��ȡ����ֽ�����
void Hard_IIC_PageWrite                          (I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err);  //AT24C02ҳд����

extern volatile unsigned short IIC_ERROR_CODE;
#ifdef __cplusplus
	}
#endif


#endif  /* end hardiic.h */