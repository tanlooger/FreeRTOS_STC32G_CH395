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

�ϵ�ͨ�������ڲ�15ͨ��ADC�������ⲿ��Դ��ѹ����Դ��ѹ�� 4.7V~5.3V ֮����������֮����

����LED�Ƽ��һ��������˸��������ڵ�LED��ͬʱ��/��˵������֮���ж�·��

����4�򴮿�3�����ۼ����ݣ�����3�յ����뷢��ֵ�Ƚϣ���ͬ�Ļ�����

����ʱ����LED��ͬʱ������/��

ע�⣺
1. P5.4��LED��Ҫ�̽� JP1 ��������
2. ����3������4ͨ�Ų�����Ҫ���� R52 ���衣

���������ʾ���ָ�λ����USB����ģʽ�ķ�����
1. ͨ��ÿ1����ִ��һ�Ρ�KeyResetScan��������ʵ�ֳ���P3.2�ڰ�������MCU��λ������USB����ģʽ��
2. ͨ�����ء�stc_usb_hid_32g.lib���⺯����ʵ��ʹ��STC-ISP�������ָ���MCU��λ������USB����ģʽ���Զ����ء�

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��)��

******************************************/

#include "../comm/STC32G.h"  //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�
#include "../comm/usb.h"     //USB���Լ���λ����ͷ�ļ�
#include "intrins.h"

/****************************** �û������ ***********************************/

#define MAIN_Fosc       24000000L   //������ʱ��
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

#define Baudrate3   115200UL
#define Baudrate4   115200UL

#define UART3_BUF_LENGTH    64
#define UART4_BUF_LENGTH    64

/*****************************************************************************/

u16 Bandgap;
u16 vcc;

u8  TX_Data;    //��������

u8  TX3_Cnt;    //���ͼ���
u8  RX3_Cnt;    //���ռ���
u8  TX4_Cnt;    //���ͼ���
u8  RX4_Cnt;    //���ռ���
bit B_Err_Flag; //�����־
bit B_TX3_Busy; //����æ��־
bit B_TX4_Busy; //����æ��־
u8 RX3_TimeOut;
u8 RX4_TimeOut;

u8  xdata RX3_Buffer[UART3_BUF_LENGTH]; //���ջ���
u8  xdata RX4_Buffer[UART4_BUF_LENGTH]; //���ջ���

//USB���Լ���λ���趨��
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //�����Զ���λ��ISP�����û��ӿ�����

//P3.2�ڰ�����λ�������
bit Key_Flag;
u16 Key_cnt;

void Timer0_Init(void);
void ADC_Init(void);
u16  Get_ADC12bitResult(u8 channel); //channel = 0~15
void delay_ms(u8 ms);
void TestError(void);
void TestIO(void);
void UART3_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer3��������.
void UART4_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer4��������.
void UART3_TxByte(u8 dat);
void UART4_TxByte(u8 dat);
void KeyResetScan(void);

/******************** ������ **************************/
void main(void)
{
    u8  i;
    u16 j;
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
    ADC_Init();
    UART3_config(3);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer3��������.
    UART4_config(4);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer4��������.
    EUSB = 1;   //IE2��ص��ж�ʹ�ܺ���Ҫ��������EUSB
    EA = 1;     //�����ж�

    delay_ms(2);
    Bandgap = (CHIPID7 << 8) + CHIPID8;
    B_Err_Flag = 0;
    
    Get_ADC12bitResult(15);  //�ȶ�һ�β��������, ���ڲ��Ĳ������ݵĵ�ѹ��������ֵ.
    Get_ADC12bitResult(15);  //�ȶ�һ�β��������, ���ڲ��Ĳ������ݵĵ�ѹ��������ֵ.
    for(j=0, i=0; i<16; i++)
    {
        j += Get_ADC12bitResult(15); //���ڲ���׼ADC, ��15ͨ��
    }
    j >>= 4;   //16��ƽ��

    vcc = (int)(4096L * Bandgap / j);            //(12λADC�㷨)����VREF�ܽŵ�ѹ,����ص�ѹ
    
    if((vcc < 4700) || (vcc > 5300))
    {
//        P0 = vcc >> 8;  //IO ����ʾ�������
//        P2 = vcc;
//        while(1);
        TestError();
    }
    else
    {
        while(1)
        {
            TestIO();
            UART4_TxByte(++TX_Data);  //���ͼ���ֵ������3
            if(B_Err_Flag)
            {
                TestError();
            }
        }
    }
}

/********************** Timer0 1ms�жϺ��� ************************/
void timer0(void) interrupt 1
{
    if(RX3_TimeOut > 0)     //��ʱ����
    {
        if(--RX3_TimeOut == 0)
        {
            if(RX3_Buffer[0] != TX_Data) B_Err_Flag = 1;  //�жϴ���3�յ��������Ƿ���ȷ
            RX3_Cnt  = 0;   //����ֽ���
        }
    }

    if (bUsbOutReady) //USB���Լ���λ�������
    {
//        memcpy(UsbInBuffer, UsbOutBuffer, 64);      //ԭ·����, ���ڲ���HID
//        usb_IN();
        
        usb_OUT_done();
    }

    KeyResetScan();   //P3.2�ڰ������������λ������USB����ģʽ������Ҫ�˹��ܿ�ɾ�����д���
}

//========================================================================
// ����: void TestError(void)
// ����: ��ʱ������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-2
// ��ע: 
//========================================================================
void TestError(void)
{
    while(1)
    {
        P0 = 0;		//LED On
        P1 = 0;		//LED On
        P2 = 0;		//LED On
        P3 = 0;		//LED On
        P4 = 0;		//LED On
        P5 = 0;		//LED On
        P6 = 0;		//LED On
        P7 = 0;		//LED On
        delay_ms(250);
        P0 = 0xff;	//LED Off
        P1 = 0xff;	//LED Off
        P2 = 0xff;	//LED Off
        P3 = 0xff;	//LED Off
        P4 = 0xff;	//LED Off
        P5 = 0xff;	//LED Off
        P6 = 0xff;	//LED Off
        P7 = 0xff;	//LED Off
        delay_ms(250);
    }
}

//========================================================================
// ����: void TestIO(void)
// ����: ��ʱ������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-2
// ��ע: 
//========================================================================
void TestIO(void)
{
    P0 = 0x55;	//LED On
    P1 = 0x55;	//LED On
    P3 = 0x55;	//LED On
    P2 = 0xaa;	//LED Off
    P52 = 1;	//LED Off
    P45 = 1;	//LED Off
    P46 = 1;	//LED Off
    P47 = 0;	//LED On
    P54 = 0;	//LED On

    delay_ms(250);
    delay_ms(250);
    P0 = 0xaa;	//LED Off
    P1 = 0xaa;	//LED Off
    P3 = 0xaa;	//LED Off
    P2 = 0x55;	//LED On
    P52 = 0;	//LED On
    P45 = 0;	//LED On
    P46 = 0;	//LED On
    P47 = 1;	//LED Off
    P54 = 1;	//LED Off

    delay_ms(250);
    delay_ms(250);
    delay_ms(250);
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
    //  Timer0��ʼ��
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
}

//========================================================================
// ����: void ADC_Init(void)
// ����: ADC��ʼ��������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-2
// ��ע: 
//========================================================================
void ADC_Init(void)
{
    ADCTIM = 0x3f;		//���� ADC �ڲ�ʱ��ADC����ʱ�佨�������ֵ
    ADCCFG = 0x2f;		//���� ADC ʱ��Ϊϵͳʱ��/2/16
    ADC_CONTR = 0x8f;   //ʹ�� ADC ģ��
}

//========================================================================
// ����: u16 Get_ADC12bitResult(u8 channel)
// ����: ��ѯ����һ��ADC���.
// ����: channel: ѡ��Ҫת����ADC.
// ����: 12λADC���.
// �汾: V1.0, 2012-10-22
//========================================================================
u16 Get_ADC12bitResult(u8 channel)  //channel = 0~15
{
    ADC_RES = 0;
    ADC_RESL = 0;

    ADC_CONTR = (ADC_CONTR & 0xf0) | channel; //����ADCת��ͨ��
    ADC_START = 1;//����ADCת��
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    while(ADC_FLAG == 0);   //wait for ADC finish
    ADC_FLAG = 0;     //���ADC������־
    return  (((u16)ADC_RES << 8) | ADC_RESL);
}

//========================================================================
// ����: void delay_ms(u8 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2021-3-9
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
// ����: void UART3_TxByte(u8 dat)
// ����: ����һ���ֽ�.
// ����: ��.
// ����: ��.
// �汾: V1.0, 2014-6-30
//========================================================================
//void UART3_TxByte(u8 dat)
//{
//    B_TX3_Busy = 1;
//    S3BUF = dat;
//    while(B_TX3_Busy);
//}

//========================================================================
// ����: void UART4_TxByte(u8 dat)
// ����: ����һ���ֽ�.
// ����: ��.
// ����: ��.
// �汾: V1.0, 2014-6-30
//========================================================================
void UART4_TxByte(u8 dat)
{
    B_TX4_Busy = 1;
    S4BUF = dat;
    while(B_TX4_Busy);
}

//========================================================================
// ����: void PrintString3(u8 *puts)
// ����: ����3�����ַ���������
// ����: puts:  �ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
//void PrintString3(u8 *puts)
//{
//    for (; *puts != 0;  puts++)     //����ֹͣ��0����
//    {
//        UART3_TxByte(*puts);
//    }
//}

//========================================================================
// ����: void PrintString4(u8 *puts)
// ����: ����4�����ַ���������
// ����: puts:  �ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
//void PrintString4(u8 *puts)
//{
//    for (; *puts != 0;  puts++)     //����ֹͣ��0����
//    {
//        UART4_TxByte(*puts);
//    }
//}

//========================================================================
// ����: SetTimer2Baudraye(u16 dat)
// ����: ����Timer2�������ʷ�������
// ����: dat: Timer2����װֵ.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void SetTimer2Baudraye(u16 dat)
{
    T2R = 0;		//Timer stop
    T2_CT = 0;	//Timer2 set As Timer
    T2x12 = 1;	//Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //��ֹ�ж�
    T2R = 1;		//Timer run enable
}

//========================================================================
// ����: void UART3_config(u8 brt)
// ����: UART3��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer3��������.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART3_config(u8 brt)    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer3��������.
{
    if(brt == 2)
    {
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate3));
        S3CON = 0x10;       //8λ����, ʹ��Timer2�������ʷ�����, �������
    }
    else
    {
        T3R = 0;		//Timer stop
        S3CON = 0x50;       //8λ����, ʹ��Timer3�������ʷ�����, �������
        T3H = (65536UL - (MAIN_Fosc / 4) / Baudrate3) / 256;
        T3L = (65536UL - (MAIN_Fosc / 4) / Baudrate3) % 256;
        T3_CT = 0;	//Timer3 set As Timer
        T3x12 = 1;	//Timer3 set as 1T mode
        T3R = 1;		//Timer run enable
    }
    ES3  = 1;       //����UART3�ж�
    S3_S = 1;       //UART3 switch bit1 to: 0: P0.0 P0.1,  1: P5.0 P5.1

    B_TX3_Busy = 0;
    TX3_Cnt = 0;
    RX3_Cnt = 0;
}

//========================================================================
// ����: void UART4_config(u8 brt)
// ����: UART4��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer4��������.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART4_config(u8 brt)    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer4��������.
{
    if(brt == 2)
    {
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate4));
        S4CON = 0x10;       //8λ����, ʹ��Timer2�������ʷ�����, �������
    }
    else
    {
        T4R = 0;	//Timer stop
        S4CON = 0x50;       //8λ����, ʹ��Timer4�������ʷ�����, �������
        T4H = (65536UL - (MAIN_Fosc / 4) / Baudrate4) / 256;
        T4L = (65536UL - (MAIN_Fosc / 4) / Baudrate4) % 256;
        T4_CT = 0;	//Timer3 set As Timer
        T4x12 = 1;	//Timer3 set as 1T mode
        T4R = 1;	//Timer run enable
    }
    ES4  = 1;       //����UART4�ж�
    S4_S = 0;       //UART4 switch bit2 to: 0: P0.2 P0.3, 1: P5.2 P5.3

    B_TX4_Busy = 0;
    TX4_Cnt = 0;
    RX4_Cnt = 0;
}

//========================================================================
// ����: void UART3_int (void) interrupt UART3_VECTOR
// ����: UART3�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART3_int (void) interrupt 17
{
    if(S3RI)
    {
        S3RI = 0;    //Clear Rx flag
        RX3_Buffer[RX3_Cnt] = S3BUF;
        if(++RX3_Cnt >= UART3_BUF_LENGTH)   RX3_Cnt = 0;
        RX3_TimeOut = 5;
    }

    if(S3TI)
    {
        S3TI = 0;   //Clear Tx flag
        B_TX3_Busy = 0;
    }
}

//========================================================================
// ����: void UART4_int (void) interrupt UART4_VECTOR
// ����: UART4�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART4_int (void) interrupt 18
{
    if(S4RI)
    {
        S4RI = 0;    //Clear Rx flag
        if(RX4_Cnt >= UART4_BUF_LENGTH)   RX4_Cnt = 0;
        RX4_Buffer[RX4_Cnt] = S2BUF;
        RX4_Cnt++;
        RX4_TimeOut = 5;
    }

    if(S4TI)
    {
        S4TI = 0;    //Clear Tx flag
        B_TX4_Busy = 0;
    }
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
