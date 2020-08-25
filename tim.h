#include <stdbool.h>
#include <stdint.h>
#include "stm32f411xe.h"
#include "mcu_init.h"

#define WT_PSC 60000
#define WT_TIMEOUT_S 5

#define WT_RCC_BUS APB2ENR
#define WT_RCC_BUS_EN RCC_APB2ENR_TIM1EN

#define WT_TIM_CR1_CEN TIM_CR1_CEN

#define WT_TIM_DIER_UIE TIM_DIER_UIE
#define WT_TIM_CR1_ARPE TIM_CR1_ARPE

#define WT_TIM_SR_UIF TIM_SR_UIF
#define WT_NVIC_IRQ TIM1_UP_TIM10_IRQn

typedef struct WTimer {
	void (*start)(void);
	void (*stop)(void);
	
}WideTimer;

struct wtimer_regs {
	RCC_TypeDef *rcc_reg;
	TIM_TypeDef *timer_reg;
};

static struct wtimer_regs wt_regs = {
	.timer_reg = TIM1,
	.rcc_reg = RCC
};

extern volatile uint32_t wt_flag;

void wtim_init(WideTimer *wt, uint32_t timeout);
