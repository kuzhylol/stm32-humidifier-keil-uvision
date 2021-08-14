#include <stdint.h>
#include "stm32f411xe.h"
#include "mcu_init.h"
#include "wtim.h"
#include "utim.h"
#include "hum_sensor.h"
#include "piezo.h"

extern volatile bool wt_flag;

int main(void) {
	stm32f11re_init();
	__enable_irq();

	Piezo piezo;
	pz_init(&piezo);
	
	WideTimer wtimer;
	wtim_init(&wtimer, 5);
	
	HumSensor chip;
	hs_init(&chip);

	uTimer utimer;
	utim_init(&utimer);

	volatile unsigned int hum_temp[2] = {0};
	volatile unsigned int hum = 0;
	volatile unsigned int temp = 0;
	int ret = 0;

	wtimer.start();

	while(1) {
		if (true == wt_flag) {
			wtimer.stop();
			ret = chip.hs_get_hum_temp(hum_temp);
			if (ret) {
				piezo.toggle();
				hum = hum_temp[0];
				temp = hum_temp[1];
			}
			wt_flag = false;
			wtimer.start();
		}
	}
}

