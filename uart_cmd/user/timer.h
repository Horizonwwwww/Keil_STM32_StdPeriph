#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void TIM2_Init(void);
void TIM2_IRQHandler(void);
uint32_t Get_TIM2_Count(void);
void TIM2_Reset(void);

#endif
