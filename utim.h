#include <stdbool.h>
#include <stdint.h>
#include "stm32f411xe.h"
#include "mcu_init.h"

#define UT_PSC 2

#define UT_RCC_BUS APB1ENR
#define UT_RCC_BUS_EN RCC_APB1ENR_TIM2EN

#define UT_TIM_CR1_CEN TIM_CR1_CEN

#define UT_TIM_DIER_UIE TIM_DIER_UIE
#define UT_TIM_CR1_ARPE TIM_CR1_ARPE

#define UT_TIM_SR_UIF TIM_SR_UIF
#define UT_NVIC_IRQ TIM2_IRQn

typedef struct UTimer {
	void (*start)(void);
	void (*stop)(void);
}uTimer;

struct utimer_regs {
	RCC_TypeDef *rcc_base;
	TIM_TypeDef *timer_base;
};

static struct utimer_regs ut_regs = {
	.rcc_base = RCC,
	.timer_base = TIM2
};

extern volatile bool ut_flag;
extern volatile uint32_t ucnt_global;

void utim_init(uTimer *ut, uint32_t us);
void udelay(uint32_t us);