#ifndef __HX711_H
#define __HX711_H

#include "stm32f10x.h"

#define HX711_SCK_PORT GPIOB
#define HX711_SCK_PIN GPIO_Pin_1
#define HX711_DT_PORT GPIOB
#define HX711_DT_PIN GPIO_Pin_0

void HX711_Init(void);
int32_t HX711_Read(void);
int32_t HX711_Read_Average(uint8_t times);
void HX711_Set_Offset(int32_t offset);
void HX711_Set_Scale(float scale);
float HX711_Get_Weight(void);

#endif
