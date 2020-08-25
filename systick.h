#include <stdint.h>
#include <stdbool.h>

extern volatile uint32_t jiffies;

bool systick_init(const uint16_t div);
bool sys_delay_ms(const uint32_t ms);