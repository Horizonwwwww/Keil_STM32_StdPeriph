#include "stm32f10x_usart.h"
#include "uart2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "led.h"
#include "sg90.h"

#define RX_BUF_SIZE 64
#define DEBUG_RX 0  // 设为 1 开启调试回显

char rx_buffer[RX_BUF_SIZE];
uint8_t rx_index = 0;

extern float Angle; // 外部舵机角度变量
extern char sg90;
extern int led1_state;
extern int led2_state;

void USART2_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // TX(PA2) 推挽输出, RX(PA3) 浮空输入
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = 9600;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
}

// 发送单个字符
void USART2_SendChar(char ch) {
    USART_SendData(USART2, ch);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

// 发送字符串
void USART2_SendString(char *str) {
    while (*str) {
        USART2_SendChar(*str++);
    }
}

// 发送整数
void USART2_SendNum(uint32_t num) {
    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)num);
    USART2_SendString(buffer);
}

// 命令处理函数
void USART2_CommandHandler(char* cmd) {
    // 清除前后空白字符
    while (*cmd == ' ' || *cmd == '\r' || *cmd == '\n') cmd++;
    int len = strlen(cmd);
    while (len > 0 && (cmd[len - 1] == ' ' || cmd[len - 1] == '\r' || cmd[len - 1] == '\n')) {
        cmd[--len] = '\0';
    }

    if (strcmp(cmd, "1") == 0) {
        LED1_ON();
			led1_state=1;
        USART2_SendString("LED1 turned ON\r\n");
    } else if (strcmp(cmd, "0") == 0) {
        LED1_OFF();
						led1_state=0;
        USART2_SendString("LED1 turned OFF\r\n");
    } else if (strcmp(cmd, "3") == 0) {
        LED2_ON();
			led2_state=1;			
        USART2_SendString("LED2 turned ON\r\n");
    } else if (strcmp(cmd, "2") == 0) {
        LED2_OFF();
			led2_state=0;					
        USART2_SendString("LED2 turned OFF\r\n");
    } 
		else if (strcmp(cmd, "F") == 0) 
		{
        if (Angle >= 180) 
			  {
            USART2_SendString("Servo already at max angle\r\n");
        } 
				else 
				{
            Angle += 90;
            if (Angle > 180) Angle = 180;
            Servo_SetAngle(Angle);
            USART2_SendString("Servo rotated FORWARD 90 degrees\r\n");
        }
				sg90='F';
    } 
		else if (strcmp(cmd, "B") == 0) {
        if (Angle <= 0) {
            USART2_SendString("Servo already at min angle\r\n");
        } else {
            Angle -= 90;
            if (Angle < 0) Angle = 0;
            Servo_SetAngle(Angle);
            USART2_SendString("Servo rotated BACKWARD 90 degrees\r\n");
        }
								sg90='B';
    } else if (strcmp(cmd, "S") == 0) 
		{
        Angle = 0;
        Servo_SetAngle(Angle);
        USART2_SendString("Servo EMERGENCY STOP (reset to 0 degrees)\r\n");
							sg90='S';
    } else if (strcmp(cmd, "STATUS") == 0) {
        USART2_SendString("System is running\r\n");
    } else {
        USART2_SendString("Unknown command: ");
        USART2_SendString(cmd);
        USART2_SendString("\r\n");
    }
}

// USART2 中断服务函数
void USART2_IRQHandler(void) {
    static char last_ch = 0;

    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        char ch = USART_ReceiveData(USART2);

#if DEBUG_RX
        USART2_SendString("RX: ");
        USART2_SendChar(ch);
        USART2_SendString("\r\n");
#endif

        // 忽略连续的 \r\n
        if (ch == '\n' && last_ch == '\r') {
            last_ch = ch;
            USART_ClearITPendingBit(USART2, USART_IT_RXNE);
            return;
        }
        last_ch = ch;

        // 立即处理单字符命令
        if (ch == 'F' || ch == 'B' || ch == 'S') {
            char cmd[2] = { ch, '\0' };
            USART2_CommandHandler(cmd);
        }
				
        // 处理多字符命令，需回车换行结尾
        else if (ch >= '0' && ch <= '9') 
					{
            char cmd[2] = { ch, '\0' };
            USART2_CommandHandler(cmd);
        }
				else 
					{
            USART2_SendString("Command too long, discarded\r\n");
        }

        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}
