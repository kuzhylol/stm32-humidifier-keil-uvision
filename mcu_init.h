#include <stdint.h>
#include <stdbool.h>

#define MCU_CLOCK_MHZ 48
#define MCU_CLOCK_HZ MCU_CLOCK_MHZ * 1000000

bool stm32f11re_init(void);
void udelay(uint32_t us);
