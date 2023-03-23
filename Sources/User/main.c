
/*
 * FreeRTOS STC port V1.0.2
 * 
 * STC32G12K128功能简介：
 *    2组 CAN, 3组Lin, USB, 32位 8051, RMB5,
 *    高精准12位ADC, DMA 支持(TFT彩屏, ADC, 4组串口, SPI, I2C )
 *  
 *  各位用户在后续的使用过程中，如果发现有不正确的地方或者
 *  有可改善的地方，请和STC联系，由STC官方进行统一修改并更新版本
 *
 *  若您的代码或文章中需要引用本程序包中的部分移植代码，请在您的
 *  代码或文章中注明参考或使用了STC的FreeRTOS的移植代码
*/

#include "FreeRTOS.h"
#include "task.h"
#include "System_init.h"

#include "display.h"
#include "rtc.h"
#include "ntc.h"
#include "adckey.h"
#include "uart2_3.h"
#include "MatrixKey.h"
#include "i2c_ps.h"
#include "pwmb.h"
//#include "ch395.h"
//#include "task_uart1.h"
#include <STC32G.H>
#include "usb.h"







		//P3.2口按键复位所需变量
		bit Key_Flag;
		u16 Key_cnt;


void KeyResetScan(void);

void vSystemInit( void );

//void timer4_int (void) interrupt 1 //1毫秒中断函数
//{
	//P3.2口按健触发软件复位，进入USB下载模式
	//KeyResetScan();
//}



void main( void )
{
	//USB调试及复位所需定义
		//char *USER_DEVICEDESC = NULL;
		//char *USER_PRODUCTDESC = NULL;
		//char *USER_STCISPCMD = "@STCISP#";//设置自动复位到ISP区的用户接口命令
		//USB调试及复位所需代码
		//P3M0 &= ~0x03;
		//P3M1 |=0x03;
		//IRC48MCR = 0x80;
		//while(!(IRC48MCR & 0x01));
		//usb_init();
		//EA=1; //打开总中断 
		//EUSB = 1;   //IE2相关的中断位操作使能后，需要重新设置EUSB
		//USB调试及复位所需代码，放在循环函数里
		//if(bUsbOutReady) {usb_OUT_done();}
	



	
    /* 系统初始化 */
    vSystemInit();
	
	  //InitCH395InfParam();  /* 初始化CH395相关变量 */
	
			





	/* 创建任务 */
	/*
    xTaskCreate((TaskFunction_t )vDisplayTask,
                (const char*    )"DISPLAY",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES + 1),
                (TaskHandle_t*  )NULL);
    xTaskCreate((TaskFunction_t )vRtcTask,
                (const char*    )"RTC",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES + 1),
                (TaskHandle_t*  )NULL);
    xTaskCreate((TaskFunction_t )vAdcKeyTask,
                (const char*    )"ADCKEY",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES),
                (TaskHandle_t*  )NULL);
    xTaskCreate((TaskFunction_t )vNtcTask,
                (const char*    )"NTC",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES),
                (TaskHandle_t*  )NULL);
    xTaskCreate((TaskFunction_t )vMatrixKeyTask,
                (const char*    )"MatrixKey",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES),
                (TaskHandle_t*  )NULL);
    xTaskCreate((TaskFunction_t )vI2C_PsTask,
                (const char*    )"I2C_PS",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES),
                (TaskHandle_t*  )NULL);
    xTaskCreate((TaskFunction_t )vPWMB_LedTask,
                (const char*    )"PWMB_LED",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES),
                (TaskHandle_t*  )NULL);
								*/

								
		    xTaskCreate((TaskFunction_t )vUart2_3Task,
                (const char*    )"UART2_3",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )1000,  //注意pvParameters参数传地址或者立即数时只有低24位是有效位，最高8位编译时会自动填0. 可通过变量或者常量传送32字节数据.
                (UBaseType_t    )(configDEFAULT_PRIORITIES),
                (TaskHandle_t*  )NULL);
								
								
								/*
	  xTaskCreate((TaskFunction_t )vUart1Task,
                (const char*    )"UART1",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )1000,  //注意pvParameters参数传地址或者立即数时只有低24位是有效位，最高8位编译时会自动填0. 可通过变量或者常量传送32字节数据.
                (UBaseType_t    )(configDEFAULT_PRIORITIES),
                (TaskHandle_t*  )NULL);
								*/

    /* 启动任务调度器开始任务调度 */
    vTaskStartScheduler();

    /* 正常情况下不会运行到此处 */
    while (1);
}





void KeyResetScan(void)
{
		if(!P32)
		{
			if (!Key_Flag) {
				Key_cnt++;
				//连续1000ms有效按键检测
				if (Key_cnt >= 1000)
				{
					Key_Flag = 1;	//设置按键状态，防止重复触发


					USBCON = 0x00;  	//清除USB设置
					USBCLK = 0x00;
					 IRC48MCR = 0x00;


					//delay_ms(10);
					vTaskDelay(10);
					IAP_CONTR= 0x60; 	//触发软件复位，从ISP开始执行
					while (1);
				}
			}
		}
		else
		{
			Key_cnt = 0; 
			Key_Flag=0;
		}
}



