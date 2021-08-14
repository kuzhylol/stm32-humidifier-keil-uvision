#include <stdint.h>
#include <stdbool.h>
#include "stm32f411xe.h"
#include "piezo.h"

static inline void pz_clock_enable(void)
{
	p_regs.rcc_base->PZ_RCC_BUS |= PZ_RCC_AHB1ENR_GPIOCEN; 
}

static inline void pz_clock_disable(void)
{
	p_regs.rcc_base->PZ_RCC_BUS &= ~(RCC_APB2ENR_TIM1EN); 
}

static inline void pz_pin_config(void)
{
	p_regs.gpio_base->MODER |= PZ_GPIO_MODER_MODER; 
	p_regs.gpio_base->OSPEEDR |= PZ_GPIO_OSPEEDR_OSPEED;
}

static inline void p_turn_on(void)
{
	p_regs.gpio_base->ODR |= PZ_GPIO_ODR;
}

static inline void p_turn_off(void)
{
	p_regs.gpio_base->ODR &= ~(PZ_GPIO_ODR);
}

static inline void p_turn_toggle(void)
{
	p_regs.gpio_base->ODR ^= PZ_GPIO_ODR;
}

void pz_init(Piezo *pz)
{
	pz->on = &p_turn_on;
	pz->off = &p_turn_off;
	pz->toggle = &p_turn_toggle;
	
	pz_clock_enable();
	pz_pin_config();
}


