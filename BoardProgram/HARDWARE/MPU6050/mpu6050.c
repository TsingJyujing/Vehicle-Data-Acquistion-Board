#include "mpu6050.h"

#define MPU_6050_SAMPLE_FREQUENCY 25
u8 MPU_Init(void) { 
	u8 res;
    Hard_IIC_Init(MPU_IIC, 0xCC);//��ʼ��IIC����
	MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80);	//��λMPU6050
    delay_ms(200);
    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00);	//����MPU6050 
	MPU_Set_Gyro_Fsr(3);					//�����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(0);					//���ٶȴ�����,��2g
    res = MPU_Set_LPF(10); //��ͨ�˲���10Hz
    if (res) {
        printf("Low-pass filter set failed.");
    }
	MPU_Set_Rate(MPU_6050_SAMPLE_FREQUENCY);						//���ò�����100Hz
	MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//�ر������ж�
	MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C��ģʽ�ر�
	MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);	//�ر�FIFO
	MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT���ŵ͵�ƽ��Ч
	res = MPU_Read_Byte(MPU_DEVICE_ID_REG);
    printf("MPU Device ID: %02X\r\n", res);
	if(res == MPU_DEVICE_ID) {//����ID��ȷ
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//����CLKSEL,PLL X��Ϊ�ο�
		MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	//���ٶ��������Ƕ�����
		MPU_Set_Rate(MPU_6050_SAMPLE_FREQUENCY);						//���ò�����Ϊ100Hz
        return 0;
 	}else{
        return 1;
    }
	
}


//����MPU6050�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}
//����MPU6050���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}

//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}
//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

float MPU_Get_Temperature(void) {
    u8 buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU_IIC_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp = 36.53f+((float)raw)/340.0f;  
    return temp;
}

//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU_IIC_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];  
		*gy=((u16)buf[2]<<8)|buf[3];  
		*gz=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU_IIC_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];  
		*ay=((u16)buf[2]<<8)|buf[3];  
		*az=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}

//IIC����д
//addr:������ַ 
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf) {
    u8 e = 0x00; 
    Hard_IIC_WriteNByte(MPU_IIC, addr, reg, len, buf, &e); 
    #ifndef RELEASE_VERSION
        if(e) printf("IIC Write N Bytes Error Code %02X\r\n", e);
    #endif
    return e;
} 

//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf){
#if IIC_LIB_SELECT==USE_SOFTWARE_IIC
    IIC_Start(); 
    IIC_Send_Byte(addr);
    if(IIC_Wait_Ack()){
        IIC_Stop();		 
        return 1;		
    }
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(addr+1);
    IIC_Wait_Ack();
    while(len)
    {
        if(len==1)*buf=IIC_Read_Byte(0);
        else *buf=IIC_Read_Byte(1);
        len--;
        buf++; 
    }    
    IIC_Stop();
    return 0;	
#else
    u8 e = 0x00; 
    Hard_IIC_PageRead(MPU_IIC, addr, reg, len, buf, &e);
    #ifndef RELEASE_VERSION
        if(e) printf("IIC Read N Bytes Error Code %02X\r\n", e);
    #endif
    return e;
#endif
}
//IICдһ���ֽ� 
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//    ����,�������
u8 MPU_Write_Byte(u8 reg,u8 data) {
    #if IIC_LIB_SELECT==USE_SOFTWARE_IIC
    IIC_Start(); 
	IIC_Send_Byte(MPU_READ);
	if(IIC_Wait_Ack())
	{
		IIC_Stop();		 
		return 1;		
	}
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();	
	IIC_Send_Byte(data);
	if(IIC_Wait_Ack())
	{
		IIC_Stop();	 
		return 1;		 
	}		 
    IIC_Stop();	 
	return 0;
    #else
    u8 e = 0x00; 
    //NVIC_DisableIRQ(USART2_IRQn);
    //NVIC_DisableIRQ(TIM3_IRQn);
    Hard_IICWriteOneByte(MPU_IIC, MPU_IIC_ADDR, reg, data, &e); 
    //NVIC_EnableIRQ(USART2_IRQn);
    //NVIC_EnableIRQ(TIM3_IRQn);
    #ifndef RELEASE_VERSION
        if(e) printf("IIC Write 1 Bytes Error Code %02X\r\n", e);
    #endif
    return e;
    #endif
}


//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
u8 MPU_Read_Byte(u8 reg) {  
    #if IIC_LIB_SELECT!=USE_SOFTWARE_IIC
    u8 e = 0x00, rtn = 0x00; 

    //NVIC_DisableIRQ(USART2_IRQn);
    //NVIC_DisableIRQ(TIM3_IRQn);
    rtn = Hard_IIC_ReadOneByte(MPU_IIC, MPU_IIC_ADDR, reg, &e);
    //NVIC_EnableIRQ(TIM3_IRQn);
    //NVIC_EnableIRQ(USART2_IRQn);
    
    #ifndef RELEASE_VERSION
        if(e) printf("IIC Read 1 Bytes Error Code %02X\r\n", e);
    #endif
    return rtn;
    #else
    u8 res;
    IIC_Start(); 
	IIC_Send_Byte(MPU_READ);//??????+???	
	IIC_Wait_Ack();		//???? 
    IIC_Send_Byte(reg);	//??????
    IIC_Wait_Ack();		//????
    IIC_Start();
	IIC_Send_Byte(MPU_WRITE);//??????+???	
    IIC_Wait_Ack();		//???? 
	res=IIC_Read_Byte(0);//????,??nACK 
    IIC_Stop();			//???????? 
	return res;		
    #endif
}

