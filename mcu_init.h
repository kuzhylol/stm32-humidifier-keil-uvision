#include <stdint.h>
#include <stdbool.h>

#define MCU_CLOCK_MHZ (uint32_t)48
#define MCU_CLOCK_HZ MCU_CLOCK_MHZ * (uint32_t)1000000

bool stm32f11re_init(void);
void udelay(uint32_t us);
