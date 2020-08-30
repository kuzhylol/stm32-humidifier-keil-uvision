#include <stdbool.h>
#include <stdint.h>

#define R	1
#define W 0
#define HUM_SENSOR_I2C_ADDR 0xB8

bool hs_init(void);
void hs_wakeup(void);
void hs_read(uint8_t count, uint8_t *data);
bool hs_write(void);