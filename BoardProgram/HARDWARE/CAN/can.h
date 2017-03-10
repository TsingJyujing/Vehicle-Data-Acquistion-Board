#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    

typedef struct st_lc_node{
    CanRxMsg Data;
    unsigned int gotTick;
    struct st_lc_node *next;
} linkedCANNode;

extern volatile linkedCANNode *nowWriteCAN;
extern volatile linkedCANNode *nowProcessedCAN;
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN��ʼ��
 
u8 CAN1_Send_Msg(u8* msg,u8 len);						//��������

u8 CAN1_Receive_Msg(u8 *buf);							//��������

void printCAN(CanRxMsg *msg);

#endif

















