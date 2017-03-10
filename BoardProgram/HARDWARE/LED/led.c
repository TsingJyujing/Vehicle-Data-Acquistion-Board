#include "led.h" 

//LED�˿ڶ���
#define D2 PAout(6)	// D2
#define D3 PAout(7)	// D3	 

//��ʼ��PA6��PA7Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void initLED(void){    	 
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
    //GPIOF9,F10��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//D2��D3��ӦIO��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
    GPIO_SetBits(GPIOA,GPIO_Pin_6 | GPIO_Pin_7);//���øߣ�����
}

/*
	Turn D2 on
*/
void onD2(void ){
	D2 = 0;
}

/*
	Turn D3 on
*/
void onD3(void ){
	D3 = 0;
}

/*
	Turn D2 off
*/
void offD2(void ){
	D2 = 1;
}

/*
	Turn D3 off
*/
void offD3(void ){
	D3 = 1;
}

/*
	Inverse D2 GPIO output
*/
void inverseD2(void ){
	D2 = !D2;
}

/*
	Inverse D3 GPIO output
*/
void inverseD3(void ){
	D3 = !D3;
}
