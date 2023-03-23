/**
 *******************************Copyright (c)************************************
 *
 *                   (c) Copyright 2021, CY, China, QD.
 *                           All Rights Reserved
 *
 *                           By(南京万福祥电子科技有限公司)
 *                           http://www.njwfx.com
 *
 *----------------------------------文件信息------------------------------------
 * 文件名称: modbusTCP.c
 * 创建人员: CY
 * 创建日期: 2021-07-18
 * 文档描述: 
 *
 *----------------------------------版本信息------------------------------------
 * 版本代号: V1.0
 * 版本说明: 
 *          |-1.0
 * 	         |-2021-07-18
 * 	          |-初始版本
 *------------------------------------------------------------------------------
 */

#include "modbusTCP.h"
#include "../typedef/bsp_typedef.h"

 
 /* 私有类型定义 --------------------------------------------------------------*/
 void transport_sendPacketBuffer( uint8_t* buf, int32_t buflen);  //tcp发送
void TCP_Exception_RSP(uint8_t _FunCode,uint8_t _ExCode);  //故障回应
void MB_TCP_RSP(uint8_t _FunCode);                         //正常回应

void TCP_RSP_01_02(void);                                  //功能码01 02读开关量
void TCP_RSP_03_04(void);                                  //功能码03 04 读寄存器
void TCP_RSP_05(void);                                     //功能码05 写单个输出开关量
void TCP_RSP_06(void);                                     //功能码06 写单个保持寄存器
void TCP_RSP_15(void);                                     //功能码15 写多个输出开关量
void TCP_RSP_16(void);                                     //功能码16 写多个保持寄存器

/* 私有宏定义 ----------------------------------------------------------------*/
    
/* 私有变量 ------------------------------------------------------------------*/
uint8_t Rx_Buf[TCP_MAX];            // 接收缓存,最大256字节
uint8_t Tx_Buf[TCP_MAX];            // 发送缓存,最大256字节
uint16_t P_RxCount = 0;     // 接收字符计数
uint16_t P_TxCount=0;       // 发送字符计数
uint16_t P_Addr,P_RegNum,P_ByteNum;

uint8_t  coil[TCP_MAX];              //继电器
uint16_t mreg[TCP_MAX];              //寄存器

SOCKET_HandleTypedef  Socket_0 =                                                  /* 保存Socket信息 */
{
    .SocketIndex = 0,              //socket 编号
    .SourPort = 5000,              //源端口
    .ProtoType = PROTO_TYPE_TCP,   //socket 连接类型
    .TcpMode = TCP_MODE_SERVER,    //tcp模式的类型
    .pRxBuff = Rx_Buf,           //接收缓冲区
    .pTxBuff = Tx_Buf,           //发送缓冲区
};
CH395_HandleTypedef  ch395_0 =  																							/* 保存CH395信息 */
{
    .CH395Index = 0,
    .HSPI = &hspi3,
    .CS_PORT = GPIOC,
    .CS_PIN = GPIO_PIN_9,
    .INT_PORT = GPIOA,
    .INT_PIN = GPIO_PIN_4,
    .IPAddr = {192,168,2,10},
    .GWIPAddr = {192,168,1,1},
    .MASKAddr = {255,255,255,0},
};

/* 扩展变量 ------------------------------------------------------------------*/

/* 私有函数原形 --------------------------------------------------------------*/

/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: tcp发送函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void transport_sendPacketBuffer( uint8_t* buf, int32_t buflen)
{      
  	CH395SendData(&ch395_0,&Socket_0,buf,buflen);   /* 发送数据 */
}

/**
  * 函数功能: 读线圈
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void TCP_RSP_01_02(void)
{

	uint16_t A_Leng=0,B_Leng=0;
  uint8_t i,x;
	uint8_t data[TCP_MAX];
  if ((P_Addr + P_RegNum) < TCP_MAX)
	{
		Tx_Buf[0] = Rx_Buf[0];          //
		Tx_Buf[1] = Rx_Buf[1];          // 
		Tx_Buf[2] = Rx_Buf[2];          //
		Tx_Buf[3] = Rx_Buf[3];          //

		Tx_Buf[6] = Rx_Buf[6];          //站号
		Tx_Buf[7] = Rx_Buf[7];          //功能码
		P_ByteNum = P_RegNum / 8;       //字节数
		
		if (P_RegNum % 8)P_ByteNum += 1;  //如果位数还有余数，则字节数+1
		Tx_Buf[8] = P_ByteNum;       //返回的字数
		
		/* 特殊情况，比如当起始地址为5，而位数为16时，如果根据P_ByteNum计算字节数时，需要加1，取后一数组的数据填充 */
		if((P_RegNum % 8==0)&&(P_Addr!=0))
		{
		  P_ByteNum += 1;
		}
		A_Leng = P_Addr/8; //按照数组位置存储，计算起始位置
		B_Leng = P_Addr%8; //计算位

    /* 将需要使用的数组及发送数组内容清零 */
		memset(&data[0],0,TCP_MAX);
	  memset(&Tx_Buf[9],0,P_ByteNum); 
		/* 将数据分开放置，数组编号1、2   3、4   5、6 ....两个数组存放同样数据，方便以下计算 */
		for(i=0;i<P_ByteNum;i++)
		{
			data[2*i+1]=coil[A_Leng+i];
			data[2*i+2]=coil[A_Leng+i];		  	       			
		}	       	
   /* 根据字节数赋值 */
		for (x = 0; x<P_ByteNum; x++)
		{
			/* 每次填充一个字节内容数据，但是会存在偏移的问题
			将数组1、3、5...等数组位偏移，也就是两个字节的首位偏移，如果偏移地址为零，则不需要偏移 */
			data[2*x+1]=data[2*x+1]>>B_Leng;
			/* 首先填充偏移后的数据内容，假设偏移了两位，那么就是还需要填充6位数据内容 */
			for(i=0;i<8-B_Leng;i++)
			{	
        /* 判断偏移后的第一位，也是最低位 */				
				if((data[2*x+1])&0x01)
				{
					Tx_Buf[9+x] |=  (1 << i);	
				}
				/* 偏移一位后for循环继续判断 */
				data[2*x+1]>>=1;				
			}
			/* 由于存在偏移，那么需要取高一位的数组数据内容填充上一个偏移后的数据内容，总共8位一字节 */
			for(i=0;i<B_Leng;i++)
			{
        /* 判断高一位数据，我们假设前面取data[1]，这里则取data[4]，data[6]，因为下一轮字节循环data[2*x+1]的值是data[3]，data[5] ...
				这样在判断赋值移位时，不影响原来的数据内容*/				
				if((data[2*x+4])&0x01)
				{
					Tx_Buf[9+x] |= (1 << (8+i-B_Leng));       						
				}
				data[2*x+4]>>=1;						
								               				
			}
		}
		
		/* 偏移后高位补零 */
		for(i=0;i<8-P_RegNum % 8;i++)
		{
		  Tx_Buf[8+P_ByteNum] &= ~(1 << (P_RegNum % 8+i));
		}
		
		P_ByteNum += 3;
		Tx_Buf[4] = P_ByteNum >> 8;
		Tx_Buf[5] = P_ByteNum;
    
    P_TxCount=P_ByteNum+6;
    transport_sendPacketBuffer(Tx_Buf,P_TxCount);
	}
	else TCP_Exception_RSP(Rx_Buf[7], 0x02);     //功能码错误回应
}

/**
  * 函数功能: 读寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void TCP_RSP_03_04(void)
{
  uint16_t i;
  if ((P_Addr + P_RegNum) < TCP_MAX)
	{
		Tx_Buf[0] = Rx_Buf[0];          //
		Tx_Buf[1] = Rx_Buf[1];          //
		Tx_Buf[2] = Rx_Buf[2];          //
		Tx_Buf[3] = Rx_Buf[3];          //

		Tx_Buf[6] = Rx_Buf[6];          //站号
		Tx_Buf[7] = Rx_Buf[7];          //功能码
		P_ByteNum = P_RegNum*2;         //字节数
		Tx_Buf[8] = P_ByteNum;          //返回的字节数
		for (i = 0; i<P_RegNum; i++)
		{
			Tx_Buf[9 + i * 2] = mreg[P_Addr + i];        //低字节
			Tx_Buf[10 + i * 2] = mreg[P_Addr + i] >> 8;  //高字节 
		}
		P_ByteNum += 3;
		Tx_Buf[4] = P_ByteNum >> 8;
		Tx_Buf[5] = P_ByteNum;
    		
    P_TxCount=P_ByteNum+6;
    transport_sendPacketBuffer(Tx_Buf,P_TxCount);
	}
	else TCP_Exception_RSP(Rx_Buf[7], 0x02);     //功能码错误回应
}

/**
  * 函数功能: 写单个线圈
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void TCP_RSP_05(void)
{
	uint16_t A_Leng,B_Leng;	
  if (P_Addr < TCP_MAX)
	{
		Tx_Buf[0] = Rx_Buf[0];          //
		Tx_Buf[1] = Rx_Buf[1];          //
		Tx_Buf[2] = Rx_Buf[2];          //
		Tx_Buf[3] = Rx_Buf[3];          //
		Tx_Buf[4] = Rx_Buf[4];
		Tx_Buf[5] = Rx_Buf[5];
		Tx_Buf[6] = Rx_Buf[6];          //站号
		Tx_Buf[7] = Rx_Buf[7];          //功能码
		Tx_Buf[8] = Rx_Buf[8];          //地址
		Tx_Buf[9] = Rx_Buf[9];          //
		Tx_Buf[10] = Rx_Buf[10];        //写入内容
		Tx_Buf[11] = Rx_Buf[11];        //
			
	  A_Leng = P_Addr/8; //按照数组位置存储，计算起始位置
		B_Leng = P_Addr%8; //计算位	
		
		if (Rx_Buf[10] == 0xff || Rx_Buf[11] == 0xff)
		{
			/* 直接将值赋予相应的地址中 */
			coil[A_Leng] |= 1<<B_Leng;  
			//设置线圈
		}			
		else
			coil[P_Addr] = 0x00;
		
    P_TxCount=12;
    transport_sendPacketBuffer(Tx_Buf,P_TxCount);
	}
	else TCP_Exception_RSP(Rx_Buf[7], 0x02);     //功能码错误回应
}

/**
  * 函数功能: 写单个寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void TCP_RSP_06(void)
{
  if (P_Addr < TCP_MAX)
	{
		Tx_Buf[0] = Rx_Buf[0];          //
		Tx_Buf[1] = Rx_Buf[1];          //
		Tx_Buf[2] = Rx_Buf[2];          //
		Tx_Buf[3] = Rx_Buf[3];          //
		Tx_Buf[4] = Rx_Buf[4];
		Tx_Buf[5] = Rx_Buf[5];
		Tx_Buf[6] = Rx_Buf[6];          //站号
		Tx_Buf[7] = Rx_Buf[7];          //功能码
		Tx_Buf[8] = Rx_Buf[8];          //地址
		Tx_Buf[9] = Rx_Buf[9];          //
		Tx_Buf[10] = Rx_Buf[10];        //写入内容
		Tx_Buf[11] = Rx_Buf[11];        //

		mreg[P_Addr] = Rx_Buf[10];        //数据写入
		mreg[P_Addr] |= Rx_Buf[11] << 8;
    
    P_TxCount=12;
    transport_sendPacketBuffer(Tx_Buf,P_TxCount);
	}
	else TCP_Exception_RSP(Rx_Buf[7], 0x02);     //功能码错误回应
}

/**
  * 函数功能: 写多个线圈
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void TCP_RSP_15(void)
{
	uint8_t next_data[255];
  uint16_t i,x;
	uint16_t A_Leng,B_Leng;
  if ((P_Addr + P_RegNum) < TCP_MAX)
	{
		Tx_Buf[0] = Rx_Buf[0];          //
		Tx_Buf[1] = Rx_Buf[1];          //
		Tx_Buf[2] = Rx_Buf[2];          //
		Tx_Buf[3] = Rx_Buf[3];          //
		Tx_Buf[4] = 0;
		Tx_Buf[5] = 7;
		Tx_Buf[6] = Rx_Buf[6];          //站号
		Tx_Buf[7] = Rx_Buf[7];          //功能码
		Tx_Buf[8] = Rx_Buf[8];          //地址
		Tx_Buf[9] = Rx_Buf[9];          //
		Tx_Buf[10] = Rx_Buf[10];        //数量
		Tx_Buf[11] = Rx_Buf[11];        //

	  A_Leng = P_Addr/8; //按照数组位置存储，计算起始位置
		B_Leng = P_Addr%8; //计算位	
		
	  memset(coil,0,Rx_Buf[12]+1);       		
		for(x=0;x<Rx_Buf[12];x++)  //根据字节数写入线圈
		{
      next_data[x]=Rx_Buf[13+x];
			//printf("---------------Rx_Buf[x]=%x\n",Rx_Buf[13+x]);
			for (i = 0; i<(8-B_Leng); i++)//设置线圈（首地址开始）
			{	
				if((Rx_Buf[13+x])&0x01)  //判断位是否有效
				{			
					coil[A_Leng+x] |= (1 << (i % 8))<<B_Leng; //从偏移地址开始赋值，偏移地址为B_Leng
					//printf("1---r[%d]=%x\n",i,coil[A_Leng+x]);				
				}			
				Rx_Buf[13+x]>>=1;				//继续判断下一位
			}

			for (i = 0; i<B_Leng; i++)//设置线圈（剩余位设置）
			{	
				if((next_data[x]>>(8-B_Leng))&0x01)  //根据前面判断剩下的数据继续处理
				{			
					coil[A_Leng+x+1] |= 1 << (i % 8);  //剩余位赋值置位
					//printf("3---r[%d]=%x\n",i,coil[A_Leng+x+1]);				
				}
				next_data[x]>>=1;				
			}		  
		}
					
    P_TxCount=12;
    transport_sendPacketBuffer(Tx_Buf,P_TxCount);
	}
		
	else TCP_Exception_RSP(Rx_Buf[7], 0x02);     //功能码错误回应
}



/**
  * 函数功能: 写多个寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void TCP_RSP_16(void)
{
  uint16_t i;
  if ((P_Addr + P_RegNum) < TCP_MAX)
	{
		Tx_Buf[0] = Rx_Buf[0];          //
		Tx_Buf[1] = Rx_Buf[1];          //
		Tx_Buf[2] = Rx_Buf[2];          //
		Tx_Buf[3] = Rx_Buf[3];          //
		Tx_Buf[4] = 0;
		Tx_Buf[5] = 7;
		Tx_Buf[6] = Rx_Buf[6];          //站号
		Tx_Buf[7] = Rx_Buf[7];          //功能码
		Tx_Buf[8] = Rx_Buf[8];          //地址
		Tx_Buf[9] = Rx_Buf[9];          //
		Tx_Buf[10] = Rx_Buf[10];        //数量
		Tx_Buf[11] = Rx_Buf[11];        //
		
		for (i = 0; i<P_RegNum; i++)//设置线圈
		{
			mreg[P_Addr + i] = Rx_Buf[13 + i * 2];      //低字节
			mreg[P_Addr + i] |= Rx_Buf[14 + i * 2] << 8;  //高字节
		}
    P_TxCount=12;
    transport_sendPacketBuffer(Tx_Buf,P_TxCount);
	}
	else TCP_Exception_RSP(Rx_Buf[7], 0x02);     //功能码错误回应
}

/**
  * 函数功能: 异常响应
  * 输入参数: _FunCode :发送异常的功能码,_ExCode:异常码
  * 返 回 值: 无
  * 说    明: 当通信数据帧发生异常时,发送异常响应
  */
void TCP_Exception_RSP(uint8_t _FunCode,uint8_t _ExCode)
{
  Tx_Buf[0] = Rx_Buf[0];          //
	Tx_Buf[1] = Rx_Buf[1];          //
	Tx_Buf[2] = Rx_Buf[2];          //
	Tx_Buf[3] = Rx_Buf[3];          //
	Tx_Buf[4] = 0;              //
	Tx_Buf[5] = 3;
	Tx_Buf[6] = Rx_Buf[6];          //站号
	Tx_Buf[7] = _FunCode | 0x80;          //功能码
	Tx_Buf[8] = _ExCode;
  
  P_TxCount=9;
  transport_sendPacketBuffer(Tx_Buf,P_TxCount);
}
/**
  * 函数功能: 正常响应
  * 输入参数: _FunCode :功能码
  * 返 回 值: 无
  * 说    明: 当通信数据帧没有异常时并且成功执行之后,发送响应数据帧
  */
void MB_TCP_RSP(uint8_t _FunCode)
{
  P_Addr = ((Rx_Buf[8]<<8)| Rx_Buf[9]);      //寄存器地址
  P_RegNum = ((Rx_Buf[10]<<8) | Rx_Buf[11]);   //寄存器数量
  switch(_FunCode)
  {
    case 01:
    case 02:
      TCP_RSP_01_02();
      break;
      
    case 03:
    case 04:
      TCP_RSP_03_04();
      break;
      
    case 05:
      TCP_RSP_05();
      break;
      
    case 06:
      TCP_RSP_06();
      break;
      
    case 15:
      TCP_RSP_15();
      break;
      
    case 16:
      TCP_RSP_16();
      break;
  }
}

/**
  * 函数功能: 对接收到的数据进行分析并执行
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void MB_Parse_Data(void)  
{

    if ((P_RxCount - 6) == (Rx_Buf[5] | Rx_Buf[4] << 8))  //效验数量
    {
      if (Rx_Buf[6] < TCP_ALLSLAVEADDR)  //从站ID
      {
        if ((Rx_Buf[7] == 01) || (Rx_Buf[7] == 02) || (Rx_Buf[7] == 03) || (Rx_Buf[7] == 04) || (Rx_Buf[7] == 05) || (Rx_Buf[7] == 06) || (Rx_Buf[7] == 15) || (Rx_Buf[7] == 16))//功能码
        {
          MB_TCP_RSP(Rx_Buf[7]);                    //正常反馈
        }
        else
        {
          TCP_Exception_RSP(Rx_Buf[7], 0x01);     //功能码错误回应
        }
      }
      else
      {
        TCP_Exception_RSP(Rx_Buf[7], 0x03);     //ID站号错误回应
      }
    }
    else
    {
      TCP_Exception_RSP(Rx_Buf[7], 0x04);     //数量错误回应
    }
    P_RxCount=0;
  
}
 

//ch395接收中断回调
void CH395_SocketIRQReceive_CallBack(CH395_HandleTypedef* pCH395Handle,SOCKET_HandleTypedef* pSocketHandle)
{
	P_RxCount = pSocketHandle->len;
  
	MB_Parse_Data();
}
 
 
 
 
 
 
/********************************End of File************************************/



