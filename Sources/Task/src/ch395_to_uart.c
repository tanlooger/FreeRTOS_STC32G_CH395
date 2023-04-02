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
#include "ch395_to_uart.h"

#include "CH395INC.H"
#include "CH395.H"
#include "HTTPS.H"
#include "CH395CMD.H"
#include "semphr.h"

extern SemaphoreHandle_t BinarySemaphore;
extern void TX2_write2buff(uint8_t dat);

uint16_t Sec_Cnt;    //1秒计数

/* UART任务函数 */
portTASK_FUNCTION( vCh395ToUartTask, pvParameters )
{
    uint8_t i;
    Sec_Cnt = 0;
	restart:/* 延时100毫秒 */ 
	//CH395CMDReset();
	
	vTaskDelay(1000);
	
		
    //PrintString2("STC32G UART2-UART3 Test Programme!\r\n");  //UART2发送一个字符串
    PrintString3("STC32G UART3-UART2 Test Programme!\r\n");  //UART3发送一个字符串
	

	
	   http_request = (st_http_request*)RecvBuffer;
   printf("CH395EVT Test Demo\n");
   //CH395_PROT_INIT();
	
	
 
	vTaskDelay(1000);
	
	vTaskDelay(1000);
   InitCH395InfParam();
	 i = CH395CMDCheckExist(0x65); 
    printf("CH395CMDCheckExist = %2x\n",(UINT16)i);  
    if(i != 0x9a){
			goto restart;
		}
    CH395CMDSetIPAddr(CH395Inf.IPAddr);                              /* 设置CH395的IP地址 */
    CH395CMDSetGWIPAddr(CH395Inf.GWIPAddr);                          /* 设置网关地址 */
    CH395CMDSetMASKAddr(CH395Inf.MASKAddr);                          /* 设置子网掩码，默认为255.255.255.0*/   
    i = CH395CMDInitCH395();                                         /* 初始化CH395芯片 */
    printf("CH395CMDInitCH395 = %2x\n",(UINT16)i); 
   if (i != CMD_ERR_SUCCESS)
	 {
		 goto restart;
	 }
   while(1)
   {                                                                                   /* 等待以太网连接成功*/
       if(CH395CMDGetPHYStatus() == PHY_DISCONN)                                       /* 查询CH395是否连接 */
       {
           mDelaymS(200);                                                              /* 未连接则等待200MS后再次查询 */
       }
       else 
       {
           printf("CH395 Connect Ethernet\n");                                         /* CH395芯片连接到以太网，此时会产生中断 */
           break;
       }
   }
   InitSocketParam();                                                                  /* 初始化socket相关变量 */
   CH395SocketInitOpen();
	 i = CH395OpenSocket(0);                                          /* 打开socket 0 */
    if (i != CMD_ERR_SUCCESS)
	 {
		 printf("b.....\n");
		 goto restart;
	 }
    i = CH395TCPListen(0); // TCP连接                                          
	 if (i != CMD_ERR_SUCCESS)
	 {
		 printf("c.....\n");
		 goto restart;
	 }
   while(1)
   {
		 xSemaphoreTake(BinarySemaphore,portMAX_DELAY);
     //if(Query395Interrupt())
		 CH395GlobalInterrupt();                                    /*查询总中断*/
     WebServer();
     if((flag & IPCHANGE )|( flag & PORTCHANGE) )                                      /*IP或者Port改变则复位CH395并重新初始化*/
     {
       flag  &=~(IPCHANGE|PORTCHANGE) ;
       printf("reset all!\n");
       CH395CMDReset();
       for(i=0;i<15;i++) { vTaskDelay(200);}                                             /*大约延时3S*/
       printf("restart\n");
       goto restart;
     }
   }
	
    while(1)
    {
        Sec_Cnt++;
        if(Sec_Cnt >= (uint16_t)pvParameters)
        {
            Sec_Cnt = 0;
            PrintString3("STC32G UART3-UART2 Test Programme!\r\n");  //UART3发送一个字符串
        }

        if(COM2.RX_TimeOut > 0)
        {
            if(--COM2.RX_TimeOut == 0)
            {
                if(COM2.RX_Cnt > 0)
                {
									TX2_write2buff(RX2_Buffer[0]);
									TX2_write2buff(RX2_Buffer[1]);
									if(RX2_Buffer[0] == 'k'){
										TX2_write2buff(RX2_Buffer[0]);
										  COM2.RX_Cnt = 0;   //清除字节数
										goto restart;
									}
                    //for(i=0; i<COM2.RX_Cnt; i++)    TX2_write2buff(RX2_Buffer[i]);//把收到的数据通过串口2输出
                    COM2.RX_Cnt = 0;   //清除字节数
                }
            }
        }
        
        vTaskDelay(1);
    }
    
    vTaskDelete(NULL);
}   
