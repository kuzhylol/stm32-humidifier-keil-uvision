#include <stdbool.h>
#include <stdint.h>

#include "stm32f411xe.h"

/* GPIO BLOCK */
#define HS_GPIO_RCC_BUS 					AHB1ENR
#define	HS_RCC_AHBENR_GPIOEN 			RCC_AHB1ENR_GPIOBEN
#define	HS_GPIO_MODER 						(GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1)
#define	HS_GPIO_TYPER 						(GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9)
#define	HS_GPIO_SPEEDR 						(GPIO_OSPEEDER_OSPEEDR8_0 | GPIO_OSPEEDER_OSPEEDR8_1 | \
																	GPIO_OSPEEDER_OSPEEDR9_0 | GPIO_OSPEEDER_OSPEEDR9_1)
																	
#define	HS_GPIO_AFR 							(GPIO_AFRH_AFSEL9_2 | GPIO_AFRH_AFSEL8_2)

/* I2C BLOCK */
#define	HS_I2C_PE									I2C_CR1_PE
#define HS_I2C_RCC_BUS 						APB1ENR
#define	HS_RCC_APBENR_I2CEN 			RCC_APB1ENR_I2C1EN


#define HS_I2C_SWRST 							I2C_CR1_SWRST
#define HS_I2C_SMBUS 							I2C_CR1_SMBUS
#define HS_I2C_DUTY 							I2C_CCR_DUTY
#define HS_I2C_CR2_FREQ 					I2C_CR2_FREQ
#define HS_I2C_CCR 								I2C_CCR_CCR
#define HS_I2C_FS 								I2C_CCR_FS
#define HS_I2C_CCR_VAL 						240
#define HS_I2C_TRISE_VAL 					49
#define HS_I2C_EVENT_INTERRUPT 		(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN)


#define R	1
#define W ~R
#define HUM_SENSOR_I2C_ADDR 0xB8
#define HS_I2C_READ_REG_DATA 0x03


#define HUM_PKG				0
#define TEMP_PKG			1
#define HUM_TEMP_PKG	2

struct hum_temp_package{
	uint8_t func_code;
	uint8_t size;
	uint8_t h_high;
	uint8_t h_low;
	uint8_t t_high;
	uint8_t t_low;
	uint8_t crc_high;
	uint8_t crc_low;
};

struct AM2320 {
	
	int (*hs_get_hum_temp)(unsigned int *hum_tem);
	
};

typedef struct AM2320 HumSensor;

struct hs_i2c_regs {
	RCC_TypeDef *rcc_base;
	I2C_TypeDef *i2c1_base;
	GPIO_TypeDef *gpio_base;
};

static struct hs_i2c_regs hs_regs = {
	.rcc_base = RCC,
	.gpio_base = GPIOB,
	.i2c1_base = I2C1
};

void hs_init(HumSensor *chip);