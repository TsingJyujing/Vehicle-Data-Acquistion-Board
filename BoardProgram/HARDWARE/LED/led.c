#include "led.h" 

//LED端口定义
#define D2 PAout(6)	// D2
#define D3 PAout(7)	// D3	 

//初始化PA6和PA7为输出口.并使能这两个口的时钟		    
//LED IO初始化
void initLED(void){    	 
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
    //GPIOF9,F10初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//D2和D3对应IO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
    GPIO_SetBits(GPIOA,GPIO_Pin_6 | GPIO_Pin_7);//设置高，灯灭
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
