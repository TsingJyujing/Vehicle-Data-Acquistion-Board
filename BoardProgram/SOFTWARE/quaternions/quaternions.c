#include "quaternions.h"
#include "math.h"
#include "mpu_data_acq.h"

//---------------------------------------------------------------------------------------------------
// ��������

#define Kp 2.0f                        // ��������֧�������������ٶȼ�/��ǿ��
#define Ki 0.005f                // ��������֧���ʵ�������ƫ�����ν�
#define halfT SAMPLE_MILLSECOND_TIME/2000.0f           // �������ڵ�һ��

float q0 = 1, q1 = 0, q2 = 0, q3 = 0;          // ��Ԫ����Ԫ�أ�������Ʒ���
float exInt = 0, eyInt = 0, ezInt = 0;        // ��������С�������
float gx=0, gy=0, gz=0, ax=0, ay=0, az=0;

// �����˲�
// ʱ�䳣��   t=a/(1-a)*dt    a=t/(t+dt)         
// t ����Ƶ��  dt������ʱ��
#define        kfa   0.98f
#define        kfan  1.0f-kfa

//ang= kfa*ang+kfgn*acc;
#define        kfg   0.80f
#define        kfgn  1.0f-kfg
float Yaw,Pitch,Roll;  //ƫ���ǣ������ǣ�������

void getAngles(float *pitch, float *roll, float *yaw) {
    *pitch = Pitch;
    *roll = Roll;
    *yaw = Yaw;
}
//ang= kfa*g+kfgn*acc;

//====================================================================================================
// Function
//====================================================================================================
//gx ����ʱ�仡������ 
//������Ϊ3��������ݣ�����һ��
//���Ϊ4Ԫ��
void quaternionsUpdate(float gxi, float gyi, float gzi, float axi, float ayi, float azi) {
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;
    //���ӻ����˲�
    /*
    float q0q0 = q0*q0;
    float q0q1 = q0*q1;
    float q0q2 = q0*q2;
    float q0q3 = q0*q3;
    float q1q1 = q1*q1;
    float q1q2 = q1*q2;
    float q1q3 = q1*q3;
    float q2q2 = q2*q2;
    float q2q3 = q2*q3;
    float q3q3 = q3*q3;
    */
    ax=ax*kfa+kfan*axi;
    ay=ay*kfa+kfan*ayi;
    az=az*kfa+kfan*azi;

    gx=gx*kfg+kfgn*gxi;
    gy=gy*kfg+kfgn*gyi;
    gz=gz*kfg+kfgn*gzi;

    // ����������
    norm = sqrt(ax*ax + ay*ay + az*az);       
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;      
    
    // ���Ʒ��������
    vx = 2*(q1*q3 - q0*q2);
    vy = 2*(q0*q1 + q2*q3);
    vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
    
    // ���������ͷ��򴫸��������ο�����֮��Ľ���˻����ܺ�
    ex = (ay*vz - az*vy);
    ey = (az*vx - ax*vz);
    ez = (ax*vy - ay*vx);
    
    // ������������������
    exInt = exInt + ex*Ki;
    eyInt = eyInt + ey*Ki;
    ezInt = ezInt + ez*Ki;
    
    // ������������ǲ���
    gx = gx + Kp*ex + exInt;
    gy = gy + Kp*ey + eyInt;
    gz = gz + Kp*ez + ezInt;
    
    // ������Ԫ���ʺ�������
    q0 = q0 + (-q1*gx - q2*gy - q3*gz) * halfT;
    q1 = q1 + ( q0*gx + q2*gz - q3*gy) * halfT;
    q2 = q2 + ( q0*gy - q1*gz + q3*gx) * halfT;
    q3 = q3 + ( q0*gz + q1*gy - q2*gx) * halfT;  
    
    // ��������Ԫ
    norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 = q0 / norm;
    q1 = q1 / norm;
    q2 = q2 / norm;
    q3 = q3 / norm;
    
    //����õ�������/�����/�����
    Pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3;	// pitch
    Roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;	// roll
    Yaw   = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;	//yaw
}

