

#pragma once
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "main.h"
#include "spi.h"
#include "usart.h"

#include <stdint.h>

#define SHIFT_REG_CAPACITY 3
#define BTN_P_NORTH 0b00000001
#define BTN_P_WEST 0b00000010
#define BTN_T_NORTH 0b00000100
#define BTN_T_EAST 0b00001000
#define BTN_T_SOUTH 0b00010000
#define BTN_T_WEST 0b00100000

typedef enum { red, green, yellow, blue, off } light_t;

typedef enum {north_south, east_west, pnorth, pwest, none, ok} direction_t;

typedef struct lstate_t {
  light_t t_north;
  light_t t_south;
  light_t t_west;
  light_t t_east;
  light_t p_north;
  light_t p_west;
} lstate_t;


void feed_shift_reg(uint8_t *lights);
void format_lights(const lstate_t *curr, uint8_t *lights);
uint8_t get_buttons();
void toggle_blue(light_t *p_light);
//void pedestrian_go(light_t *p_light);
//void car_go(light_t *t_light);
