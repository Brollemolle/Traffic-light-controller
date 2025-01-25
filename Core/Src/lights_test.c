/**
******************************************************************************
@brief
Traffic light tests, functions for testing traffic light functions.
@file
lights_test.c
@author Adam Brolin
@version 1.0
@date
08-December-2024
@brief
Enable testing program on line 259 in freertos.c
******************************************************************************
*/

#pragma once
#include "lights_test.h"
#include <stdint.h>

void test_program(lstate_t *state) {
  /*state->t_north = off;*/
  /*state->t_east = off;*/
  /*state->t_south = off;*/
  /*state->t_west = off;*/
  /*state->p_north = blue;*/
  /*state->p_west = red;*/

  while (1) {

  // test_feed_shift_ref(); // PERF: Passed
  // test_format_lights(); // PERF: Passed
  test_get_buttons(state); // PERF: Passed
  // test_toggle_blue(state); // PERF: Passed
  //
  }
}

void test_feed_shift_ref() {

  uint8_t bits[] = {0b0, 0b0, 0b0};

  for (int i = 0; i < 3; i++) {

    bits[i] = 0b00000001;

    for (int j = 0; j < 8; j++) {
      feed_shift_reg(bits);
      bits[i] = bits[i] << 1;
      HAL_Delay(1000);
    }
  }

  bits[0] = 0b11111111;
  bits[1] = 0b11111111;
  bits[2] = 0b11111111;
  feed_shift_reg(bits);
  HAL_Delay(2000);
}

test_format_lights() {
  uint8_t bits[] = {0b0, 0b0, 0b0};

  lstate_t curr;
  curr.t_north = red;
  curr.t_east = yellow;
  curr.t_south = green;
  curr.t_west = off;
  curr.p_north = red;
  curr.p_west = green;

  format_lights(&curr, bits);
  feed_shift_reg(bits);
  HAL_Delay(100);

  curr.t_north = yellow;
  curr.t_east = green;
  curr.t_south = off;
  curr.t_west = red;
  curr.p_north = green;
  curr.p_west = blue;

  format_lights(&curr, bits);
  feed_shift_reg(bits);
  HAL_Delay(100);

  curr.t_north = green;
  curr.t_east = off;
  curr.t_south = red;
  curr.t_west = yellow;
  curr.p_north = blue;
  curr.p_west = red;

  format_lights(&curr, bits);
  feed_shift_reg(bits);
  HAL_Delay(100);

  curr.t_north = off;
  curr.t_east = red;
  curr.t_south = yellow;
  curr.t_west = green;
  curr.p_north = off;
  curr.p_west = off;

  format_lights(&curr, bits);
  feed_shift_reg(bits);
  HAL_Delay(100);
}

void test_get_buttons(lstate_t *state) {
  if (get_buttons() & BTN_P_NORTH)
    state->p_north = green;
  else
    state->p_north = off;

  if (get_buttons() & BTN_P_WEST)
    state->p_west = blue;
  else
    state->p_west = off;

  if (get_buttons() & BTN_T_NORTH)
    state->t_north = green;
  else
    state->t_north = off;

  if (get_buttons() & BTN_T_EAST)
    state->t_east = green;
  else
    state->t_east = off;

  if (get_buttons() & BTN_T_SOUTH)
    state->t_south = green;
  else
    state->t_south = off;

  if (get_buttons() & BTN_T_WEST)
    state->t_west = green;
  else
    state->t_west = off;

  uint8_t bits[] = {0b0, 0b0, 0b0};
  format_lights(state, bits);
  feed_shift_reg(bits);
}

void test_toggle_blue(lstate_t *state) {
  toggle_blue(&(state->p_north));
  toggle_blue(&(state->p_west));

  uint8_t bits[] = {0b0, 0b0, 0b0};
  format_lights(state, bits);
  feed_shift_reg(bits);
  HAL_Delay(1000);
}
