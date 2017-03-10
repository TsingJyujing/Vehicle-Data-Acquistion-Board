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
* @Name   : Ӳ��IIC��ʼ��
*
* @Brief  : none
*
* @Input  : I2Cx:           IIC��
*           SlaveAdd:       ��Ϊ���豸ʱʶ���ַ
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
	I2C_InitStructure.I2C_OwnAddress1 = SlaveAdd;  //���豸��ַ
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = (400 * 1000);  //SCL���100KHz


	I2C_Cmd(IICx, ENABLE);
	I2C_Init(IICx, &I2C_InitStructure);
	I2C_AcknowledgeConfig(IICx, ENABLE);

	IICx->CR1 |= 1 << 7;
}

/**
*****************************************************************************
* @Name   : Ӳ��IIC�ȴ����豸�ڲ��������
*
* @Brief  : none
*
* @Input  : I2Cx:     IIC��
*           SlaveAdd: ��Ϊ���豸ʱʶ���ַ
*           ReadAdd:  ��ȡ��EEPROM�ڴ��ַ
*
* @Output : *err:     ���صĴ���ֵ
*
* @Return : ��ȡ��������
*****************************************************************************
**/
void Hard_IICWaiteStandby(I2C_TypeDef* IICx, uint8_t SlaveAdd)
{
	u16 tmp = 0;
#ifndef RELEASE_MODE
	printf("Hard_IICWaiteStandby\r\n");
#endif
	IICx->SR1 &= 0x0000;  //���״̬�Ĵ���1
	do {
		I2C_GenerateSTART(IICx, ENABLE);  //������ʼ�ź�
		tmp = IICx->SR1;  //��ȡSR1�Ĵ�����Ȼ��д�����ݼĴ������������SBλ��EV5
		I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //���ʹ��豸��ַ
	} while ((IICx->SR1 & 0x0002) == 0x0000);  //��ADDR = 1ʱ������Ӧ���ˣ�����ѭ��
	I2C_ClearFlag(IICx, I2C_FLAG_AF);  //���Ӧ��ʧ�ܱ�־λ
	I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
}


unsigned char waitIICxResponse(I2C_TypeDef* IICx, unsigned short maxWaitCount) {
	unsigned short waitingCount = 0;
	while (I2C_GetFlagStatus(IICx, I2C_FLAG_BUSY))  {//�ȴ�IIC
		waitingCount++;
		if (waitingCount > maxWaitCount) {
			I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
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
* @Name   : Ӳ��IIC����һ���ֽ�����
*
* @Brief  : none
*
* @Input  : I2Cx:     IIC��
*           SlaveAdd: ��Ϊ���豸ʱʶ���ַ
*           WriteAdd: д��EEPROM�ڴ��ַ
*           Data:     д�������
*
* @Output : *err:     ���صĴ���ֵ
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

	I2C_GenerateSTART(IICx, ENABLE);  //������ʼ�ź�

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_MODE_SELECT);
	if (*err) {
		*err = 0x02;
		return;
	}

	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //�����豸��ַ

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	if (*err) {
		*err = 0x03;
		return;
	}

	flag = IICx->SR2;  //�����ȡSR1�Ĵ�����,��SR2�Ĵ����Ķ����������ADDRλ
	I2C_SendData(IICx, WriteAdd);  //���ʹ洢��ַ

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	if (*err) {
		*err = 0x04;
		return;
	}

	I2C_SendData(IICx, Data);  //��������

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	if (*err) {
		*err = 0x05;
		return;
	}

	I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
}

/**
*****************************************************************************
* @Name   : Ӳ��IIC��ȡһ���ֽ�����
*
* @Brief  : none
*
* @Input  : I2Cx:     IIC��
*           SlaveAdd: ��Ϊ���豸ʱʶ���ַ
*           ReadAdd:  ��ȡ��EEPROM�ڴ��ַ
*
* @Output : *err:     ���صĴ���ֵ
*
* @Return : ��ȡ��������
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
	I2C_GenerateSTART(IICx, ENABLE);  //������ʼ�ź�

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_MODE_SELECT);
	if (*err) {
		*err = 0x02;
		return 0x00;
	}


	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //�����豸��ַ
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	if (*err) {
		*err = 0x03;
		return 0x00;
	}

	flag = IICx->SR2;  //�����ȡSR1�Ĵ�����,��SR2�Ĵ����Ķ����������ADDRλ�������٣�����������������

	I2C_SendData(IICx, ReadAdd);  //���ʹ洢��ַ


	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	if (*err) {
		*err = 0x04;
		return 0x00;
	}

	I2C_GenerateSTART(IICx, ENABLE);  //�����ź�

	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_MODE_SELECT);
	if (*err) {
		*err = 0x05;
		return 0x00;
	}

	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Receiver);  //��ȡ����
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	if (*err) {
		*err = 0x06;
		return 0x00;
	}

	flag = IICx->SR2;

	I2C_AcknowledgeConfig(IICx, DISABLE);  //����NACK
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
* @Name   : Ӳ��IIC���Ͷ���ֽ�����
*
* @Brief  : none
*
* @Input  : I2Cx:       IIC��
*           SlaveAdd:   ��Ϊ���豸ʱʶ���ַ
*           WriteAdd:   д��EEPROM�ڴ���ʼ��ַ
*           NumToWrite: д��������
*           *pBuffer:   д��������黺��
*
* @Output : *err:     ���صĴ���ֵ
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

	I2C_GenerateSTART(IICx, ENABLE);  //������ʼ�ź�
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_MODE_SELECT);
	if (*err) {
		*err = 0x02;
		return;
	}

	I2C_Send7bitAddress(IICx, SlaveAdd, I2C_Direction_Transmitter);  //�����豸��ַ
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	if (*err) {
		*err = 0x03;
		return;
	}


	//��ȡSR2״̬�Ĵ���
	sta = IICx->SR2;  //�����ȡSR1�Ĵ�����,��SR2�Ĵ����Ķ����������ADDRλ�������٣�����������������
	I2C_SendData(IICx, WriteAdd);  //���ʹ洢��ַ
	*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	if (*err) {
		*err = 0x03;
		return;
	}

	while (NumToWrite--) {
		I2C_SendData(IICx, *pBuffer);  //��������
		pBuffer++;
		*err = waitIICxEvent(IICx, MAX_WAITING_COUNT, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
		if (*err) {
			*err = 0x04;
			return;
		}
	}

	I2C_GenerateSTOP(IICx, ENABLE);  //����ֹͣ�ź�
}

/**
*****************************************************************************
* @Name   : Ӳ��IIC��ȡ����ֽ�����
*
* @Brief  : none
*
* @Input  : I2Cx:      IIC��
*           SlaveAdd:  ��Ϊ���豸ʱʶ���ַ
*           ReadAdd:   ��ȡ��EEPROM�ڴ���ʼ��ַ
*           NumToRead: ��ȡ����
*
* @Output : *pBuffer: �������������
*           *err:     ���صĴ���ֵ
*
* @Return : ��ȡ��������
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
* @Name   : AT24C02ҳд����
*
* @Brief  : none
*
* @Input  : I2Cx:       IIC��
*           SlaveAdd:   ��Ϊ���豸ʱʶ���ַ
*           WriteAdd:   д��EEPROM�ڴ���ʼ��ַ
*           NumToWrite: д��������
*           *pBuffer:   д��������黺��
*
* @Output : *err:     ���صĴ���ֵ
*
* @Return : none
*****************************************************************************
**/
void Hard_IIC_PageWrite(I2C_TypeDef * IICx, u8 SlaveAdd, u8 WriteAdd, u8 NumToWrite, u8 * pBuffer, u8 * err) {
	u8 Num_Page = 0;  //����ҳ��С��Ҫд��Ĵ���
	u8 Num_Remain = 0;  //ҳдʣ���ֽ���
	u8 Addr = 0;  //���ݿ��׵�ַΪEEPROMҳ��������
	u8 cnt = 0;  //��������
#ifndef RELEASE_MODE
	printf("5");
#endif
	Addr = WriteAdd % AT24Cxx_PageSize;  //�������ݿ��׵�ַ�Ƿ���ҳ��С��������
	cnt = AT24Cxx_PageSize - Addr;
	Num_Page = NumToWrite / AT24Cxx_PageSize;  //�õ�����
	Num_Remain = NumToWrite % AT24Cxx_PageSize;  //ʣ���ֽ���
	//
	//�ж�д������
	//
	if (Addr == 0)  //������
	{
		if (Num_Page == 0)  //���д������ݿ鳤��С��ҳ��С
		{
			Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //С��һ��ҳ��С����
			Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
		}
		else  //����һҳ����
		{
			while (Num_Page--)  //����ҳ��д
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, AT24Cxx_PageSize, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
				WriteAdd += AT24Cxx_PageSize;
				pBuffer += AT24Cxx_PageSize;
			}
			if (Num_Remain != 0)  //����һ��ҳ������
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //С��һ��ҳ��С����
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
			}
		}
	}
	else  //����������
	{
		if (Num_Page == 0)  //С��һ��ҳ��������
		{
			Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //д��С��һ��ҳ��������
			Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
		}
		else  //����һ��Ҳ������
		{
			//
			//���¼���
			//
			NumToWrite -= cnt;
			Num_Page = NumToWrite / AT24Cxx_PageSize;
			Num_Remain = NumToWrite % AT24Cxx_PageSize;

			if (cnt != 0)
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, cnt, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
				WriteAdd += cnt;
				pBuffer += cnt;
			}
			while (Num_Page--)  //����ҳ��д
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, AT24Cxx_PageSize, pBuffer, err);
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
				WriteAdd += AT24Cxx_PageSize;
				pBuffer += AT24Cxx_PageSize;
			}
			if (Num_Remain != 0)  //����һ��ҳ������
			{
				Hard_IIC_WriteNByte(IICx, SlaveAdd, WriteAdd, Num_Remain, pBuffer, err);  //С��һ��ҳ��С����
				Hard_IICWaiteStandby(IICx, SlaveAdd);  //�ȴ��������
			}
		}
	}
}
