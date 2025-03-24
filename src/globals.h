#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "address_map_nios2.h"

#define SCL_PIN 21
#define SDA_PIN 20

#define VS_PIN 19
#define HS_PIN 18
#define XLK_PIN 16
#define PLK_PIN 17

#define D0_PIN 24
#define D1_PIN 25
#define D2_PIN 26
#define D3_PIN 27
#define D4_PIN 28
#define D5_PIN 29
#define D6_PIN 30
#define D7_PIN 31

#define FRAME_PIN 13
#define PIX_PIN 15

#define Da_PIN 1
#define Db_PIN 0

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

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

extern volatile int pixel_buffer_start;
extern short int Buffer1[240][512];
extern short int Buffer2[240][512];

void delay_us(uint32_t c);
uint32_t time_stamp();

void GPIO_setup(GPIO_t *port);
// void write_I2C_byte(GPIO_t *port, uint8_t address, uint8_t byte);

uint32_t read_GPIO_word(GPIO_t *port);
void write_GPIO_word(GPIO_t *port, uint32_t word);
bool read_GPIO_bit(GPIO_t *port, uint8_t pin);
void write_GPIO_bit(GPIO_t *port, uint8_t pin, bool value);
void wait_for_rising_edge_GPIO(GPIO_t *port, uint8_t pin);
void wait_for_falling_edge_GPIO(GPIO_t *port, uint8_t pin);
uint16_t read_pixel(GPIO_t *port);

void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();
void clear_screen();

#endif
