#include <stdint.h>
#include <stdbool.h>
#include "stm32f411xe.h"
#include "piezo.h"

struct Humidifier {
	struct PiezoDevice *piezo;
	/* Other peripheral */
	
};

bool hum_init(struct Humidifier *hum);

