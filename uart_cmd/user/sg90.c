#include "stm32f10x.h"
#include "pwm.h"
#include "sg90.h"

void Servo_Init(void)
{
	PWM_Init();
}

void Servo_SetAngle(float Angle)
{
	PWM_SetCompare4(Angle/180*2000+500);
}
