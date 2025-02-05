#include "pico/stdlib.h"
#include <stdio.h>

#include "ws2812b_matrix.h"

#define LED_STRIP_PIN 7
#define LED_RED_PIN 12
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 13
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define OLED_DISPLAY_SDA_PIN 14
#define OLED_DISPLAY_SCL_PIN 15

static void die(const char *msg);
static void on_press(uint gpio, uint32_t events);

// TODO: fontes personalizadas

int main(void) {
	stdio_init_all();

	ws2812b_matrix_t mt;
	if (!ws2812b_matrix_init(&mt, pio0, LED_STRIP_PIN))
		die("falha ao inicializar a Matriz de LEDs");

	/* ssd1306_t oled; */
	// TODO: init ssd1306_t

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

	while (true) {
		sleep_ms(10000);
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
