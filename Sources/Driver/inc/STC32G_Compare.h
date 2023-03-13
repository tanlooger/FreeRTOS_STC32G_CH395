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

#ifndef	__STC32G_COMPARE_H
#define	__STC32G_COMPARE_H

#include	"FreeRTOS.h"

//========================================================================
//                              定义声明
//========================================================================

#define	CMP_P_P37	0x00	//输入正极性选择, 0: 选择内部P3.7做正输入
#define	CMP_P_P50	0x01	//输入正极性选择, 1: 选择内部P5.0做正输入
#define	CMP_P_P51	0x02	//输入正极性选择, 2: 选择内部P5.1做正输入
#define	CMP_P_ADC	0x03	//输入正极性选择, 3: 由ADC_CHS[3:0]所选择的ADC输入端做正输入.
#define	CMP_N_P36	0x00	//输入负极性选择, 0: 选择外部P3.6做输入.
#define	CMP_N_GAP	0x01	//输入负极性选择, 1: 选择内部BandGap电压BGv做负输入.

#define	CMPO_P34()	CMPO_S = 0	//结果输出到P3.4.
#define	CMPO_P41()	CMPO_S = 1	//结果输出到P4.1.

typedef struct
{ 
	uint8_t	CMP_EN;					//比较器允许或禁止, 			ENABLE,DISABLE
	uint8_t	CMP_P_Select;		//比较器输入正极选择, CMP_P_P37: 选择P3.7做正输入, CMP_P_ADC: 由ADC模拟输入端做正输入.
	uint8_t	CMP_N_Select;		//比较器输入负极选择, CMP_N_GAP: 选择内部BandGap经过OP后的电压做负输入, CMP_N_P36: 选择P3.6做负输入.
	uint8_t	CMP_Outpt_En;		//允许比较结果输出,   ENABLE,DISABLE
	uint8_t	CMP_InvCMPO;		//比较器输出取反, ENABLE,DISABLE
	uint8_t	CMP_100nsFilter;	//内部0.1uF滤波,  ENABLE,DISABLE
	uint8_t	CMP_OutDelayDuty;	//0~63, 比较结果变化延时周期数
} CMP_InitDefine; 

void	CMP_Inilize(CMP_InitDefine *CMPx);

#endif
