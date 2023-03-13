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

#ifndef	__STC32G_EEPROM_H
#define	__STC32G_EEPROM_H

#include	"FreeRTOS.h"

//========================================================================
//                              定义声明
//========================================================================


//========================================================================
//                               IAP设置
//========================================================================

#define		IAP_STANDBY()	IAP_CMD = 0		//IAP空闲命令（禁止）
#define		IAP_READ()		IAP_CMD = 1		//IAP读出命令
#define		IAP_WRITE()		IAP_CMD = 2		//IAP写入命令
#define		IAP_ERASE()		IAP_CMD = 3		//IAP擦除命令

#define	IAP_ENABLE()		IAPEN = 1; IAP_TPS = MAIN_Fosc / 1000000
#define	IAP_DISABLE()		IAP_CONTR = 0; IAP_CMD = 0; IAP_TRIG = 0; IAP_ADDRH = 0xff; IAP_ADDRL = 0xff


void	DisableEEPROM(void);
void 	EEPROM_read_n(uint32_t EE_address,uint8_t *DataAddress,uint16_t number);
void 	EEPROM_write_n(uint32_t EE_address,uint8_t *DataAddress,uint16_t number);
void	EEPROM_SectorErase(uint32_t EE_address);


#endif