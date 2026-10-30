#include "stm32f10x.h" 
#include "hc_sr501.h" 

// GPIO初始化（PA0）
void HC_SR501_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// 检测人体移动
uint8_t HC_SR501_Detect(void)
{
    return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
}

