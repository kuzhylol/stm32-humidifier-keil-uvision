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
	(void)I2C1->SR1;
}

static inline void i2c_stop(void)
{
	I2C1->CR1 |= I2C_CR1_STOP;
	utim_udelay(1600);
}

static inline void i2c_send_data(uint8_t word)
{
	I2C1->DR = word;
	utim_udelay(80);

	(void)I2C1->SR1;
}

static inline void i2c_rd_addr(void)
{
	I2C1->DR = HS_I2C_ADDR+R;
	utim_udelay(60);
}

static void hs_wakeup(void)
{
	i2c_start();
	
	i2c_send_data(HS_I2C_ADDR);
	
	utim_udelay(1500);
	
	i2c_stop();
}

static inline void i2c_wr_addr(void)
{
		I2C1->DR = HS_I2C_ADDR+W;
		while(!(I2C1->SR1 & I2C_SR1_ADDR)) {
			__NOP();
		}
		(void)I2C1->SR2;
}

static void i2c_fetch_data_reg(uint8_t *flow_buff, uint8_t npkg) 
{
	for(unsigned int i = 0; i < npkg; i++) {
		i2c_rx_wait();
		flow_buff[i] = (uint8_t)I2C1->DR;
	}
}

static void read_target_package(uint8_t *buff, uint8_t n)
{
	if (NULL == buff) {
		return;
	}
	i2c_enable();

	hs_wakeup();
	
	i2c_start();

	i2c_wr_addr();
	i2c_send_data(HS_I2C_READ_REG_DATA);
	i2c_send_data(HS_I2C_START_REG_DATA); // Start address
	i2c_send_data(n-4); // load data only (humidity 2 bytes + temp 2 bytes)

	i2c_stop();
	
	i2c_ack_enable();

	i2c_start();
	
	i2c_rd_addr();

	i2c_fetch_data_reg(buff, n);

	i2c_ack_disable();

	i2c_stop();
	
	i2c_disable();
}

static void read_sys_package(uint8_t *buff, uint8_t n)
{
	if (NULL == buff) {
		return;
	}
	i2c_enable();

	hs_wakeup();
	
	i2c_start();

	i2c_wr_addr();
	i2c_send_data(0x03);
	i2c_send_data(0x0A); // Start address
	i2c_send_data(n-2-2); // - system bytes - crc

	i2c_stop();
	
	i2c_ack_enable();

	i2c_start();
	
	i2c_rd_addr();

	i2c_fetch_data_reg(buff, n);

	i2c_ack_disable();

	i2c_stop();
	
	i2c_disable();
}

static uint16_t crc16(uint8_t *ptr, uint8_t len)
{
	uint16_t crc = 0xFFFF;
	uint8_t i;
	while(len--) {
		crc ^= *(ptr++);
		for(i = 0 ; i < 8 ; i++) {
			if(crc & 0x01) {
				crc >>= 1;
				crc ^= 0xA001;
			} else {
				crc >>= 1;
			}
		}
	}
	return crc;
}

static uint16_t concat_bytes(uint8_t a, uint8_t b)
{
	return (uint16_t)((a) | (b << 8));
}

static inline bool crc_check(struct hum_temp_package *package)
{
	uint8_t pkg_valid_data[]= {
		package->func_code,
		package->size,
		package->h_high,
		package->h_low,
		package->t_high,
		package->t_low,
	};
	
	/* store 8 bit high and 8 bit low at 16 bit crc variable */
	uint16_t crc_fetched  = concat_bytes(package->crc_high, package->crc_low);
	uint16_t crc_computed = crc16(pkg_valid_data, sizeof(pkg_valid_data));
	
	bool ret = (crc_computed == crc_fetched) ? true : false;
	
	return ret;
}

static bool get_hum_temp(unsigned int *hum_temp)
{
	if (NULL == hum_temp) {
		return false;
	}
	
	uint8_t pkg_buff[8];
	read_target_package(pkg_buff, (sizeof(pkg_buff)));
	
	struct hum_temp_package *parsed = (struct hum_temp_package *)pkg_buff;

	if (false == crc_check(parsed)) {
		return false;
	}
	
	unsigned int humidity = concat_bytes(parsed->h_low, parsed->h_high);
	unsigned int temperature = concat_bytes(parsed->t_low, parsed->t_high);

	hum_temp[0] = humidity; hum_temp[1] = temperature;
	
	return true;
}

static bool make_probe(void)
{
	uint8_t dev_id_pckg[5];
	read_sys_package(dev_id_pckg, (sizeof(dev_id_pckg)));
	
	uint8_t dev_id = (dev_id_pckg[2]);

	if ((dev_id <= 0) || (dev_id == 0xFF)) {
		return false;
	}
	
	utim_udelay(3000);
	return true;
}

void hs_init(HumSensor *chip)
{
	rcc_gpio_init();
	i2c_init();
	
	chip->hs_get_hum_temp = &get_hum_temp;
	chip->probe = &make_probe;
}


