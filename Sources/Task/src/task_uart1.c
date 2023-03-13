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


/*************  功能说明    **************

本例程基于STC32G为主控芯片的实验箱9进行编写测试.

短接实验箱上的J7、J8跳线进行测试.

串口3定时发送一串数据给串口2.

串口2输出从串口3接收到的数据.

往串口2发送数据，MCU收到后从串口2原样输出.

******************************************/

#include "FreeRTOS.h"
#include "task.h"

#include "STC32G_UART.h"
#include "task_uart1.h"

extern void TX1_write1buff(uint8_t dat);

uint16_t Sec_Cnt;    //1秒计数

/* UART任务函数 */
portTASK_FUNCTION( vUart1Task, pvParameters )
{
    uint8_t i;
    
    Sec_Cnt = 0;

    PrintString1("STC32G UART1-UART3 Test Programme!\r\n");  //UART2发送一个字符串
    PrintString3("STC32G UART3-UART1 Test Programme!\r\n");  //UART3发送一个字符串
    while(1)
    {
        Sec_Cnt++;
        if(Sec_Cnt >= (uint16_t)pvParameters)
        {
            Sec_Cnt = 0;
            PrintString3("STC32G UART3-UART1 Test Programme!\r\n");  //UART3发送一个字符串
        }

        if(COM1.RX_TimeOut > 0)
        {
            if(--COM1.RX_TimeOut == 0)
            {
                if(COM1.RX_Cnt > 0)
                {
                    for(i=0; i<COM1.RX_Cnt; i++)    TX1_write1buff(RX1_Buffer[i]);    //把收到的数据通过串口1输出
                    COM1.RX_Cnt = 0;   //清除字节数
                }
            }
        }
        
        vTaskDelay(1);
    }
    
    vTaskDelete(NULL);
}   
