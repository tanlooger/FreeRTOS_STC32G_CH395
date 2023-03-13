#include "ch395.h"
#include "string.h"



struct _SOCK_INF xdata SockInf[4];/* 保存Socket信息 */
struct _CH395_SYS xdata CH395Inf;/* 保存CH395信息 */


/* CH395相关定义 */
const uint8_t CH395IPAddr[4] = {192,168,1,10};                         /* CH395IP地址 */
const uint8_t CH395GWIPAddr[4] = {192,168,1,1};                        /* CH395网关 */
const uint8_t CH395IPMask[4] = {255,255,255,0};                        /* CH395子网掩码 */



/**********************************************************************************
* Function Name  : InitCH395InfParam
* Description    : 初始化CH395Inf参数
* Input          : None
* Output         : None
* Return         : None
**********************************************************************************/
void InitCH395InfParam(void)
{
    memset(&CH395Inf,0,sizeof(CH395Inf)); /* 将CH395Inf全部清零*/
    memcpy(CH395Inf.IPAddr,CH395IPAddr,sizeof(CH395IPAddr));         /* 将IP地址写入CH395Inf中 */
    memcpy(CH395Inf.GWIPAddr,CH395GWIPAddr,sizeof(CH395GWIPAddr));   /* 将网关IP地址写入CH395Inf中 */
    memcpy(CH395Inf.MASKAddr,CH395IPMask,sizeof(CH395IPMask));       /* 将子网掩码写入CH395Inf中 */
}



/**********************************************************************************
* Function Name  : CH395Init
* Description    : 配置CH395的IP,GWIP,MAC等参数，并初始化
* Input          : None
* Output         : None
* Return         : 函数执行结果
**********************************************************************************/
UINT8  CH395Init(void)
{
    UINT8 i;
    
    i = CH395CMDCheckExist(0x65);                      
    if(i != 0x9a)return CH395_ERR_UNKNOW; /* 测试命令，如果无法通过返回0XFA */
               /* 返回0XFA一般为硬件错误或者读写时序不对 */
#if (CH395_OP_INTERFACE_MODE == 5)        
#ifdef UART_WORK_BAUDRATE
    CH395CMDSetUartBaudRate(UART_WORK_BAUDRATE);/* 设置波特率 */   
    mDelaymS(1);
    SetMCUBaudRate();
#endif
#endif
/*注：CH395出厂时已烧录MAC地址，此处设置MAC地址主要为演示操作，建议正常使用时候，直接调用获取MAC地址命令，无需重新设置MAC地址*/
    //CH395CMDSetMACAddr(CH395Inf.MacAddr); /* 设置CH395的MAC地址，MAC RAW仅需要设置MAC地址 */
    mDelaymS(200);                        /*必要的延时函数*/
    mDelaymS(200);    
    mDelaymS(200);    
    mDelaymS(200);    
    mDelaymS(200);    
    mDelaymS(200);    
    mDelaymS(200);    
    mDelaymS(200);    
    mDelaymS(200);    
    i = CH395CMDInitCH395();              /* 初始化CH395芯片 */
    return i;
}




