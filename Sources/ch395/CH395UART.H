#ifndef __CH395UART_H__
#define __CH395UART_H__

#include "FreeRTOS.h"

void mDelayuS(uint8_t us);
void mDelaymS(uint8_t ms);
void xWriteCH395Cmd(uint8_t cmd);
void  xWriteCH395Data(uint8_t mdata);
uint8_t  xReadCH395Data(void);

#endif