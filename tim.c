#include <stdint.h>
#include "stm32f411xe.h"
#include "tim.h"
#include "mcu_init.h"

static inline void wtim_clock_enable(void)
{
	wt_regs.rcc_reg->WT_RCC_BUS |= WT_RCC_BUS_EN; 
}

static inline void wtim_clock_disable(void)
{
	wt_regs.rcc_reg->WT_RCC_BUS &= ~(RCC_APB2ENR_TIM1EN); 
}

static inline void wtim_set_timeout(uint32_t timeout)
{
	wt_regs.timer_reg->PSC = (uint32_t)WT_PSC;
	wt_regs.timer_reg->ARR = (uint32_t)(timeout * (MCU_CLOCK_HZ/(WT_PSC+1))-1);
	
	/*  Auto-reload preload enable */
	wt_regs.timer_reg->CR1 |= WT_TIM_CR1_ARPE;
	
}

static inline void wtim_enable(void)
{
	/* UIF interrupt enable */
	NVIC_EnableIRQ(WT_NVIC_IRQ);		
	wt_regs.timer_reg->DIER |= WT_TIM_DIER_UIE;	
	wt_regs.timer_reg->CR1 |= WT_TIM_CR1_CEN;
}

static inline void wtim_disable(void)
{
	wt_regs.timer_reg->CR1 &= ~(WT_TIM_CR1_CEN);	
	wt_regs.timer_reg->DIER &= ~(WT_TIM_DIER_UIE);	
	NVIC_DisableIRQ(WT_NVIC_IRQ);	
}

static inline void wtim_clean_irq_flag(void)
{
	wt_regs.timer_reg->SR &= ~WT_TIM_SR_UIF;
}

static inline bool wtim_is_irq(void)
{
	return (wt_regs.timer_reg->SR & WT_TIM_SR_UIF) ? true : false;
}

static void wtim_start(void)
{
	wtim_enable();
}

static void wtim_stop(void)
{
	wtim_enable();
}

void wtim_init(WideTimer *wt, uint32_t timeout)
{
	wt->start = &wtim_start;
	wt->stop = &wtim_stop;
	
	wtim_clock_enable();
	wtim_set_timeout(timeout);
	wtim_disable();
}

volatile uint32_t wt_flag = false;

/* Handler triggers every N seconds */
void TIM1_UP_TIM10_IRQHandler(void)
{
	if (wtim_is_irq()) {
		wtim_clean_irq_flag();
		wt_flag = true;
	}
}