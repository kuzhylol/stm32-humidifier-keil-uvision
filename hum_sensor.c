#include <stdint.h>
#include <stdbool.h>
#include "stm32f411xe.h"
#include "hum_sensor.h"
#include "mcu_init.h"
#include "systick.h"
#include <stdlib.h>

static inline void gpio_rcc_init(void)
{
	hs_regs.rcc_base->HS_GPIO_RCC_BUS |= HS_RCC_AHBENR_GPIOEN;
}

static inline void gpio_init(void)
{
	hs_regs.gpio_base->MODER 		|= 	HS_GPIO_MODER;
	hs_regs.gpio_base->OTYPER 	|= 	HS_GPIO_TYPER;
	hs_regs.gpio_base->OSPEEDR 	|= 	HS_GPIO_SPEEDR;
	hs_regs.gpio_base->AFR[1] 	|= 	HS_GPIO_AFR;
}

static void rcc_gpio_init(void)
{
	gpio_rcc_init();
	gpio_init();
}

static inline void i2c_rcc_init(void)
{
	hs_regs.rcc_base->HS_I2C_RCC_BUS |= HS_RCC_APBENR_I2CEN;
}

static inline void i2c_reset(void)
{
	hs_regs.i2c1_base->CR1 |= HS_I2C_SWRST;
	__NOP();
	hs_regs.i2c1_base->CR1 &= ~HS_I2C_SWRST;
}

static inline void hs_i2c_set_mode(void)
{
	hs_regs.i2c1_base->CR2 &= ~HS_I2C_SMBUS;
}

static inline void i2c_set_freq(void)
{
	/* duty cycle 50% */
	hs_regs.i2c1_base->CCR &= ~HS_I2C_DUTY;
	hs_regs.i2c1_base->CR2 &= ~HS_I2C_CR2_FREQ;
	
	/*  Peripheral clock frequency - 48 MHz*/
	hs_regs.i2c1_base->CR2 |= MCU_CLOCK_MHZ;

	hs_regs.i2c1_base->CCR &= ~HS_I2C_CCR;

	/* SM mode */
	hs_regs.i2c1_base->CCR &= ~HS_I2C_FS;

		hs_regs.i2c1_base->TRISE = (uint32_t) 0;

	/* Tscl/2Tpclk = 10us(SM)/(2*(1/48Mhz)) - 100kHz SM mode*/
	hs_regs.i2c1_base->CCR |= HS_I2C_CCR_VAL;
	
	hs_regs.i2c1_base->TRISE = (uint32_t) 0;
	/* (Trmax / Tpclk) + 1 = (1000nS (SM) / 1/48Mhz) + 1 = 48 */
	hs_regs.i2c1_base->TRISE |= HS_I2C_TRISE_VAL;
	
}

static inline void i2c_ack_enable(void)
{
	/*  Auto ACK enable */
	I2C1->CR1 |= (I2C_CR1_ACK);
}

static inline void i2c_ack_disable(void)
{
	/*  Auto ACK disable */
	I2C1->CR1 &= ~I2C_CR1_ACK;
}

static inline void i2c_enable(void)
{
	hs_regs.i2c1_base->CR1 |= HS_I2C_PE;
}

static inline void i2c_disable(void)
{	
	hs_regs.i2c1_base->CR1 &= ~HS_I2C_PE;
}

static inline void i2c_int_enable(void)
{
	hs_regs.i2c1_base->CR2 |= HS_I2C_EVENT_INTERRUPT;
}

static inline void i2c_int_disable(void)
{
	hs_regs.i2c1_base->CR2 &= ~HS_I2C_EVENT_INTERRUPT;
}

static void i2c_init(void)
{
	i2c_rcc_init();
	i2c_reset();
	i2c_disable();
	hs_i2c_set_mode();
	i2c_set_freq();
	i2c_int_enable();
}

static inline void i2c_busy_wait(void)
{
		while (I2C1->SR2 & I2C_SR2_BUSY) {
			__NOP();
		}
}

static inline void i2c_rx_wait(void)
{
		while(!(I2C1->SR1 & I2C_SR1_RXNE)) {
			__NOP();
		}
}
static inline void i2c_start(void)
{
	i2c_busy_wait();
	
	I2C1->CR1 |= I2C_CR1_START;

	/* Wait until Start bit completed */
	while (!(I2C1->SR1 & I2C_SR1_SB)) {
		__NOP();
	}
	(void) I2C1->SR1;
}

static inline void i2c_stop(void)
{
	I2C1->CR1 &= ~I2C_CR1_ACK; // Send NACK

	I2C1->CR1 |= I2C_CR1_STOP;
	udelay(160);
}

static inline void i2c_send_data(uint8_t word)
{
	I2C1->DR = word;
	udelay(8);

	(void) I2C1->SR1;
}

static inline void i2c_rd_addr(void)
{
	I2C1->CR1 |= I2C_CR1_ACK; // ACK Master receiver
	I2C1->DR = HUM_SENSOR_I2C_ADDR+R;
	udelay(6);
}

static void hs_wakeup(void)
{
	i2c_start();
	
	i2c_send_data(HUM_SENSOR_I2C_ADDR);
	
	udelay(90);
	
	i2c_stop();
}

static inline void i2c_set_addr(void)
{
		I2C1->DR = HUM_SENSOR_I2C_ADDR;
		while(!(I2C1->SR1 & I2C_SR1_ADDR));
		(void) I2C1->SR2;
}

bool hs_write(void)
{
		i2c_start();

		i2c_set_addr();

		i2c_send_data(0x03);

		i2c_send_data(0x00);

		i2c_send_data(0x04);

		i2c_stop();
}

void hs_read(uint8_t count, uint8_t *data)
{		
		i2c_start();
	
		i2c_set_read();
	
		for(uint8_t i = 0; i < count; i++) {
			while(!(I2C1->SR1 & I2C_SR1_RXNE));
				*(data++) = I2C1->DR;
		}
		i2c_stop();
}