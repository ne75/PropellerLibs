#include <propeller.h>

#include "dht11.h"

DHT11 temp_sensor(0);

int main() {

	while (1) {
        temp_sensor.read();
        printf("T=%f, RH=%f\n", (float)temp_sensor.T, (float)temp_sensor.RH);
        waitcnt(CLKFREQ*3 + CNT);
	}
}