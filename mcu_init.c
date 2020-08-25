#include <stdbool.h>
#include <stdint.h>
#include "stm32f411xe.h"
#include "systick.h"
#include "mcu_init.h"
#include "utim.h"

void udelay(uint32_t us)
{
	uint32_t now = ucnt_global;
	while((ucnt_global - now) < us);
}

static void clock_hsi_pll_init(void)
{
	// f(VCO clock) = f(PLL clock input) × (PLLN / PLLM)
	// f(PLL general clock output) = f(VCO clock) / PLLP
	
	/* 000010: PLLM = 16 */
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_4;
	
	/* 000110010: PLLN = 50 */
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_7;
		
	/* 01: PLLP = 4 */
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0;
	
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ_2;

	/* Fpll out (SYSCLK) is 48 MHz */
		
	/* 10: PLL selected as system clock - System Clock MUX*/ 
	RCC->CFGR |= RCC_CFGR_SW_PLL;
}

static void set_flash_latency(uint32_t hz)
{
		if ((hz > 30) && (hz <= 64)) {
			FLASH->ACR = FLASH_ACR_LATENCY_1WS;
		}
}

static void clock_tree_en(void) 
{
	/* Internal high-speed clock enable */
	RCC->CR |= RCC_CR_HSION;
	/* Turn on PLL */
	RCC->CR |= RCC_CR_PLLON; 
}


bool stm32f11re_init(void) 
{
	clock_hsi_pll_init();
	set_flash_latency(MCU_CLOCK_MHZ);
	clock_tree_en();
		/* 1 ms */
	systick_init(1000);

	return true;
}
