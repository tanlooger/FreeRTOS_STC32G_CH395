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

�߼�PWM��ʱ�� PWM5,PWM6,PWM7,PWM8 ÿ��ͨ�����ɶ���ʵ��PWM���.

4��ͨ��PWM���ö�ӦP20~P23�����.

ͨ��P2�������ӵ�LED�ƣ�����PWMʵ�ֺ�����Ч��.

PWM���ں�ռ�ձȿ����Զ������ã���߿ɴ�65535.

���������ʾ���ָ�λ����USB����ģʽ�ķ�����
1. ͨ��ÿ1����ִ��һ�Ρ�KeyResetScan��������ʵ�ֳ���P3.2�ڰ�������MCU��λ������USB����ģʽ��
2. ͨ�����ء�stc_usb_hid_32g.lib���⺯����ʵ��ʹ��STC-ISP�������ָ���MCU��λ������USB����ģʽ���Զ����ء�

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

******************************************/

#include "../comm/STC32G.h"  //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�
#include "../comm/usb.h"     //USB���Լ���λ����ͷ�ļ�

/****************************** �û������ ***********************************/

#define MAIN_Fosc       24000000L   //������ʱ��
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

/*****************************************************************************/

#define PWM5_1      0x00	//P2.0
#define PWM5_2      0x01	//P1.7
#define PWM5_3      0x02	//P0.0
#define PWM5_4      0x03	//P7.4

#define PWM6_1      0x00	//P2.1
#define PWM6_2      0x04	//P5.4
#define PWM6_3      0x08	//P0.1
#define PWM6_4      0x0C	//P7.5

#define PWM7_1      0x00	//P2.2
#define PWM7_2      0x10	//P3.3
#define PWM7_3      0x20	//P0.2
#define PWM7_4      0x30	//P7.6

#define PWM8_1      0x00	//P2.3
#define PWM8_2      0x40	//P3.4
#define PWM8_3      0x80	//P0.3
#define PWM8_4      0xC0	//P7.7

#define ENO5P       0x01
#define ENO6P       0x04
#define ENO7P       0x10
#define ENO8P       0x40

#define PWM_PERIOD  1023    //��������ֵ

/***************************** ���ر������� **********************************/
bit B_1ms;          //1ms��־

u16 PWM5_Duty;
u16 PWM6_Duty;
u16 PWM7_Duty;
u16 PWM8_Duty;

bit PWM5_Flag;
bit PWM6_Flag;
bit PWM7_Flag;
bit PWM8_Flag;

//USB���Լ���λ���趨��
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //�����Զ���λ��ISP�����û��ӿ�����

//P3.2�ڰ�����λ�������
bit Key_Flag;
u16 Key_cnt;

void UpdatePwm(void);
void KeyResetScan(void);

/******************************** ������ ************************************/
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

    PWM5_Flag = 0;
    PWM6_Flag = 0;
    PWM7_Flag = 0;
    PWM8_Flag = 0;
	
    PWM5_Duty = 0;
    PWM6_Duty = 256;
    PWM7_Duty = 512;
    PWM8_Duty = 1024;

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

    PWMB_CCER1 = 0x00; //д CCMRx ǰ���������� CCxE �ر�ͨ��
    PWMB_CCER2 = 0x00;
    PWMB_CCMR1 = 0x60; //ͨ��ģʽ����
    PWMB_CCMR2 = 0x60;
    PWMB_CCMR3 = 0x60;
    PWMB_CCMR4 = 0x60;
    PWMB_CCER1 = 0x33; //����ͨ�����ʹ�ܺͼ���
    PWMB_CCER2 = 0x33;

    PWMB_ARRH = (u8)(PWM_PERIOD >> 8); //��������ʱ��
    PWMB_ARRL = (u8)PWM_PERIOD;

    PWMB_ENO = 0x00;
    PWMB_ENO |= ENO5P; //ʹ�����
    PWMB_ENO |= ENO6P; //ʹ�����
    PWMB_ENO |= ENO7P; //ʹ�����
    PWMB_ENO |= ENO8P; //ʹ�����

    PWMB_PS = 0x00;    //�߼� PWM ͨ�������ѡ��λ
    PWMB_PS |= PWM5_1; //ѡ�� PWM5_1 ͨ��
    PWMB_PS |= PWM6_1; //ѡ�� PWM6_1 ͨ��
    PWMB_PS |= PWM7_1; //ѡ�� PWM7_1 ͨ��
    PWMB_PS |= PWM8_1; //ѡ�� PWM8_1 ͨ��

    PWMB_BKR = 0x80;   //ʹ�������
    PWMB_CR1 |= 0x01;  //��ʼ��ʱ

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
    if(!PWM5_Flag)
    {
        PWM5_Duty++;
        if(PWM5_Duty > PWM_PERIOD) PWM5_Flag = 1;
    }
    else
    {
        PWM5_Duty--;
        if(PWM5_Duty <= 0) PWM5_Flag = 0;
    }

    if(!PWM6_Flag)
    {
        PWM6_Duty++;
        if(PWM6_Duty > PWM_PERIOD) PWM6_Flag = 1;
    }
    else
    {
        PWM6_Duty--;
        if(PWM6_Duty <= 0) PWM6_Flag = 0;
    }

    if(!PWM7_Flag)
    {
        PWM7_Duty++;
        if(PWM7_Duty > PWM_PERIOD) PWM7_Flag = 1;
    }
    else
    {
        PWM7_Duty--;
        if(PWM7_Duty <= 0) PWM7_Flag = 0;
    }

    if(!PWM8_Flag)
    {
        PWM8_Duty++;
        if(PWM8_Duty > PWM_PERIOD) PWM8_Flag = 1;
    }
    else
    {
        PWM8_Duty--;
        if(PWM8_Duty <= 0) PWM8_Flag = 0;
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
    PWMB_CCR5H = (u8)(PWM5_Duty >> 8); //����ռ�ձ�ʱ��
    PWMB_CCR5L = (u8)(PWM5_Duty);
    PWMB_CCR6H = (u8)(PWM6_Duty >> 8); //����ռ�ձ�ʱ��
    PWMB_CCR6L = (u8)(PWM6_Duty);
    PWMB_CCR7H = (u8)(PWM7_Duty >> 8); //����ռ�ձ�ʱ��
    PWMB_CCR7L = (u8)(PWM7_Duty);
    PWMB_CCR8H = (u8)(PWM8_Duty >> 8); //����ռ�ձ�ʱ��
    PWMB_CCR8L = (u8)(PWM8_Duty);
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
