#include <stdint.h>
#include <stdbool.h>
#include "systick.h"
#include "mcu_init.h"
#include "stm32f411xe.h"

volatile uint32_t jiffies = 0;

void SysTick_Handler(void)
{
	jiffies++;
}

bool systick_init(const uint16_t div)
{
	unsigned int load = (MCU_CLOCK_HZ)/(div-1);
	SysTick->LOAD = load;
	SysTick->VAL = load;
	
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
									SysTick_CTRL_TICKINT_Msk   |
									SysTick_CTRL_ENABLE_Msk;
	
	NVIC_EnableIRQ(SysTick_IRQn);
	
	return true;
}

bool sys_delay_ms(const uint32_t ms) 
{
	uint32_t moment = jiffies;
	while((jiffies - moment) < ms); 
	
	return true;
}

