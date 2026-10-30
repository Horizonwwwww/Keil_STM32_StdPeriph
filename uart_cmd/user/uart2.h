#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void USART2_Init(void);
void USART2_SendChar(char ch);
void USART2_SendString(char *str);
void USART2_SendNum(uint32_t num);
void USART2_CommandHandler(char* cmd);
void USART2_IRQHandler(void);

#endif
