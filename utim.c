#include <stdint.h>
#include "stm32f411xe.h"
#include "utim.h"
#include "mcu_init.h"

static inline void utim_clock_enable(void)
{
	ut_regs.rcc_base->UT_RCC_BUS |= UT_RCC_BUS_EN; 
}

static inline void utim_clock_disable(void)
{
	ut_regs.rcc_base->UT_RCC_BUS &= ~(UT_RCC_BUS_EN); 
}

static inline void utim_set_timeout(uint32_t timeout)
{
	ut_regs.timer_base->PSC = (uint32_t)UT_PSC;
	ut_regs.timer_base->ARR = (uint32_t)(timeout *(MCU_CLOCK_HZ/(UT_PSC+1)/1000000)-1);
	
	/*  Auto-reload preload enable */
	ut_regs.timer_base->CR1 |= UT_TIM_CR1_ARPE;
	
}

static inline void utim_enable(void)
{
	/* UIF interrupt enable */
	NVIC_EnableIRQ(UT_NVIC_IRQ);		
	ut_regs.timer_base->DIER |= UT_TIM_DIER_UIE;	
	ut_regs.timer_base->CR1 |= UT_TIM_CR1_CEN;
}

static inline void utim_disable(void)
{
	NVIC_DisableIRQ(UT_NVIC_IRQ);	
	ut_regs.timer_base->CR1 &= ~(UT_TIM_CR1_CEN);	
	ut_regs.timer_base->DIER &= ~(UT_TIM_DIER_UIE);	
}

static inline void utim_clean_irq_flag(void)
{
	ut_regs.timer_base->SR &= ~UT_TIM_SR_UIF;
}

static inline bool utim_is_irq(void)
{
	return (ut_regs.timer_base->SR & UT_TIM_SR_UIF) ? true : false;
}

static void utim_start(uint32_t us)
{
	utim_set_timeout(us);
	utim_enable();
}

static inline void utim_stop(void)
{
	utim_disable();
}

void utim_init(uTimer *ut)
{
	ut->start = &utim_start;
	ut->stop = &utim_stop;
	
	utim_clock_enable();
	utim_disable();
}

void utim_udelay(uint32_t us)
{
	ut_global_flag = true;
	const uint32_t extention = 10;
	const uint32_t factor = us/extention;

	unsigned int us_cnt = 0;

	utim_start(extention);	
	do {
		while(ut_global_flag) {
			__NOP();
		}
		ut_global_flag = true;
	}while(++us_cnt < factor);
	
	utim_stop();
}

/* Handler triggers every N seconds */
void TIM2_IRQHandler(void)
{
	if (utim_is_irq()) {
		ut_global_flag = false;
		utim_clean_irq_flag();
	}
}
