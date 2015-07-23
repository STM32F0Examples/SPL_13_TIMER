#include "stm32f0xx.h"                  // Device header

/**
 * Set System Clock Speed to 48Mhz
 */
void setToMaxSpeed(void);

/**
 * Initialized the timer 15 as a downcounter periodic timer
 * Also enables the update interrupt for this timer
 * @param period_ms timer period in milliseconds
 */
void timer_15_init(int period_ms);

/**
 * Initialize Pin B1 as output
 */
void led_init(void);

int main(void){
	setToMaxSpeed();
	led_init();
	timer_15_init(500);
	while(1){
	}
}

void setToMaxSpeed(void){
	int internalClockCounter;
	RCC_PLLCmd(DISABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_HSEConfig(RCC_HSE_OFF);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_12);
	RCC_PREDIV1Config(RCC_PREDIV1_Div1);
	RCC_PLLCmd(ENABLE);
	while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	for(internalClockCounter=0;internalClockCounter<1024;internalClockCounter++){
		if(RCC_GetSYSCLKSource()==RCC_SYSCLKSource_PLLCLK){
			SystemCoreClockUpdate();
			break;
		}
	}
}

void timer_15_init(int period_ms){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15,ENABLE);
	TIM_TimeBaseInitTypeDef myTimer;
	TIM_TimeBaseStructInit(&myTimer);
	myTimer.TIM_CounterMode=TIM_CounterMode_Down;
	myTimer.TIM_Prescaler=(48000-1);//CountFreq=(48MHz/Prescaler+1)=1KHz
	myTimer.TIM_ClockDivision=TIM_CKD_DIV1;
	myTimer.TIM_Period=period_ms;
	TIM_TimeBaseInit(TIM15,&myTimer);
	
	TIM_InternalClockConfig(TIM15);
	
	TIM_ITConfig(TIM15,TIM_IT_Update,ENABLE);
	NVIC_EnableIRQ(TIM15_IRQn);
	
	TIM_Cmd(TIM15,ENABLE);
}

void led_init(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	GPIO_InitTypeDef myGPIO;
	GPIO_StructInit(&myGPIO);
	myGPIO.GPIO_Pin=GPIO_Pin_1;
	myGPIO.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_Init(GPIOB,&myGPIO);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
}

void TIM15_IRQHandler(void){
	TIM_ClearITPendingBit(TIM15,TIM_IT_Update);
	GPIO_WriteBit(GPIOB,GPIO_Pin_1,!GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_1));//toggles led
}
