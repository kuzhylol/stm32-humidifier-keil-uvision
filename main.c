#include <stdint.h>
#include "stm32f411xe.h"
#include "mcu_init.h"
#include "wtim.h"
#include "utim.h"
#include "hum_sensor.h"
#include "piezo.h"

extern volatile bool ut_flag, wt_flag;

int main(void) {
	stm32f11re_init();
	__enable_irq();

	Piezo piezo;
	pz_init(&piezo);

	uTimer utimer;
	utim_init(&utimer, 10); // 1us * 10
	
	WideTimer wtimer;
	wtim_init(&wtimer, 5);
	
	utimer.start();

	wtimer.start();

	HumSensor chip;
	hs_init(&chip);
	
	udelay(100);

	unsigned int hum_temp[2] = {0};
	unsigned int hum = 0;
	unsigned int temp = 0;
	int ret = 0;
	
	while(1) {
		if (true == wt_flag) {
			piezo.toggle();
			ret = chip.hs_get_hum_temp(hum_temp);
			if (ret > 0) {
				hum = hum_temp[0];
				temp = hum_temp[1];
			}
			
			wt_flag = false;
		}
	}
}

