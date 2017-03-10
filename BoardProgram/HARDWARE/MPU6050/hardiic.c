#include "sys.h"
#include "hardiic.h"
#include "stdio.h"
#include "delay.h"
volatile unsigned short IIC_ERROR_CODE = 0x00;
#define RELEASE_MODE
#define MAX_FAIL_TIMES 256
#define MAX_WAITING_COUNT 800
/**
*****************************************************************************
* @Name   : 硬件IIC初始化
*
* @Brief  : none
*
* @Input  : I2Cx:           IIC组
*           SlaveAdd:       作为从设备时识别地址
*
* @Output : none
*
* @Return : none
*****************************************************************************
**/

void Hard_IIC_Init(I2C_TypeDef* IICx, u8 SlaveAdd){
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

#ifndef RELEASE_MODE
	printf("Hard_IIC_Init\r\n");
#endif
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);// enable GPIOB CLOCK 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = SlaveAdd;  //从设备地址
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = (400 * 1000);  //SCL最大100KHz


	I2C_Cmd(IICx, ENABLE);
	I2C_Init(IICx, &I2C_InitStructure);
	I2C_AcknowledgeConfig(IICx, ENABLE);

	IICx->CR1 |= 1 << 7;
}

/**
*****************************************************************************
* @Name   : 硬件IIC等待从设备内部操作完成
*
* @Brief  : none
*
* @Input  : I2Cx:     IIC组
*           SlaveAdd: 作为从设备时识别地址
*           ReadAdd:  读取的EEPROM内存地址
*
* @Output : *err:     返回的错误值
*
* @Return : 读取到的数据
*****************************************************************************
**/
void Hard_IICWaiteStandby(I2C_TypeDef* IICx, uint8_t SlaveAdd)
{
	u16 tmp = 0;
#ifndef RELEASE_MODE
	printf("Hard_IICWaiteStandby\r\n");
#endif
	IICx->SR1 &= 0x0000;  //清除状态寄存器1
	do {
		I2C_GenerateSTART(IICx, ENABLE);  //产生起始信号
		tmp = IICx->SR1;  //读取SR1寄存器，然后写入数据寄存器操作来清除SB位，EV5
		I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //发送从设备地址
	} while ((IICx->SR1 & 0x0002) == 0x0000);  //当ADDR = 1时，表明应答了，跳出循环
	I2C_ClearFlag(IICx, I2C_FLAG_AF);  //清除应答失败标志位
	I2C_GenerateSTOP(IICx, ENABLE);  //发送停止信号
}


unsigned char waitIICxResponse(I2C_TypeDef* IICx, unsigned short maxWaitCount) {
	unsigned short waitingCount = 0;
	while (I2C_GetFlagStatus(IICx, I2C_FLAG_BUSY))  {//等待IIC
		waitingCount++;
		if (waitingCount > maxWaitCount) {
			I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
			return 0x01;
		}
	}
	return 0x00;
}
unsigned char waitIICxEvent(I2C_TypeDef* IICx, unsigned short maxWaitCount, unsigned int Event) {
	unsigned short waitingCount = 0;
	while (!I2C_CheckEvent(IICx, Event)){
		waitingCount++;
		if (waitingCount > MAX_WAITING_COUNT){
			I2C_GenerateSTOP(IICx, ENABLE);
			return 0x02;
		}
	}
	return 0x00;
}

/**
*****************************************************************************
* @Name   : 硬件IIC发送一个字节数据
*
* @Brief  : none
*
* @Input  : I2Cx:     IIC组
*           SlaveAdd: 作为从设备时识别地址
*           WriteAdd: 写入EEPROM内存地址
*           Data:     写入的数据
*
* @Output : *err:     返回的错误值
*
* @Return : none
*****************************************************************************
**/
void Hard_IICWriteOneByte(I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 WriteAdd, u8 Data, u8 * err) {
	u16 waitingCount = 0;
	u16 flag = 0;

#ifndef RELEASE_MODE
	printf("Hard_IICWriteOneByte\r\n");
#endif

	*err = waitIICxResponse(IICx, MAX_WAITING_COUNT);
	if (*err) {
		*err = 0x01;
		return;
	}

	I2C_GenerateSTART(IICx, ENABLE);  //产生起始信号

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_MODE_SELECT);
	if (*err) {
		*err = 0x02;
		return;
	}

	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //发送设备地址

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	if (*err) {
		*err = 0x03;
		return;
	}

	flag = IICx->SR2;  //软件读取SR1寄存器后,对SR2寄存器的读操作将清除ADDR位
	I2C_SendData(IICx, WriteAdd);  //发送存储地址

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	if (*err) {
		*err = 0x04;
		return;
	}

	I2C_SendData(IICx, Data);  //发送数据

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	if (*err) {
		*err = 0x05;
		return;
	}

	I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
}

/**
*****************************************************************************
* @Name   : 硬件IIC读取一个字节数据
*
* @Brief  : none
*
* @Input  : I2Cx:     IIC组
*           SlaveAdd: 作为从设备时识别地址
*           ReadAdd:  读取的EEPROM内存地址
*
* @Output : *err:     返回的错误值
*
* @Return : 读取到的数据
*****************************************************************************
**/
u8 Hard_IIC_ReadOneByte(I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 * err) {
	u16 i = 0;
	u8 waitingCount = 0;
	u16 flag = 0;
#ifndef RELEASE_MODE
	printf("Hard_IIC_ReadOneByte\r\n");
#endif
	*err = waitIICxResponse(IICx, MAX_WAITING_COUNT);
	if (*err) return 0;
	I2C_GenerateSTART(IICx, ENABLE);  //发送起始信号

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_MODE_SELECT);
	if (*err) {
		*err = 0x02;
		return 0x00;
	}


	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //发送设备地址
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	if (*err) {
		*err = 0x03;
		return 0x00;
	}

	flag = IICx->SR2;  //软件读取SR1寄存器后,对SR2寄存器的读操作将清除ADDR位，不可少！！！！！！！！！

	I2C_SendData(IICx, ReadAdd);  //发送存储地址


	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	if (*err) {
		*err = 0x04;
		return 0x00;
	}

	I2C_GenerateSTART(IICx, ENABLE);  //重启信号

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_MODE_SELECT);
	if (*err) {
		*err = 0x05;
		return 0x00;
	}

	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Receiver);  //读取命令
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	if (*err) {
		*err = 0x06;
		return 0x00;
	}

	flag = IICx->SR2;

	I2C_AcknowledgeConfig(IICx, DISABLE);  //发送NACK
	I2C_GenerateSTOP(IICx, ENABLE);

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_RECEIVED);
	if (*err) {
		*err = 0x07;
		return 0x00;
	}

	waitingCount = I2C_ReceiveData(IICx);
	I2C_AcknowledgeConfig(IICx, ENABLE);

	return waitingCount;
}

/**
*****************************************************************************
* @Name   : 硬件IIC发送多个字节数据
*
* @Brief  : none
*
* @Input  : I2Cx:       IIC组
*           SlaveAdd:   作为从设备时识别地址
*           WriteAdd:   写入EEPROM内存起始地址
*           NumToWrite: 写入数据量
*           *pBuffer:   写入的数据组缓存
*
* @Output : *err:     返回的错误值
*
* @Return : none
*****************************************************************************
**/
void Hard_IIC_WriteNByte(I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err) {
	u16 sta = 0;
	u16 waitingCount = 0;
#ifndef RELEASE_MODE
	printf("Hard_IIC_WriteNByte\r\n");
#endif

	*err = waitIICxResponse(IICx, MAX_WAITING_COUNT);
	if (*err) return;

	I2C_GenerateSTART(IICx, ENABLE);  //产生起始信号
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_MODE_SELECT);
	if (*err) {
		*err = 0x02;
		return;
	}

	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //发送设备地址
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	if (*err) {
		*err = 0x03;
		return;
	}


	//读取SR2状态寄存器
	sta = IICx->SR2;  //软件读取SR1寄存器后,对SR2寄存器的读操作将清除ADDR位，不可少！！！！！！！！！
	I2C_SendData(IICx, WriteAdd);  //发送存储地址
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	if (*err) {
		*err = 0x03;
		return;
	}

	while (NumToWrite--) {
		I2C_SendData(IICx, *pBuffer);  //发送数据
		pBuffer++;
		*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
		if (*err) {
			*err = 0x04;
			return;
		}
	}

	I2C_GenerateSTOP(IICx, ENABLE);  //产生停止信号
}

/**
*****************************************************************************
* @Name   : 硬件IIC读取多个字节数据
*
* @Brief  : none
*
* @Input  : I2Cx:      IIC组
*           SlaveAdd:  作为从设备时识别地址
*           ReadAdd:   读取的EEPROM内存起始地址
*           NumToRead: 读取数量
*
* @Output : *pBuffer: 数据输出缓冲区
*           *err:     返回的错误值
*
* @Return : 读取到的数据
*****************************************************************************
**/
void Hard_IIC_PageRead(I2C_TypeDef* IICx, uint8_t SlaveAdd, u8 ReadAdd, u8 NumToRead, u8 * pBuffer, u8 * err){
	u16 i = 0;
	u16 waitingCount = 0;
	u16 sta = 0;
#ifndef RELEASE_MODE
	printf("Hard_IIC_PageRead");
#endif
IIC_ERROR_CODE = 1;
	*err = waitIICxResponse(IICx, MAX_WAITING_COUNT);
	if (*err) return;
    
	I2C_GenerateSTART(IICx, ENABLE);
IIC_ERROR_CODE++;//2
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_MODE_SELECT);
	if (*err) {
		*err = 0x02;
		return;
	}
IIC_ERROR_CODE++;//3
	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	if (*err) {
		*err = 0x03;
		return;
	}


	sta = IICx->SR2;
IIC_ERROR_CODE++;//4
	I2C_SendData(IICx, ReadAdd);
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	if (*err) {
		*err = 0x03;
		return;
	}

	I2C_GenerateSTART(IICx, ENABLE);
IIC_ERROR_CODE++;//5
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_MODE_SELECT);
	if (*err) {
		*err = 0x03;
		return;
	}

	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Receiver);
IIC_ERROR_CODE++;//6
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	if (*err) {
		*err = 0x03;
		return;
	}

	sta = IICx->SR2;
IIC_ERROR_CODE++;//7
	i = 0;
	while (NumToRead) {
		if (NumToRead == 1){
			I2C_AcknowledgeConfig(IICx, DISABLE);
			I2C_GenerateSTOP(IICx, ENABLE);
		}
		if (I2C_CheckEvent(IICx, I2C_EVENT_MASTER_BYTE_RECEIVED)){
			*pBuffer = I2C_ReceiveData(IICx);
			pBuffer++;
			NumToRead--;
			i = 0;
		}
		else{
			i++;
			delay_us(1);
		}
		if (i >= MAX_FAIL_TIMES) {
			*err = 7;
			I2C_AcknowledgeConfig(IICx, ENABLE);
			return;
		}
	}
IIC_ERROR_CODE++;//8
	I2C_AcknowledgeConfig(IICx, ENABLE);
IIC_ERROR_CODE = 0x00;//0
}


/**
*****************************************************************************
* @Name   : AT24C02页写函数
*
* @Brief  : none
*
* @Input  : I2Cx:       IIC组
*           SlaveAdd:   作为从设备时识别地址
*           WriteAdd:   写入EEPROM内存起始地址
*           NumToWrite: 写入数据量
*           *pBuffer:   写入的数据组缓存
*
* @Output : *err:     返回的错误值
*
* @Return : none
*****************************************************************************
**/
void Hard_IIC_PageWrite(I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err) {
	u8 Num_Page = 0;  //按照页大小需要写入的次数
	u8 Num_Remain = 0;  //页写剩余字节数
	u8 Addr = 0;  //数据块首地址为EEPROM页数整数倍
	u8 cnt = 0;  //计数变量
#ifndef RELEASE_MODE
	printf("5");
#endif
	Addr = WriteAdd % AT24Cxx_PageSize;  //计算数据块首地址是否是页大小的整数倍
	cnt = AT24Cxx_PageSize - Addr;
	Num_Page = NumToWrite / AT24Cxx_PageSize;  //得到次数
	Num_Remain = NumToWrite % AT24Cxx_PageSize;  //剩余字节数
	//
	//判断写入数量
	//
	if (Addr == 0)  //整数倍
	{
		if (Num_Page == 0)  //如果写入的数据块长度小于页大小
		{
			Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //小于一个页大小数据
			Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
		}
		else  //大于一页数据
		{
			while (Num_Page--)  //按照页来写
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, AT24Cxx_PageSize, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
				WriteAdd += AT24Cxx_PageSize;
				pBuffer += AT24Cxx_PageSize;
			}
			if (Num_Remain != 0)  //不够一个页的数据
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //小于一个页大小数据
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
			}
		}
	}
	else  //不是整数倍
	{
		if (Num_Page == 0)  //小于一个页的数据量
		{
			Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //写入小于一个页的数据量
			Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
		}
		else  //大于一个也数据量
		{
			//
			//重新计算
			//
			NumToWrite -= cnt;
			Num_Page = NumToWrite / AT24Cxx_PageSize;
			Num_Remain = NumToWrite % AT24Cxx_PageSize;

			if (cnt != 0)
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, cnt, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
				WriteAdd += cnt;
				pBuffer += cnt;
			}
			while (Num_Page--)  //按照页来写
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, AT24Cxx_PageSize, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
				WriteAdd += AT24Cxx_PageSize;
				pBuffer += AT24Cxx_PageSize;
			}
			if (Num_Remain != 0)  //不够一个页的数据
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //小于一个页大小数据
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //等待操作完成
			}
		}
	}
}
