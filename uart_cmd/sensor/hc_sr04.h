#ifndef __HCSR04_H
#define __HCSR04_H

#include "stm32f10x.h"

void HC_SR04_Init(void);

float HC_SR04_GetDistance(void);

#endif
