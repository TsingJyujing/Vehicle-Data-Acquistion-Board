#include "iwdg.h"


volatile unsigned char gps_flag = 0;
volatile unsigned char can_flag = 0;
volatile unsigned char mpu_flag = 0;

void setGPSFlag(){
    gps_flag = 0xff;
    IWDG_Feed();
}

void setCANFlag(){
    can_flag = 0xff;
    IWDG_Feed();
}

void setMPUFlag(){
    mpu_flag = 0xff;
    IWDG_Feed();
}

//��ʼ���������Ź�
//prer:��Ƶ��:0~7(ֻ�е�3λ��Ч!)
//rlr:�Զ���װ��ֵ,0~0XFFF.
//��Ƶ����=4*2^prer.�����ֵֻ����256!
//rlr:��װ�ؼĴ���ֵ:��11λ��Ч.
//ʱ�����(���):Tout=((4*2^prer)*rlr)/32 (ms).
// Tout=lambda prer,rlr:((4.0*(2**prer))*rlr)/32.0
void IWDG_Init(u8 prer,u16 rlr) {
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //ʹ�ܶ�IWDG->PR IWDG->RLR��д
	IWDG_SetPrescaler(prer); //����IWDG��Ƶϵ��
	IWDG_SetReload(rlr);   //����IWDGװ��ֵ
	IWDG_ReloadCounter(); //reload
	IWDG_Enable();       //ʹ�ܿ��Ź�
}

//ι�������Ź�
void IWDG_Feed(void) {
    if (gps_flag!=0 && mpu_flag!=0 && can_flag!=0) {
        IWDG_ReloadCounter();//reload
        //Reset Flag
        gps_flag = 0;
        can_flag = 0;
        mpu_flag = 0;
    }
}
