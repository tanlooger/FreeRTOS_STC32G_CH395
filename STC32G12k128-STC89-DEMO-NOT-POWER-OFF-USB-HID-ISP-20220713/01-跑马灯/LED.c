/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include "../comm/STC32G.h"  //包含此头文件后，不需要再包含"reg51.h"头文件
#include "../comm/usb.h"     //USB调试及复位所需头文件

#define     MAIN_Fosc       24000000L   //定义主时钟

/*************  功能说明    **************

本例程基于STC32G转STC89系列转接板（降龙棍）进行编写测试。

程序演示跑马灯效果，输出低驱动。

通过设置“VirtualDevice”定义值切换软硬件显示方式：
VirtualDevice=0：驱动核心板上的硬件LED灯；
VirtualDevice=1：驱动STC-ISP软件调试接口里面的LED-DIP40虚拟设备（需要在HID助手里打开对应的设备）。

此外程序演示两种复位进入USB下载模式的方法：
1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
2. 通过加载“stc_usb_hid_32g.lib”库函数，实现使用STC-ISP软件发送指令触发MCU复位，进入USB下载模式并自动下载。

下载时, 选择时钟 24MHZ (用户可自行修改频率)。

当用户使用硬件 USB 对 STC32G 系列进行 ISP 下载时不能调节内部 IRC 的频率，
但用户可用选择内部预置的 16 个频率
（分别是 5.5296M、 6M、 11.0592M、 12M、 18.432M、 20M、 22.1184M、 
24M、27M、 30M、 33.1776M、 35M）。
下载时用户只能从频率下拉列表中进行选择其中之一，而不能手动输入其他频率。
（使用串口下载则可用输入 4M～35M 之间的任意频率）。

******************************************/

#define VirtualDevice        0    //0: 驱动硬件LED;  1: 驱动调试接口LED-DIP40虚拟设备

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
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

/******************** 主函数 **************************/
void main(void)
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    //USB调试及复位所需代码-----
    P3M0 &= ~0x03;
    P3M1 |= 0x03;
    IRC48MCR = 0x80;
    while (!(IRC48MCR & 0x01));
    usb_init();
    //-------------------------

    Timer0_Init();
    EUSB = 1;   //IE2相关的中断位操作使能后，需要重新设置EUSB
    EA = 1;     //打开总中断
    
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
    LED40_SendData(LED_Buf[index++], 7);            //控制DIP40的各个管脚上LED的状态
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

    P54 = 0;		//LED On (需要短接JP1)
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
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-3
// 备注: 
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
// 函数: void Timer0_Init(void)
// 描述: 定时器0初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-2
// 备注: 
//========================================================================
void Timer0_Init(void)
{
    // Timer0初始化
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
}

//========================================================================
// 函数: void KeyResetScan(void)
// 描述: P3.2口按键长按1秒触发软件复位，进入USB下载模式。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void KeyResetScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 1000)		//连续1000ms有效按键检测
            {
                Key_Flag = 1;		//设置按键状态，防止重复触发

                USBCON = 0x00;      //清除USB设置
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //触发软件复位，从ISP开始执行
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
// 函数: void timer0_int(void)
// 描述: Timer0 1ms中断函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void timer0_int(void) interrupt 1  //1ms 中断函数
{
    if (bUsbOutReady) //USB调试及复位所需代码
    {
//        memcpy(UsbInBuffer, UsbOutBuffer, 64);      //原路返回, 用于测试HID
//        usb_IN();
        
        usb_OUT_done();
    }

    KeyResetScan();   //P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码
}

