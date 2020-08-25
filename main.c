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

	//wtimer.start();

	hs_init();
	udelay(100);

	hs_wakeup();
	hs_write();
	//hs_read();
	//h_sensor_read(0x3);
	while(1) {
		//if (true == wt_flag) {
		//	piezo.toggle();
		//	wt_flag = false;
		//}
	}	
}

