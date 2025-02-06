#include "pico/stdlib.h"
#include "hardware/timer.h"

#include <stdio.h>
#include <string.h>

#include "ws2812b_matrix.h"
#include "ssd1306.h"

#define LED_STRIP_PIN 7
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

#define DISPLAY_SDA_PIN 14
#define DISPLAY_SCL_PIN 15
#define DISPLAY_I2C_PORT i2c1
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DEBOUNCING_TIME_US 10000

static volatile _Atomic bool green_on = false;
static volatile _Atomic bool blue_on = false;
static volatile _Atomic char last_char = ' ';
static const char *led_msg = "EmbarcaTech U4 C6";
static ws2812b_matrix_t matrix;
static ssd1306_t display;
static LedDisplay digit_reprs[10];

static void die(const char *msg);
static void on_press(uint gpio, uint32_t events);
static bool update_display(struct repeating_timer *_);
static void init_digit_reprs(LedDisplay *reprs);

int main(void) {
	stdio_init_all();

	init_digit_reprs(digit_reprs);

	if (!ws2812b_matrix_init(&matrix, pio0, LED_STRIP_PIN))
		die("falha ao inicializar a matriz de LEDs");

	gpio_init(LED_GREEN_PIN);
	gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
	gpio_put(LED_GREEN_PIN, green_on);

	gpio_init(LED_BLUE_PIN);
	gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
	gpio_put(LED_BLUE_PIN, blue_on);

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

	if (!ssd1306_init(&display, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, 0x3C, DISPLAY_I2C_PORT))
		die("falha ao inicializar o display OLED");

	// limpar a tela
	ssd1306_fill(&display, 0);
	ssd1306_send_data(&display);

	struct repeating_timer timer;
	add_repeating_timer_ms(50, update_display, NULL, &timer);

	while (true)
		sleep_ms(10000);

	return 0;
}

static void die(const char *msg) {
	while (true) {
		printf("ERRO FATAL: %s\n", msg);
		sleep_ms(2000);
	}
}

static void on_press(uint gpio, uint32_t events) {
	static volatile uint32_t last_time_a = 0;
	static volatile uint32_t last_time_b = 0;
    uint32_t current_time = to_us_since_boot(get_absolute_time());

	if (gpio == BUTTON_A_PIN) {
		if (current_time - last_time_a > DEBOUNCING_TIME_US) {
			bool button_a_pressed = !gpio_get(BUTTON_A_PIN);
			if (button_a_pressed) {
				green_on = !green_on;
				gpio_put(LED_GREEN_PIN, green_on);
				led_msg = green_on ? "LED verde ligado" : "LED verde desligado";
			}
			last_time_a = current_time;
		}
	} else if (gpio == BUTTON_B_PIN) {
		if (current_time - last_time_b > DEBOUNCING_TIME_US) {
			bool button_b_pressed = !gpio_get(BUTTON_B_PIN);
			if (button_b_pressed) {
				blue_on = !blue_on;
				gpio_put(LED_BLUE_PIN, blue_on);
				led_msg = blue_on ? "LED azul ligado" : "LED azul desligado";
			}
			last_time_b = current_time;
		}
	}
}

static bool update_display(struct repeating_timer *_) {
    uint32_t time_ms = to_us_since_boot(get_absolute_time()) / 1000;
	bool bg_phase = (time_ms % 2000 < 1000);

	int result = stdio_getchar_timeout_us(0);
	if (result != PICO_ERROR_TIMEOUT)
		last_char = (char)result;

	ssd1306_fill(&display, !bg_phase);
	ssd1306_rect(&display, 3, 3, 122, 58, bg_phase, !bg_phase);

	uint8_t x = 8, y = 10;
	ssd1306_draw_string(&display, led_msg, &x, &y);

	x = 8, y += 8;
	ssd1306_draw_char(&display, last_char, x, y);

	ssd1306_send_data(&display);

	if (last_char >= '0' && last_char <= '9')
		ws2812b_matrix_draw(&matrix, &digit_reprs[last_char - '0']);

	return true;
}

static void init_digit_reprs(LedDisplay *reprs) {
	LedColor c0 = {0.0, 0.0, 0.0};
	LedColor c1 = {0.05, 0.05, 0.05};

	LedDisplay r0 = {
		c0, c1, c1, c1, c0,
		c0, c1, c0, c1, c0,
		c0, c1, c0, c1, c0,
		c0, c1, c0, c1, c0,
		c0, c1, c1, c1, c0,
	};

	LedDisplay r1 = {
		c0, c0, c0, c1, c0,
		c0, c0, c1, c1, c0,
		c0, c1, c0, c1, c0,
		c0, c0, c0, c1, c0,
		c0, c0, c0, c1, c0,
	};

	LedDisplay r2 = {
		c0, c1, c1, c0, c0,
		c0, c0, c0, c1, c0,
		c0, c0, c1, c0, c0,
		c0, c1, c0, c0, c0,
		c0, c1, c1, c1, c0,
	};

	LedDisplay r3 = {
		c0, c1, c1, c1, c0,
		c0, c0, c0, c1, c0,
		c0, c1, c1, c1, c0,
		c0, c0, c0, c1, c0,
		c0, c1, c1, c1, c0,
	};

	LedDisplay r4 = {
		c0, c1, c0, c1, c0,
		c0, c1, c0, c1, c0,
		c0, c1, c1, c1, c0,
		c0, c0, c0, c1, c0,
		c0, c0, c0, c1, c0,
	};

	LedDisplay r5 = {
		c0, c1, c1, c1, c0,
		c0, c1, c0, c0, c0,
		c0, c1, c1, c1, c0,
		c0, c0, c0, c1, c0,
		c0, c1, c1, c1, c0,
	};

	LedDisplay r6 = {
		c0, c1, c1, c1, c0,
		c0, c1, c0, c0, c0,
		c0, c1, c1, c1, c0,
		c0, c1, c0, c1, c0,
		c0, c1, c1, c1, c0,
	};

	LedDisplay r7 = {
		c0, c1, c1, c1, c0,
		c0, c0, c0, c1, c0,
		c0, c0, c0, c1, c0,
		c0, c0, c0, c1, c0,
		c0, c0, c1, c0, c0,
	};

	LedDisplay r8 = {
		c0, c1, c1, c1, c0,
		c0, c1, c0, c1, c0,
		c0, c1, c1, c1, c0,
		c0, c1, c0, c1, c0,
		c0, c1, c1, c1, c0,
	};

	LedDisplay r9 = {
		c0, c1, c1, c1, c0,
		c0, c1, c0, c1, c0,
		c0, c1, c1, c1, c0,
		c0, c0, c0, c1, c0,
		c0, c0, c0, c1, c0,
	};

	memcpy(reprs[0], r0, sizeof(LedDisplay));
	memcpy(reprs[1], r1, sizeof(LedDisplay));
	memcpy(reprs[2], r2, sizeof(LedDisplay));
	memcpy(reprs[3], r3, sizeof(LedDisplay));
	memcpy(reprs[4], r4, sizeof(LedDisplay));
	memcpy(reprs[5], r5, sizeof(LedDisplay));
	memcpy(reprs[6], r6, sizeof(LedDisplay));
	memcpy(reprs[7], r7, sizeof(LedDisplay));
	memcpy(reprs[8], r8, sizeof(LedDisplay));
	memcpy(reprs[9], r9, sizeof(LedDisplay));
}
