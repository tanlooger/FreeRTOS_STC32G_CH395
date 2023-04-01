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

#ifndef __STC32G_UART_H
#define __STC32G_UART_H	 

#include	"FreeRTOS.h"

//========================================================================
//                              定义声明
//========================================================================

#define	UART1	1
#define	UART2	2
#define	UART3	3
#define	UART4	4

#define	UART_BUF_type	edata

#define	PRINTF_SELECT  UART3		//选择 printf 函数所使用的串口，参数 UART1~UART4

#ifdef UART1
#define	COM_TX1_Lenth	64
#define	COM_RX1_Lenth	64
#endif
#ifdef UART2
#define	COM_TX2_Lenth	64
#define	COM_RX2_Lenth	64
#endif
#ifdef UART3
#define	COM_TX3_Lenth	64
#define	COM_RX3_Lenth	64
#endif
#ifdef UART4
#define	COM_TX4_Lenth	64
#define	COM_RX4_Lenth	64
#endif

#define	UART_ShiftRight	0		//同步移位输出
#define	UART_8bit_BRTx	(1<<6)	//8位数据,可变波特率
#define	UART_9bit				(2<<6)	//9位数据,固定波特率
#define	UART_9bit_BRTx	(3<<6)	//9位数据,可变波特率


#define	TimeOutSet1		5
#define	TimeOutSet2		5
#define	TimeOutSet3		5
#define	TimeOutSet4		5

#define	BRT_Timer1	1
#define	BRT_Timer2	2
#define	BRT_Timer3	3
#define	BRT_Timer4	4

//========================================================================
//                              UART设置
//========================================================================

#define		UART1_RxEnable(n)		(n==0?(REN = 0):(REN = 1))				/* UART1接收使能 */
#define		UART2_RxEnable(n)		(n==0?(S2REN = 0):(S2REN = 1))		/* UART2接收使能 */
#define		UART3_RxEnable(n)		(n==0?(S3REN = 0):(S3REN = 1))		/* UART3接收使能 */
#define		UART4_RxEnable(n)		(n==0?(S4REN = 0):(S4REN = 1))		/* UART4接收使能 */


#define		CLR_TI2()			S2TI = 0			/* 清除TI2											 */
#define		CLR_RI2()			S2RI = 0			/* 清除RI2											 */
#define		CLR_TI3()			S3TI = 0			/* 清除TI3											 */
#define		CLR_RI3()			S3RI = 0			/* 清除RI3											 */
#define		CLR_TI4()			S4TI = 0			/* 清除TI3									     */
#define		CLR_RI4()			S4RI = 0			/* 清除RI3									     */

#define		S3_8bit()			S3SM0 = 0	/* 串口3模式0，8位UART，波特率 = 定时器的溢出率 / 4  */
#define		S3_9bit()			S3SM0 = 1	/* 串口3模式1，9位UART，波特率 = 定时器的溢出率 / 4  */
#define 	S3_BRT_UseTimer3()	S3ST3 = 1	/* BRT select Timer3								 */
#define 	S3_BRT_UseTimer2()	S3ST3 = 0	/* BRT select Timer2								 */

#define		S4_8bit()			S4SM0 = 0	/* 串口4模式0，8位UART，波特率 = 定时器的溢出率 / 4  */
#define		S4_9bit()			S4SM0 = 1	/* 串口4模式1，9位UART，波特率 = 定时器的溢出率 / 4  */
#define 	S4_BRT_UseTimer4()	S4ST4 = 1	/* BRT select Timer4								 */
#define 	S4_BRT_UseTimer2()	S4ST4 = 0	/* BRT select Timer2								 */

//========================================================================
//                              变量声明
//========================================================================

typedef struct
{ 
	uint8_t	id;				//串口号

	uint8_t	TX_send;		//已发送指针
	uint8_t	TX_write;		//发送写指针
	uint8_t	B_TX_busy;		//忙标志

	uint8_t RX_Cnt;			//接收字节计数
	uint8_t	RX_TimeOut;		//接收超时
	uint8_t	B_RX_OK;		//接收块完成
} COMx_Define; 

typedef struct
{ 
	uint8_t	UART_Mode;			//模式,         UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	uint8_t	UART_BRT_Use;		//使用波特率,   BRT_Timer1,BRT_Timer2,BRT_Timer3,BRT_Timer4
	uint32_t UART_BaudRate;		//波特率, 	   一般 110 ~ 115200
	uint8_t	Morecommunicate;	//多机通讯允许, ENABLE,DISABLE
	uint8_t	UART_RxEnable;		//允许接收,   ENABLE,DISABLE
	uint8_t	BaudRateDouble;		//波特率加倍, ENABLE,DISABLE
} COMx_InitDefine; 

#ifdef UART1
extern	COMx_Define	COM1;
extern	uint8_t	UART_BUF_type TX1_Buffer[COM_TX1_Lenth];	//发送缓冲
extern	uint8_t 	UART_BUF_type RX1_Buffer[COM_RX1_Lenth];	//接收缓冲
#endif
#ifdef UART2
extern	COMx_Define	COM2;
extern	uint8_t	UART_BUF_type TX2_Buffer[COM_TX2_Lenth];	//发送缓冲
extern	uint8_t 	UART_BUF_type RX2_Buffer[COM_RX2_Lenth];	//接收缓冲
#endif
#ifdef UART3
extern	COMx_Define	COM3;
extern	uint8_t	UART_BUF_type TX3_Buffer[COM_TX3_Lenth];	//发送缓冲
extern	uint8_t 	UART_BUF_type RX3_Buffer[COM_RX3_Lenth];	//接收缓冲
#endif
#ifdef UART4
extern	COMx_Define	COM4;
extern	uint8_t	UART_BUF_type TX4_Buffer[COM_TX4_Lenth];	//发送缓冲
extern	uint8_t 	UART_BUF_type RX4_Buffer[COM_RX4_Lenth];	//接收缓冲
#endif

uint8_t	UART_Configuration(uint8_t UARTx, COMx_InitDefine *COMx);
#ifdef UART1
//void TX1_write2buff(uint8_t dat);
void PrintString1(uint8_t *puts); 
#endif
#ifdef UART2
//void TX2_write2buff(uint8_t dat);
void PrintString2(uint8_t *puts);
#endif
#ifdef UART3
//void TX3_write2buff(uint8_t dat);
void PrintString3(uint8_t *puts);
#endif
#ifdef UART4
//void TX4_write2buff(uint8_t dat);
void PrintString4(uint8_t *puts);
#endif

//void COMx_write2buff(COMx_Define *COMx, uint8_t dat);
//void PrintString(COMx_Define *COMx, uint8_t *puts);

#endif

