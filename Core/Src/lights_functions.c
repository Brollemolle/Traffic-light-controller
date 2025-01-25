/**
******************************************************************************
@brief
Traffic lights, functions for controlling traffic lights.
@file
lights_functions.c
@author Adam Brolin
@version 1.0
@date
03-December-2024
@brief
Functions and structures for controlling and handling flow of traffic lights.
******************************************************************************
*/

#pragma once
#include "lights_functions.h"

/**
@brief feed_shift_reg, feed shift register with 24 bit input.
@param uint8_t *lights, pointer to 24 bits divided into 3 bytes.
Each bit represent an led on the 74HC595D shield, some bits are unmapped.
@return void, no return value
*/
void feed_shift_reg(uint8_t *lights) {
  HAL_SPI_Transmit(&hspi3, lights, SHIFT_REG_CAPACITY, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(STCP_GPIO_Port, STCP_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(STCP_GPIO_Port, STCP_Pin, GPIO_PIN_RESET);
}

/**
@brief format_lights, transform state of traffic lights to 24 bit input.
@param const lstate_t *curr, the current state of the traffic intersection.
@param uint8_t *lights, location where 24 bit value will return.
@return void, no return value.
*/
void format_lights(const lstate_t *curr, uint8_t *lights) {
  // Set all bits to 0
  lights[0] = 0b00000000;
  lights[1] = 0b00000000;
  lights[2] = 0b00000000;

  switch (curr->t_north) {
  case green:
    lights[0] = lights[0] | 0b00000100;
    break;
  case yellow:
    lights[0] = lights[0] | 0b00001000;
    break;
  case red:
    lights[0] = lights[0] | 0b00010000;
    break;
  default:
    break;
  }

  switch (curr->t_east) {
  case green:
    lights[0] = lights[0] | 0b00100000;
    break;
  case yellow:
    lights[0] = lights[0] | 0b01000000;
    break;
  case red:
    lights[0] = lights[0] | 0b10000000;
    break;
  default:
    break;
  }

  switch (curr->t_south) {
  case green:
    lights[1] = lights[1] | 0b00100000;
    break;
  case yellow:
    lights[1] = lights[1] | 0b01000000;
    break;
  case red:
    lights[1] = lights[1] | 0b10000000;
    break;
  default:
    break;
  }

  switch (curr->t_west) {
  case green:
    lights[2] = lights[2] | 0b00100000;
    break;
  case yellow:
    lights[2] = lights[2] | 0b01000000;
    break;
  case red:
    lights[2] = lights[2] | 0b10000000;
    break;
  default:
    break;
  }

  switch (curr->p_north) {
  case green:
    lights[1] = lights[1] | 0b00001000;
    break;
  case red:
    lights[1] = lights[1] | 0b00010000;
    break;
  case blue:
    // Blue will only be enabled when red is
    lights[1] = lights[1] | 0b00010100;
    break;
  default:
    break;
  }

  switch (curr->p_west) {
  case green:
    lights[2] = lights[2] | 0b00001000;
    break;
  case red:
    lights[2] = lights[2] | 0b00010000;
    break;
  case blue:
    // Blue will only be enabled when red is
    lights[2] = lights[2] | 0b00010100;
    break;
  default:
    break;
  }
}

/**
@brief get_buttons, returns an 8-bit integer. Bits 0-5 represent one button
each.
@param void, no parameter.
@return uint8_t, bits 0-5 represent one button each.
Use macros BTN_X_XXXX with bit-wise AND to check for specific button.
1 is on, 0 is off.
*/
uint8_t get_buttons() {

  uint8_t out = 0;

  if (HAL_GPIO_ReadPin(P_North_GPIO_Port, P_North_Pin) == GPIO_PIN_RESET)
    out |= BTN_P_NORTH;
  if (HAL_GPIO_ReadPin(P_West_GPIO_Port, P_West_Pin) == GPIO_PIN_RESET)
    out |= BTN_P_WEST;

  if (HAL_GPIO_ReadPin(T_North_GPIO_Port, T_North_Pin) == GPIO_PIN_RESET)
    out |= BTN_T_NORTH;
  if (HAL_GPIO_ReadPin(T_East_GPIO_Port, T_East_Pin) == GPIO_PIN_RESET)
    out |= BTN_T_EAST;
  if (HAL_GPIO_ReadPin(T_South_GPIO_Port, T_South_Pin) == GPIO_PIN_RESET)
    out |= BTN_T_SOUTH;
  if (HAL_GPIO_ReadPin(T_West_GPIO_Port, T_West_Pin) == GPIO_PIN_RESET)
    out |= BTN_T_WEST;

  return out;
}

/**
@brief toggle_blue, change logical state of ligh to red if currently blue
and blue if it is currently red. Does nothing if state is neither red nor blue.
@note Logical blue represents both red and blue led on.
@param light_t *p_light, pointer to the logical state of a pedestrian light.
@return void, no return value.
*/
void toggle_blue(light_t *p_light) {
  if (*p_light == blue)
    *p_light = red;
  else if (*p_light == red)
    *p_light = blue;
}

pedestrian_go(light_t *p_light) {}
