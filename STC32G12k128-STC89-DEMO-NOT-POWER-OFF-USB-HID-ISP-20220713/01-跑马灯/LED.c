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

#include "../comm/STC32G.h"  //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�
#include "../comm/usb.h"     //USB���Լ���λ����ͷ�ļ�

#define     MAIN_Fosc       24000000L   //������ʱ��

/*************  ����˵��    **************

�����̻���STC32GתSTC89ϵ��ת�Ӱ壨�����������б�д���ԡ�

������ʾ�����Ч���������������

ͨ�����á�VirtualDevice������ֵ�л���Ӳ����ʾ��ʽ��
VirtualDevice=0���������İ��ϵ�Ӳ��LED�ƣ�
VirtualDevice=1������STC-ISP������Խӿ������LED-DIP40�����豸����Ҫ��HID������򿪶�Ӧ���豸����

���������ʾ���ָ�λ����USB����ģʽ�ķ�����
1. ͨ��ÿ1����ִ��һ�Ρ�KeyResetScan��������ʵ�ֳ���P3.2�ڰ�������MCU��λ������USB����ģʽ��
2. ͨ�����ء�stc_usb_hid_32g.lib���⺯����ʵ��ʹ��STC-ISP�������ָ���MCU��λ������USB����ģʽ���Զ����ء�

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��)��

���û�ʹ��Ӳ�� USB �� STC32G ϵ�н��� ISP ����ʱ���ܵ����ڲ� IRC ��Ƶ�ʣ�
���û�����ѡ���ڲ�Ԥ�õ� 16 ��Ƶ��
���ֱ��� 5.5296M�� 6M�� 11.0592M�� 12M�� 18.432M�� 20M�� 22.1184M�� 
24M��27M�� 30M�� 33.1776M�� 35M����
����ʱ�û�ֻ�ܴ�Ƶ�������б��н���ѡ������֮һ���������ֶ���������Ƶ�ʡ�
��ʹ�ô���������������� 4M��35M ֮�������Ƶ�ʣ���

******************************************/

#define VirtualDevice        0    //0: ����Ӳ��LED;  1: �������Խӿ�LED-DIP40�����豸

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

//USB���Լ���λ���趨��
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //�����Զ���λ��ISP�����û��ӿ�����

//P3.2�ڰ�����λ�������
bit Key_Flag;
u16 Key_cnt;

u8 index=0;
u8 code LED_Buf[34][7]=
{
   //Mask  P0   P1   P2   P3   P4   P5
    {0x3f,0xff,0xfe,0xff,0xff,0xff,0xff},  /* P1.0 */
    {0x3f,0xff,0xfd,0xff,0xff,0xff,0xff},  /* P1.1 */
    {0x3f,0xff,0xff,0xff,0xff,0xff,0xfb},  /* P5.2 */
    {0x3f,0xff,0xf7,0xff,0xff,0xff,0xff},  /* P1.3 */
    {0x3f,0xff,0xef,0xff,0xff,0xff,0xff},  /* P1.4 */
    {0x3f,0xff,0xdf,0xff,0xff,0xff,0xff},  /* P1.5 */
    {0x3f,0xff,0xbf,0xff,0xff,0xff,0xff},  /* P1.6 */
    {0x3f,0xff,0x7f,0xff,0xff,0xff,0xff},  /* P1.7 */

    {0x3f,0xff,0xff,0xff,0xff,0xbf,0xff},  /* P4.6 */
    {0x3f,0xff,0xff,0xff,0xff,0x7f,0xff},  /* P4.7 */

    {0x3f,0xff,0xff,0xff,0xfb,0xff,0xff},  /* P3.2 */
    {0x3f,0xff,0xff,0xff,0xf7,0xff,0xff},  /* P3.3 */
    {0x3f,0xff,0xff,0xff,0xef,0xff,0xff},  /* P3.4 */
    {0x3f,0xff,0xff,0xff,0xdf,0xff,0xff},  /* P3.5 */
    {0x3f,0xff,0xff,0xff,0xbf,0xff,0xff},  /* P3.6 */
    {0x3f,0xff,0xff,0xff,0x7f,0xff,0xff},  /* P3.7 */

    {0x3f,0xff,0xff,0xfe,0xff,0xff,0xff},  /* P2.0 */
    {0x3f,0xff,0xff,0xfd,0xff,0xff,0xff},  /* P2.1 */
    {0x3f,0xff,0xff,0xfb,0xff,0xff,0xff},  /* P2.2 */
    {0x3f,0xff,0xff,0xf7,0xff,0xff,0xff},  /* P2.3 */
    {0x3f,0xff,0xff,0xef,0xff,0xff,0xff},  /* P2.4 */
    {0x3f,0xff,0xff,0xdf,0xff,0xff,0xff},  /* P2.5 */
    {0x3f,0xff,0xff,0xbf,0xff,0xff,0xff},  /* P2.6 */
    {0x3f,0xff,0xff,0x7f,0xff,0xff,0xff},  /* P2.7 */

    {0x3f,0xff,0xff,0xff,0xff,0xff,0xef},  /* P5.4 */
    {0x3f,0xff,0xff,0xff,0xff,0xdf,0xff},  /* P4.5 */

    {0x3f,0x7f,0xff,0xff,0xff,0xff,0xff},  /* P0.7 */
    {0x3f,0xbf,0xff,0xff,0xff,0xff,0xff},  /* P0.6 */
    {0x3f,0xdf,0xff,0xff,0xff,0xff,0xff},  /* P0.5 */
    {0x3f,0xef,0xff,0xff,0xff,0xff,0xff},  /* P0.4 */
    {0x3f,0xf7,0xff,0xff,0xff,0xff,0xff},  /* P0.3 */
    {0x3f,0xfb,0xff,0xff,0xff,0xff,0xff},  /* P0.2 */
    {0x3f,0xfd,0xff,0xff,0xff,0xff,0xff},  /* P0.1 */
    {0x3f,0xfe,0xff,0xff,0xff,0xff,0xff},  /* P0.0 */
};

void Timer0_Init(void);
void delay_ms(u8 ms);
void HardwareMarquee(void);
void SoftwareMarquee(void);

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

    //USB���Լ���λ�������-----
    P3M0 &= ~0x03;
    P3M1 |= 0x03;
    IRC48MCR = 0x80;
    while (!(IRC48MCR & 0x01));
    usb_init();
    //-------------------------

    Timer0_Init();
    EUSB = 1;   //IE2��ص��ж�λ����ʹ�ܺ���Ҫ��������EUSB
    EA = 1;     //�����ж�
    
    while(1)
    {
        #if(VirtualDevice)
        SoftwareMarquee();
        #else
        HardwareMarquee();
        #endif
    }
}

void SoftwareMarquee(void)
{
    LED40_SendData(LED_Buf[index++], 7);            //����DIP40�ĸ����ܽ���LED��״̬
    if(index >= 34) index = 0;

    delay_ms(250);
}

void HardwareMarquee(void)
{
    P10 = 0;		//LED On
    delay_ms(250);
    P10 = 1;		//LED Off
    P11 = 0;		//LED On
    delay_ms(250);
    P11 = 1;		//LED Off
    P52 = 0;		//LED On
    delay_ms(250);
    P52 = 1;		//LED Off
    P13 = 0;		//LED On
    delay_ms(250);
    P13 = 1;		//LED Off
    P14 = 0;		//LED On
    delay_ms(250);
    P14 = 1;		//LED Off
    P15 = 0;		//LED On
    delay_ms(250);
    P15 = 1;		//LED Off
    P16 = 0;		//LED On
    delay_ms(250);
    P16 = 1;		//LED Off
    P17 = 0;		//LED On
    delay_ms(250);
    P17 = 1;		//LED Off

    P46 = 0;		//LED On
    delay_ms(250);
    P46 = 1;		//LED Off
    P47 = 0;		//LED On
    delay_ms(250);
    P47 = 1;		//LED Off
    P32 = 0;		//LED On
    delay_ms(250);
    P32 = 1;		//LED Off
    P33 = 0;		//LED On
    delay_ms(250);
    P33 = 1;		//LED Off
    P34 = 0;		//LED On
    delay_ms(250);
    P34 = 1;		//LED Off
    P35 = 0;		//LED On
    delay_ms(250);
    P35 = 1;		//LED Off
    P36 = 0;		//LED On
    delay_ms(250);
    P36 = 1;		//LED Off
    P37 = 0;		//LED On
    delay_ms(250);
    P37 = 1;		//LED Off

    P20 = 0;		//LED On
    delay_ms(250);
    P20 = 1;		//LED Off
    P21 = 0;		//LED On
    delay_ms(250);
    P21 = 1;		//LED Off
    P22 = 0;		//LED On
    delay_ms(250);
    P22 = 1;		//LED Off
    P23 = 0;		//LED On
    delay_ms(250);
    P23 = 1;		//LED Off
    P24 = 0;		//LED On
    delay_ms(250);
    P24 = 1;		//LED Off
    P25 = 0;		//LED On
    delay_ms(250);
    P25 = 1;		//LED Off
    P26 = 0;		//LED On
    delay_ms(250);
    P26 = 1;		//LED Off
    P27 = 0;		//LED On
    delay_ms(250);
    P27 = 1;		//LED Off

    P54 = 0;		//LED On (��Ҫ�̽�JP1)
    delay_ms(250);
    P54 = 1;		//LED Off
    P45 = 0;		//LED On
    delay_ms(250);
    P45 = 1;		//LED Off

    P07 = 0;		//LED On
    delay_ms(250);
    P07 = 1;		//LED Off
    P06 = 0;		//LED On
    delay_ms(250);
    P06 = 1;		//LED Off
    P05 = 0;		//LED On
    delay_ms(250);
    P05 = 1;		//LED Off
    P04 = 0;		//LED On
    delay_ms(250);
    P04 = 1;		//LED Off
    P03 = 0;		//LED On
    delay_ms(250);
    P03 = 1;		//LED Off
    P02 = 0;		//LED On
    delay_ms(250);
    P02 = 1;		//LED Off
    P01 = 0;		//LED On
    delay_ms(250);
    P01 = 1;		//LED Off
    P00 = 0;		//LED On
    delay_ms(250);
    P00 = 1;		//LED Off
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
// ����: void Timer0_Init(void)
// ����: ��ʱ��0��ʼ��������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-2
// ��ע: 
//========================================================================
void Timer0_Init(void)
{
    // Timer0��ʼ��
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
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

//========================================================================
// ����: void timer0_int(void)
// ����: Timer0 1ms�жϺ�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-11
// ��ע: 
//========================================================================
void timer0_int(void) interrupt 1  //1ms �жϺ���
{
    if (bUsbOutReady) //USB���Լ���λ�������
    {
//        memcpy(UsbInBuffer, UsbOutBuffer, 64);      //ԭ·����, ���ڲ���HID
//        usb_IN();
        
        usb_OUT_done();
    }

    KeyResetScan();   //P3.2�ڰ������������λ������USB����ģʽ������Ҫ�˹��ܿ�ɾ�����д���
}

