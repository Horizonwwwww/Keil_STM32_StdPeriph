#include "hc_sr04.h"
#include "delay.h"
#include "stm32f10x_tim.h"
#include "timer.h"

#define TRIG_PIN GPIO_Pin_15
#define TRIG_PORT GPIOA
#define ECHO_PIN GPIO_Pin_0
#define ECHO_PORT GPIOA




void HC_SR04_Init(void)
{
	

RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // 禁用JTAG保留SWD
	
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // 配置TRIG引脚为推挽输出
    GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TRIG_PORT, &GPIO_InitStructure);
    
    // 配置ECHO引脚为浮空输入
    GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(ECHO_PORT, &GPIO_InitStructure);
    
    // 配置TIM2用于测量高电平时间
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;  // 72MHz/72 = 1MHz, 1us计数
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // 配置TIM2的输入捕获
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x00;
    TIM_ICInit(TIM2, &TIM_ICInitStructure);
    
    // 使能TIM2捕获中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
    
    // 启动TIM2
    TIM_Cmd(TIM2, ENABLE);
}



float HC_SR04_GetDistance(void)
{
		float Distance = 0; 
		uint32_t high_time = 0;
    // 发送10us的高电平触发信号
    GPIO_SetBits(TRIG_PORT, TRIG_PIN);
    Delay_us(10);
    GPIO_ResetBits(TRIG_PORT, TRIG_PIN);
    
    // 等待回波信号
    while(GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN) == RESET);
    
    // 开始计时
    TIM_SetCounter(TIM2, 0);
    while(GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN) == SET);
    
    // 获取高电平时间(us)
    high_time = TIM_GetCounter(TIM2);
    
    // 计算距离 (cm) = (高电平时间 * 声速(340m/s)) / 2
    Distance = high_time * 0.034 / 2;
    
    return Distance;
}


