#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "address_map_nios2.h"

// Custom parameters for Gaussian Filter
#define SIGMA 0.6
#define KERNAL_SIZE 3 // Odd integers only

// Custom parameters for double threshold
// should change to calculate based off ratio
#define HIGH_THRESHOLD 0.8
#define LOW_THRESHOLD 0.5

// Classifications for edge tracking
#define STRONG_EDGE 0xFFFF
#define WEAK_EDGE 128


#define SCL_PIN 21
#define SDA_PIN 20

#define I2C_ADDR 0x21
#define I2C_DELAY 5

#define VS_PIN 19
#define HS_PIN 12
#define XLK_PIN 16
#define PLK_PIN 17

#define D0_PIN 22
#define D1_PIN 23
#define D2_PIN 24
#define D3_PIN 26
#define D4_PIN 27
#define D5_PIN 28
#define D6_PIN 29
#define D7_PIN 31

#define FRAME_PIN 13
#define PIX_PIN 15

#define Da_PIN 1
#define Db_PIN 0

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240

#define TIMERVAL 100

typedef struct GPIO
{
	volatile uint32_t data;
	volatile uint32_t direction;
	volatile uint32_t interrupt;
	volatile uint32_t edge;
} GPIO_t;

typedef struct timer
{
	volatile unsigned int status;
	volatile unsigned int control;
	volatile unsigned int periodlo;
	volatile unsigned int periodhi;
	volatile unsigned int snaplo;
	volatile unsigned int snaphi;
} timer_t;

typedef struct switches
{
	volatile uint32_t data;
} switches_t;

typedef struct LED
{
	volatile uint32_t data;
} LED_t;

extern GPIO_t *const GPIO_1;
extern timer_t* const timer;
extern switches_t *const sw;
extern LED_t *const LED;

extern volatile uint32_t pixel_buffer_start;
extern uint16_t Buffer1[240][512];
extern uint16_t Buffer2[240][512];

extern uint16_t input_frame[IMAGE_WIDTH * IMAGE_HEIGHT];
extern uint16_t grayscale[IMAGE_WIDTH * IMAGE_HEIGHT];
extern uint16_t blurred[IMAGE_WIDTH * IMAGE_HEIGHT];
extern uint16_t gradient[IMAGE_WIDTH * IMAGE_HEIGHT];
extern uint16_t suppressed[IMAGE_WIDTH * IMAGE_HEIGHT];
extern uint16_t thresholded[IMAGE_WIDTH * IMAGE_HEIGHT];
extern uint16_t hysteresis[IMAGE_WIDTH * IMAGE_HEIGHT];

extern int gx_buffer[IMAGE_WIDTH * IMAGE_HEIGHT];
extern int gy_buffer[IMAGE_WIDTH * IMAGE_HEIGHT];
extern int thresholds[IMAGE_WIDTH * IMAGE_HEIGHT];

extern uint16_t input_frame[IMAGE_WIDTH * IMAGE_HEIGHT];
extern grayscale[IMAGE_WIDTH * IMAGE_HEIGHT];
extern blurred[IMAGE_WIDTH * IMAGE_HEIGHT];
extern gradient[IMAGE_WIDTH * IMAGE_HEIGHT];
extern suppressed[IMAGE_WIDTH * IMAGE_HEIGHT];
extern thresholded[IMAGE_WIDTH * IMAGE_HEIGHT];
extern hysteresis[IMAGE_WIDTH * IMAGE_HEIGHT];

extern double gaussian_kernal[KERNAL_SIZE][KERNAL_SIZE];
extern int gx_buffer[IMAGE_WIDTH * IMAGE_HEIGHT];
extern int gy_buffer[IMAGE_WIDTH * IMAGE_HEIGHT];
extern int thresholds[IMAGE_WIDTH * IMAGE_HEIGHT];

void delay_us(uint32_t c);
uint32_t time_stamp();

void GPIO_setup(GPIO_t *port);
void write_I2C_byte(GPIO_t *port, uint8_t address, uint8_t byte);

void i2c_start(GPIO_t *port);
void i2c_stop(GPIO_t *port);
void i2c_send_bit(GPIO_t *port, bool bit);
void i2c_send_byte(GPIO_t *port, uint8_t byte);

uint32_t read_GPIO_word(GPIO_t *port);
void write_GPIO_word(GPIO_t *port, uint32_t word);
bool read_GPIO_bit(GPIO_t *port, uint8_t pin);
void write_GPIO_bit(GPIO_t *port, uint8_t pin, bool value);
void wait_for_rising_edge_GPIO(GPIO_t *port, uint8_t pin);
void wait_for_falling_edge_GPIO(GPIO_t *port, uint8_t pin);
uint16_t read_pixel(GPIO_t* port, uint8_t plk_pin);
uint16_t read_pixel2(GPIO_t *port);

void plot_pixel(uint32_t x, uint32_t y, uint16_t line_color);
void wait_for_vsync();
void clear_screen();

void build_gaussian_kernal();
void apply_gaussian_kernal();
void apply_grey_scale();
void apply_sobel_operator();
void apply_non_max_suppression();
void apply_double_threshold();
void apply_edge_tracking();

#endif
