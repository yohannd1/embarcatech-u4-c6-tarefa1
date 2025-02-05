#include "pico/stdlib.h"
#include <stdio.h>

int main(void) {
	stdio_init_all();

	while (true) {
		sleep_ms(10000);
	}

	return 0;
}
