#ifndef _WS2812B_MATRIX_H
#define _WS2812B_MATRIX_H

#include "hardware/pio.h"

typedef struct ws2812b_matrix_t {
	PIO pio;
	uint pin;
	uint sm;
} ws2812b_matrix_t;

typedef struct LedColor {
	double r, g, b;
} LedColor;

typedef LedColor LedDisplay[25];

bool ws2812b_matrix_init(ws2812b_matrix_t *lm, PIO pio, uint pin);
void ws2812b_matrix_draw(ws2812b_matrix_t *lm, const LedDisplay *disp);

#endif
