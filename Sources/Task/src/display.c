#include "FreeRTOS.h"
#include "task.h"

#include "display.h"

static void prvDisplayInit( void );
static void prvDisplayUpdate( void );

static uint8_t ucDisplayIndex;                          //显示位索引
uint8_t pucLEDBuffer[8];                                //显示缓冲


		//P3.2口按键复位所需变量
		bit Key_Flag;
		uint16_t Key_cnt;


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



/* 显示任务函数 */
portTASK_FUNCTION( vDisplayTask, pvParameters )
{
	UNUSED( pvParameters );
    
    prvDisplayInit();
    while(1)
    {
        prvDisplayUpdate();
			
			//P3.2口按健触发软件复位，进入USB下载模式
			KeyResetScan();
        
        vTaskDelay(2);
    }
    
    vTaskDelete(NULL);
}   

static void prvDisplayInit( void )
{
    uint8_t index;
    
    for (index = 0; index < 8; index++)
        pucLEDBuffer[index] = DIS_BLACK;
    
    ucDisplayIndex = 0;
}

static void prvDisplayUpdate( void )
{
    static uint8_t code pucBITMAP[] =                   //位码
    {
        0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80
    };
    static uint8_t code pucDISPLAY[] =                  //标准字库
    {
    //   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
        0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
    //black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
        0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    //   0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   -1
        0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46
    };
    
	P6 = 0x00;                                          //防止鬼影
	P7 = ~pucBITMAP[7-ucDisplayIndex];
	P6 = ~pucDISPLAY[pucLEDBuffer[ucDisplayIndex]];
    
    if(++ucDisplayIndex >= 8)
        ucDisplayIndex = 0;                             //8位结束回0
}

