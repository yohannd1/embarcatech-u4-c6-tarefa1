#include "pico/stdlib.h"
#include <stdio.h>

#include "ws2812b_matrix.h"
#include "ssd1306.h"

#define LED_STRIP_PIN 7
#define LED_RED_PIN 12
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 13
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

#define DISPLAY_SDA_PIN 14
#define DISPLAY_SCL_PIN 15
#define DISPLAY_I2C_PORT i2c1 // TODO: usar i2c0?
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

static void die(const char *msg);
static void on_press(uint gpio, uint32_t events);

// TODO: fontes personalizadas

int main(void) {
	stdio_init_all();

	ws2812b_matrix_t mt;
	if (!ws2812b_matrix_init(&mt, pio0, LED_STRIP_PIN))
		die("falha ao inicializar a matriz de LEDs");

	gpio_init(LED_RED_PIN);
	gpio_set_dir(LED_RED_PIN, GPIO_OUT);

	gpio_init(LED_GREEN_PIN);
	gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

	gpio_init(LED_BLUE_PIN);
	gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);

	gpio_init(BUTTON_A_PIN);
	gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
	gpio_pull_up(BUTTON_A_PIN);

	gpio_init(BUTTON_B_PIN);
	gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
	gpio_pull_up(BUTTON_B_PIN);

	gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &on_press);
	gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &on_press);

	// inicializar o I2C para o display
	i2c_init(DISPLAY_I2C_PORT, 400000); // 400KHz
	gpio_set_function(DISPLAY_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(DISPLAY_SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(DISPLAY_SDA_PIN);
	gpio_pull_up(DISPLAY_SCL_PIN);

	ssd1306_t disp;
	if (!ssd1306_init(&disp, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, 0x3C, DISPLAY_I2C_PORT))
		die("falha ao inicializar o display OLED");

	ssd1306_send_data(&disp);
	ssd1306_fill(&disp, false);
	ssd1306_send_data(&disp);

	uint32_t counter = 0;
	while (true) {
		bool even = (counter % 2) == 0;

		ssd1306_fill(&disp, !even);
		ssd1306_rect(&disp, 3, 3, 122, 58, even, !even);
		ssd1306_draw_string(&disp, "AAA", 8, 10);
		ssd1306_draw_string(&disp, "BBB", 20, 30);
		ssd1306_draw_string(&disp, "CCC", 15, 48);
		ssd1306_send_data(&disp);

		sleep_ms(1000);

		counter++;
	}

	return 0;
}

static void die(const char *msg) {
	while (true) {
		printf("ERRO FATAL: %s\n", msg);
		sleep_ms(2000);
	}
}

static void on_press(uint gpio, uint32_t events) {
	// TODO: process (w/ debouncing)
}
