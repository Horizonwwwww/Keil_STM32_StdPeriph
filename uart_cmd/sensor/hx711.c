#include "hx711.h"
#include "delay.h"

static int32_t HX711_Offset = 0;
static float HX711_Scale = 1.0f;

void HX711_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // SCK配置为输出
    GPIO_InitStructure.GPIO_Pin = HX711_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HX711_SCK_PORT, &GPIO_InitStructure);
    
    // DT配置为输入
    GPIO_InitStructure.GPIO_Pin = HX711_DT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(HX711_DT_PORT, &GPIO_InitStructure);
    
    GPIO_SetBits(HX711_SCK_PORT, HX711_SCK_PIN);
}

int32_t HX711_Read(void)
{
    uint32_t count = 0;
    uint8_t i;
    
    GPIO_ResetBits(HX711_SCK_PORT, HX711_SCK_PIN);
    
    while(GPIO_ReadInputDataBit(HX711_DT_PORT, HX711_DT_PIN));
    
    for(i = 0; i < 24; i++)
    {
        GPIO_SetBits(HX711_SCK_PORT, HX711_SCK_PIN);
        Delay_us(1);
        count <<= 1;
        GPIO_ResetBits(HX711_SCK_PORT, HX711_SCK_PIN);
        Delay_us(1);
        if(GPIO_ReadInputDataBit(HX711_DT_PORT, HX711_DT_PIN))
            count++;
    }
    
    GPIO_SetBits(HX711_SCK_PORT, HX711_SCK_PIN);
    count ^= 0x800000;
    GPIO_ResetBits(HX711_SCK_PORT, HX711_SCK_PIN);
    
    return (int32_t)count;
}

int32_t HX711_Read_Average(uint8_t times)
{
    int32_t sum = 0;
    uint8_t i;
    
    for(i = 0; i < times; i++)
    {
        sum += HX711_Read();
        Delay_ms(10);
    }
    
    return sum / times;
}

void HX711_Set_Offset(int32_t offset)
{
    HX711_Offset = offset;
}

void HX711_Set_Scale(float scale)
{
    HX711_Scale = scale;
}

float HX711_Get_Weight(void)
{
    int32_t val = HX711_Read_Average(10) - HX711_Offset;
    return (float)val / HX711_Scale;
}
