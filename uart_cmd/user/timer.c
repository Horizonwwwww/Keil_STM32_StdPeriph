#include "timer.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
//#include "key.h"

/**
  * @brief  初始化TIM2用于输入捕获
  * @param  无
  * @retval 无
  */
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 使能TIM2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // 定时器基础设置
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  // 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = 71;       // 预分频值 (72MHz/72 = 1MHz, 1us计数)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // 输入捕获设置
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  // 上升沿捕获
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;        // 不分频
    TIM_ICInitStructure.TIM_ICFilter = 0x00;                     // 不滤波
    TIM_ICInit(TIM2, &TIM_ICInitStructure);
    
    // 使能捕获中断
    TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
    
    // 配置TIM2中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 启动TIM2
    TIM_Cmd(TIM2, ENABLE);
}

/**
  * @brief  获取TIM2计数器值
  * @param  无
  * @retval 计数器值
  */
uint32_t Get_TIM2_Count(void)
{
    return TIM_GetCounter(TIM2);
}

/**
  * @brief  重置TIM2计数器
  * @param  无
  * @retval 无
  */
void TIM2_Reset(void)
{
    TIM_SetCounter(TIM2, 0);
}

/**
  * @brief  TIM2中断服务函数
  * @param  无
  * @retval 无
  */

//extern volatile uint8_t key_flag;
//extern uint8_t KeyNum;

void TIM2_IRQHandler(void)
{
    // 处理定时器更新中断
//    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
//    {
//        KeyNum = Key_GetNum();
//        if (KeyNum != 0)
//        {
//            key_flag = KeyNum;
//        }

//        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  // 清除更新中断标志
//    }

    // 处理捕获比较中断
    if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
    {
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
    }
}





