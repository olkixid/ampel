/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include "stm32f407xx_gpio_driver.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif



static GPIO_Handle_t red_led = {
	GPIOA,
	{
		GPIO_PIN_NO_1,
		GPIO_MODE_OUT,
		GPIO_SPEED_LOW,
		GPIO_NO_PUPD,
		GPIO_OP_TYPE_PP
	}
};

static GPIO_Handle_t yellow_led = {
	GPIOA,
	{
		GPIO_PIN_NO_3,
		GPIO_MODE_OUT,
		GPIO_SPEED_LOW,
		GPIO_NO_PUPD,
		GPIO_OP_TYPE_PP
	}
};

static GPIO_Handle_t green_led = {
	GPIOA,
	{
		GPIO_PIN_NO_5,
		GPIO_MODE_OUT,
		GPIO_SPEED_LOW,
		GPIO_NO_PUPD,
		GPIO_OP_TYPE_PP
	}
};

static GPIO_Handle_t button = {
	GPIOB,
	{
		GPIO_PIN_NO_1,
		GPIO_MODE_IT_FT,
		GPIO_SPEED_LOW,
		GPIO_PIN_PU,
		GPIO_OP_TYPE_PP
	}
};



enum color_state{
	RED_ON,
	RED_YELLOW_ON,
	GREEN_ON,
	YELLOW_ON
};

typedef struct {
	volatile uint32_t counter;
	volatile enum color_state cs;
}tl_state_t;

static volatile tl_state_t tl_state = {
	0U,
	RED_ON
};


static void wait_for_timer(void) {
	for (; tl_state.counter != 0; tl_state.counter--);
}

static void set_timer_5s(void) {
	tl_state.counter = 5697356U;
}

static void set_timer_7s(void) {
	tl_state.counter = 7976298U;
}

static void next_state(void) {
	switch (tl_state.cs) {
	case RED_ON:
		GPIO_WriteToOutputPin(&yellow_led, 1);
		tl_state.cs = RED_YELLOW_ON;
		break;
	case RED_YELLOW_ON:
		GPIO_WriteToOutputPin(&red_led, 0);
		GPIO_WriteToOutputPin(&yellow_led, 0);
		GPIO_WriteToOutputPin(&green_led, 1);
		tl_state.cs = GREEN_ON;
		break;
	case GREEN_ON:
		GPIO_WriteToOutputPin(&green_led, 0);
		GPIO_WriteToOutputPin(&yellow_led, 1);
		tl_state.cs = YELLOW_ON;
		break;
	case YELLOW_ON:
		GPIO_WriteToOutputPin(&yellow_led, 0);
		GPIO_WriteToOutputPin(&red_led, 1);
		tl_state.cs = RED_ON;
		break;
	}
}



int main(void)
{
	GPIO_Init(&red_led);
	GPIO_Init(&yellow_led);
	GPIO_Init(&green_led);

	GPIO_Init(&button);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI1, ENABLE);

	GPIO_WriteToOutputPin(&red_led, 1);


	//set_timer_5s und next_state müssten atomic sein. Diese Version könnte Menschenleben kosten.
	for(;;) {
		set_timer_5s();
		wait_for_timer();
		next_state();
	}
}



void EXTI1_IRQHandler(void) {
	GPIO_WriteToOutputPin(&red_led, 1);
	GPIO_WriteToOutputPin(&yellow_led, 0);
	GPIO_WriteToOutputPin(&green_led, 0);
	tl_state.cs = RED_ON;
	set_timer_7s();
	GPIO_IRQHandling(button.GPIO_PinConfig.GPIO_PinNumber);
}
