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

#include "STC32G_EEPROM.h"

//========================================================================
// 函数: void ISP_Disable(void)
// 描述: 禁止访问ISP/IAP.
// 参数: non.
// 返回: non.
// 版本: V1.0, 2012-10-22
//========================================================================
void DisableEEPROM(void)
{
	IAP_CONTR = 0;			//禁止IAP操作
	IAP_CMD   = 0;			//去除IAP命令
	IAP_TRIG  = 0;			//防止IAP命令误触发
	IAP_ADDRE = 0xff;   //将地址设置到非 IAP 区域
	IAP_ADDRH = 0xff;   //将地址设置到非 IAP 区域
	IAP_ADDRL = 0xff;
}

//========================================================================
// 函数: void EEPROM_Trig(void)
// 描述: 触发EEPROM操作.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
void EEPROM_Trig(void)
{
	F0 = EA;    //保存全局中断
	EA = 0;     //禁止中断, 避免触发命令无效
	IAP_TRIG = 0x5A;
	IAP_TRIG = 0xA5;                    //先送5AH，再送A5H到IAP触发寄存器，每次都需要如此
																			//送完A5H后，IAP命令立即被触发启动
																			//CPU等待IAP完成后，才会继续执行程序。
	_nop_();   //由于STC32G是多级流水线的指令系统，触发命令后建议加4个NOP，保证IAP_DATA的数据完成准备
	_nop_();
	_nop_();
	_nop_();
	EA = F0;    //恢复全局中断
}

//========================================================================
// 函数: void EEPROM_read_n(uint32_t EE_address,uint8_t *DataAddress,uint16_t number)
// 描述: 从指定EEPROM首地址读出n个字节放指定的缓冲.
// 参数: EE_address:  读出EEPROM的首地址.
//       DataAddress: 读出数据放缓冲的首地址.
//       number:      读出的字节长度.
// 返回: non.
// 版本: V1.0, 2012-10-22
//========================================================================
void EEPROM_read_n(uint32_t EE_address,uint8_t *DataAddress,uint16_t number)
{
	IAP_ENABLE();                           //设置等待时间，允许IAP操作，送一次就够
	IAP_READ();                             //送字节读命令，命令不需改变时，不需重新送命令
	do
	{
		IAP_ADDRE = (uint8_t)(EE_address >> 16); //送地址高字节（地址需要改变时才需重新送地址）
		IAP_ADDRH = (uint8_t)(EE_address >> 8);  //送地址中字节（地址需要改变时才需重新送地址）
		IAP_ADDRL = (uint8_t)EE_address;         //送地址低字节（地址需要改变时才需重新送地址）
		EEPROM_Trig();                      //触发EEPROM操作
		*DataAddress = IAP_DATA;            //读出的数据送往
		EE_address++;
		DataAddress++;
	}while(--number);

	DisableEEPROM();
}

//========================================================================
// 函数: void EEPROM_SectorErase(uint32_t EE_address)
// 描述: 把指定地址的EEPROM扇区擦除.
// 参数: EE_address:  要擦除的扇区EEPROM的地址.
// 返回: non.
// 版本: V1.0, 2013-5-10
//========================================================================
void EEPROM_SectorErase(uint32_t EE_address)
{
	IAP_ENABLE();                       //设置等待时间，允许IAP操作，送一次就够
	IAP_ERASE();                        //宏调用, 送扇区擦除命令，命令不需改变时，不需重新送命令
																			//只有扇区擦除，没有字节擦除，512字节/扇区。
																			//扇区中任意一个字节地址都是扇区地址。
	IAP_ADDRE = (uint8_t)(EE_address >> 16); //送扇区地址高字节（地址需要改变时才需重新送地址）
	IAP_ADDRH = (uint8_t)(EE_address >> 8);  //送扇区地址中字节（地址需要改变时才需重新送地址）
	IAP_ADDRL = (uint8_t)EE_address;         //送扇区地址低字节（地址需要改变时才需重新送地址）
	EEPROM_Trig();                      //触发EEPROM操作
	DisableEEPROM();                    //禁止EEPROM操作
}

//========================================================================
// 函数: void EEPROM_write_n(uint32_t EE_address,uint8_t *DataAddress,uint16_t number)
// 描述: 把缓冲的n个字节写入指定首地址的EEPROM.
// 参数: EE_address:  写入EEPROM的首地址.
//       DataAddress: 写入源数据的缓冲的首地址.
//       number:      写入的字节长度.
// 返回: non.
// 版本: V1.0, 2012-10-22
//========================================================================
void EEPROM_write_n(uint32_t EE_address,uint8_t *DataAddress,uint16_t number)
{
	IAP_ENABLE();                       //设置等待时间，允许IAP操作，送一次就够
	IAP_WRITE();                        //宏调用, 送字节写命令
	do
	{
		IAP_ADDRE = (uint8_t)(EE_address >> 16); //送地址高字节（地址需要改变时才需重新送地址）
		IAP_ADDRH = (uint8_t)(EE_address >> 8);  //送地址中字节（地址需要改变时才需重新送地址）
		IAP_ADDRL = (uint8_t)EE_address;         //送地址低字节（地址需要改变时才需重新送地址）
		IAP_DATA  = *DataAddress;         //送数据到IAP_DATA，只有数据改变时才需重新送
		EEPROM_Trig();                    //触发EEPROM操作
		EE_address++;                     //下一个地址
		DataAddress++;                    //下一个数据
	}while(--number);                   //直到结束
	DisableEEPROM();
}

