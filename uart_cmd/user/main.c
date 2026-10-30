#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "oled.h"
#include "pwm.h"
#include "key.h"
#include "sg90.h"
#include "led.h"
#include "hc_sr04.h"
#include "hc_sr501.h"
#include "dht11.h"
#include "hx711.h"
#include "stm32f10x_usart.h" 
#include "uart2.h" 
#include <stdio.h>
#include "sys.h"


uint8_t i;
uint8_t KeyNum;
float Angle=0;
volatile uint8_t key_flag = 0;
char sg90='S';


float Distance = 0;  

u8 temperature;
u8 humidity;

float weight = 0;
int led1_state = 1;
int led2_state = 1;
uint8_t HC_SR501_Detect(void);

int main(void)
{

	OLED_Init();
	OLED_Clear();
	Key_Init();
	LED_Init();

	Servo_Init();
	Servo_SetAngle(0);
	HC_SR04_Init();
    HC_SR501_Init();	
    DHT11_Init();
    HX711_Init();
	USART2_Init();
	
    // HX711校准 
    HX711_Set_Offset(HX711_Read_Average(20));
    HX711_Set_Scale(415.0f); // 比例系数需要根据传感器调整	
	
	OLED_ShowString(1, 1, "   L   K   !");
	uint64_t num = 15575439940;
	OLED_ShowNum(3, 1, (uint32_t)(num/1000000000), 2);
	OLED_ShowNum(3, 3, (uint32_t)(num%1000000000), 9);
    Delay_ms(1000);
    OLED_Clear();
	
	while (1)
	{
		
		KeyNum = Key_GetNum();
		if(KeyNum==1)
		{
			Angle += 90;
			if(Angle > 180)
			{
				Angle = 180;
			}
			sg90='F';
            key_flag = 1;			
		}
		
		if(KeyNum==2)
		{
			Angle -= 90;
			if(Angle <= 0)
			{
				Angle = 0;
			}
			sg90='B';		
            key_flag = 1;			
		}
		
		if (KeyNum==3)               
        {
					LED1_Turn();
            key_flag = 1;					
        }

        // 测量距离并显示
        Distance = HC_SR04_GetDistance();
        OLED_ShowString(2, 1, "Dist:");
        OLED_ShowNum(2, 6, (uint16_t)Distance, 3);
        OLED_ShowString(2, 9, "cm");	
				
        if(HC_SR501_Detect())
        {
            OLED_ShowString(4, 1, "Active Now     ");
            
        }
        else
        {
            OLED_ShowString(4, 1, "No activity yet ");
        }

		DHT11_Read_Data(&temperature, &humidity);
        OLED_ShowString(1, 1, "TEMP:");
        OLED_ShowNum(1, 6, temperature, 2);
        OLED_ShowString(1, 8, "  H:");
        OLED_ShowNum(1, 12, humidity, 2);	
		
        // 读取重量
        weight = HX711_Get_Weight();
        OLED_ShowString(3, 1, "Weight:");
        OLED_ShowNum(3, 8, (int16_t)weight, 4);
        OLED_ShowString(3, 12, "g");				
						
		Servo_SetAngle(Angle);

    // 格式化数据为JSON字符串
    char uart_msg[128];

sprintf(uart_msg, 
    "{\"Distance\":%.1f,\"PIR\":%d,\"Temp\":%d,\"Humi\":%d,\"Weight\":%.1f,\"sg90\":\"%c\",\"LED1\":%d,\"LED2\":%d}\r\n",
    Distance,
    HC_SR501_Detect(),
    temperature,
    humidity,
    weight,
    sg90,
    led1_state,
    led2_state
);

    
    // 通过UART2发送
    USART2_SendString(uart_msg);
		
        // 按键状态反馈
        if(key_flag) {
            char msg[32];
            sprintf(msg, "Key pressed: %d\r\n", KeyNum);
            USART2_SendString(msg);
            key_flag = 0;
        }		
				
//		Delay_ms(100);
						
	}
}

