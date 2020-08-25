#include <stdint.h>
#include <stdbool.h>
#include "stm32f411xe.h"

#define PZ_RCC_BASE 								RCC_BASE
#define	PZ_GPIO_BASE 							GPIOC_BASE
#define PZ_RCC_BUS 								AHB1ENR
#define	PZ_RCC_AHB1ENR_GPIOCEN 		RCC_AHB1ENR_GPIOCEN
#define	PZ_PIN 										8;
#define	PZ_GPIO_MODER_MODER	 			GPIO_MODER_MODER8_0
#define	PZ_GPIO_OSPEEDR_OSPEED 		GPIO_OSPEEDR_OSPEED8_0
#define	PZ_GPIO_ODR								GPIO_ODR_OD8
	
typedef struct PiezoControl {
	void (*on)(void);
	void (*off)(void);
	void (*toggle)(void);
}Piezo;
	
struct piezo_regs {
	RCC_TypeDef *rcc_base;
	GPIO_TypeDef *gpio_base;
};

static struct piezo_regs p_regs = {
	.rcc_base = RCC,
	.gpio_base = GPIOC
};


void pz_init(Piezo *pz);

