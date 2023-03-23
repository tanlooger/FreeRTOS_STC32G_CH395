/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/


/*************  ����˵��    **************

�����̻���STC32GתSTC89ϵ��ת�Ӱ壨�����������б�д���ԡ�

�߼�PWM��ʱ�� PWM1P/PWM1N,PWM2P/PWM2N,PWM3P/PWM3N,PWM4P/PWM4N ÿ��ͨ�����ɶ���ʵ��PWM������������������Գ����.

8��ͨ��PWM���ö�ӦP1��8���˿ڣ�PWM2P��ӦP5.4�ڣ���Ҫ�̽�JP1�������PWM��LED�ϣ�.

ͨ��P1�������ӵ�8��LED�ƣ�����PWMʵ�ֺ�����Ч��.

PWM���ں�ռ�ձȿ��Ը�����Ҫ�������ã���߿ɴ�65535.

���������ʾ���ָ�λ����USB����ģʽ�ķ�����
1. ͨ��ÿ1����ִ��һ�Ρ�KeyResetScan��������ʵ�ֳ���P3.2�ڰ�������MCU��λ������USB����ģʽ��
2. ͨ�����ء�stc_usb_hid_32g.lib���⺯����ʵ��ʹ��STC-ISP�������ָ���MCU��λ������USB����ģʽ���Զ����ء�

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

******************************************/

#include "../comm/STC32G.h"  //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�
#include "../comm/usb.h"     //USB���Լ���λ����ͷ�ļ�
#include "intrins.h"

/****************************** �û������ ***********************************/

#define MAIN_Fosc       24000000L   //������ʱ��
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

/*****************************************************************************/

#define PWM1_1      0x00	//P:P1.0  N:P1.1
#define PWM1_2      0x01	//P:P2.0  N:P2.1
#define PWM1_3      0x02	//P:P6.0  N:P6.1

#define PWM2_1      0x00	//P:P1.2/P5.4  N:P1.3
#define PWM2_2      0x04	//P:P2.2  N:P2.3
#define PWM2_3      0x08	//P:P6.2  N:P6.3

#define PWM3_1      0x00	//P:P1.4  N:P1.5
#define PWM3_2      0x10	//P:P2.4  N:P2.5
#define PWM3_3      0x20	//P:P6.4  N:P6.5

#define PWM4_1      0x00	//P:P1.6  N:P1.7
#define PWM4_2      0x40	//P:P2.6  N:P2.7
#define PWM4_3      0x80	//P:P6.6  N:P6.7
#define PWM4_4      0xC0	//P:P3.4  N:P3.3

#define ENO1P       0x01
#define ENO1N       0x02
#define ENO2P       0x04
#define ENO2N       0x08
#define ENO3P       0x10
#define ENO3N       0x20
#define ENO4P       0x40
#define ENO4N       0x80

#define PWM_PERIOD  1023    //��������ֵ

/*************  ���ر�������    **************/
bit B_1ms;          //1ms��־

u16 PWM1_Duty;
u16 PWM2_Duty;
u16 PWM3_Duty;
u16 PWM4_Duty;

bit PWM1_Flag;
bit PWM2_Flag;
bit PWM3_Flag;
bit PWM4_Flag;

//USB���Լ���λ���趨��
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //�����Զ���λ��ISP�����û��ӿ�����

//P3.2�ڰ�����λ�������
bit Key_Flag;
u16 Key_cnt;

void UpdatePwm(void);
void KeyResetScan(void);

/******************** ������ **************************/
void main(void)
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

    PWM1_Flag = 0;
    PWM2_Flag = 0;
    PWM3_Flag = 0;
    PWM4_Flag = 0;

    PWM1_Duty = 0;
    PWM2_Duty = 256;
    PWM3_Duty = 512;
    PWM4_Duty = 1024;

    //USB���Լ���λ�������-----
    P3M0 &= ~0x03;
    P3M1 |= 0x03;
    IRC48MCR = 0x80;
    while (!(IRC48MCR & 0x01));
    usb_init();
    //-------------------------

    //  Timer0��ʼ��
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    PWMA_CCER1 = 0x00; //д CCMRx ǰ���������� CCxE �ر�ͨ��
    PWMA_CCER2 = 0x00;
    PWMA_CCMR1 = 0x60; //ͨ��ģʽ����
    PWMA_CCMR2 = 0x60;
    PWMA_CCMR3 = 0x60;
    PWMA_CCMR4 = 0x60;
    PWMA_CCER1 = 0x55; //����ͨ�����ʹ�ܺͼ���
    PWMA_CCER2 = 0x55;

    PWMA_ARRH = (u8)(PWM_PERIOD >> 8); //��������ʱ��
    PWMA_ARRL = (u8)PWM_PERIOD;

    PWMA_ENO = 0x00;
    PWMA_ENO |= ENO1P; //ʹ�����
    PWMA_ENO |= ENO1N; //ʹ�����
    PWMA_ENO |= ENO2P; //ʹ�����
    PWMA_ENO |= ENO2N; //ʹ�����
    PWMA_ENO |= ENO3P; //ʹ�����
    PWMA_ENO |= ENO3N; //ʹ�����
    PWMA_ENO |= ENO4P; //ʹ�����
    PWMA_ENO |= ENO4N; //ʹ�����

    PWMA_PS = 0x00;  //�߼� PWM ͨ�������ѡ��λ
    PWMA_PS |= PWM1_1; //ѡ�� PWM1_1 ͨ��
    PWMA_PS |= PWM2_1; //ѡ�� PWM2_1 ͨ��
    PWMA_PS |= PWM3_1; //ѡ�� PWM3_1 ͨ��
    PWMA_PS |= PWM4_1; //ѡ�� PWM4_1 ͨ��

    PWMA_BKR = 0x80; //ʹ�������
    PWMA_CR1 |= 0x01; //��ʼ��ʱ

    EUSB = 1;   //IE2��ص��ж�λ����ʹ�ܺ���Ҫ��������EUSB
    EA = 1;     //�����ж�

    while (1)
    {
        if (bUsbOutReady) //USB���Լ���λ�������
        {
//            memcpy(UsbInBuffer, UsbOutBuffer, 64);      //ԭ·����, ���ڲ���HID
//            usb_IN();
            
            usb_OUT_done();
        }
    }
}


/********************** Timer0 1ms�жϺ��� ************************/
void timer0(void) interrupt 1
{
    if(!PWM1_Flag)
    {
        PWM1_Duty++;
        if(PWM1_Duty > PWM_PERIOD) PWM1_Flag = 1;
    }
    else
    {
        PWM1_Duty--;
        if(PWM1_Duty <= 0) PWM1_Flag = 0;
    }

    if(!PWM2_Flag)
    {
        PWM2_Duty++;
        if(PWM2_Duty > PWM_PERIOD) PWM2_Flag = 1;
    }
    else
    {
        PWM2_Duty--;
        if(PWM2_Duty <= 0) PWM2_Flag = 0;
    }

    if(!PWM3_Flag)
    {
        PWM3_Duty++;
        if(PWM3_Duty > PWM_PERIOD) PWM3_Flag = 1;
    }
    else
    {
        PWM3_Duty--;
        if(PWM3_Duty <= 0) PWM3_Flag = 0;
    }

    if(!PWM4_Flag)
    {
        PWM4_Duty++;
        if(PWM4_Duty > PWM_PERIOD) PWM4_Flag = 1;
    }
    else
    {
        PWM4_Duty--;
        if(PWM4_Duty <= 0) PWM4_Flag = 0;
    }
    
    UpdatePwm();
    KeyResetScan();   //P3.2�ڰ������������λ������USB����ģʽ������Ҫ�˹��ܿ�ɾ�����д���
}


//========================================================================
// ����: UpdatePwm(void)
// ����: ����PWMռ�ձ�. 
// ����: none.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void UpdatePwm(void)
{
    PWMA_CCR1H = (u8)(PWM1_Duty >> 8); //����ռ�ձ�ʱ��
    PWMA_CCR1L = (u8)(PWM1_Duty);
    PWMA_CCR2H = (u8)(PWM2_Duty >> 8); //����ռ�ձ�ʱ��
    PWMA_CCR2L = (u8)(PWM2_Duty);
    PWMA_CCR3H = (u8)(PWM3_Duty >> 8); //����ռ�ձ�ʱ��
    PWMA_CCR3L = (u8)(PWM3_Duty);
    PWMA_CCR4H = (u8)(PWM4_Duty >> 8); //����ռ�ձ�ʱ��
    PWMA_CCR4L = (u8)(PWM4_Duty);
}

//========================================================================
// ����: void delay_ms(u8 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-3
// ��ע: 
//========================================================================
void delay_ms(u8 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);   //6T per loop
    }while(--ms);
}

//========================================================================
// ����: void KeyResetScan(void)
// ����: P3.2�ڰ�������1�봥�������λ������USB����ģʽ��
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-11
// ��ע: 
//========================================================================
void KeyResetScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 1000)		//����1000ms��Ч�������
            {
                Key_Flag = 1;		//���ð���״̬����ֹ�ظ�����

                USBCON = 0x00;      //���USB����
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //���������λ����ISP��ʼִ��
                while (1);
            }
        }
    }
    else
    {
        Key_cnt = 0;
        Key_Flag = 0;
    }
}
