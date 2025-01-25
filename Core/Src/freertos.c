/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lights_functions.h"
#include "lights_test.h"
#include <semphr.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define PERIOD_BLUE_LIGHT 1000
#define PERIOD_RENDER_STATE 100
#define PERIOD_CHECK_INPUT 10
#define PERIOD_CHANGE_DIRECTION 8000
#define PERIOD_ORANGE 1000
#define PERIOD_PEDESTRIAN_WAIT 1000

#define PERIOD_COUNT_DOWN 100
#define PERIOD_TMR_FINISH 10

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

lstate_t state;
uint8_t north_p_waiting = 0;
uint8_t west_p_waiting = 0;
SemaphoreHandle_t stateMtx;

int32_t toggleFreq = 1000;    // ms
int32_t toggleCountDown = -1; // ms, -1 inactive

int32_t pedestrianDelay = 8000;   // ms

int32_t walkingDelay = 5000;   // ms

int32_t orangeDelay = 2000;   // ms


int32_t greenDelay = 8000;   // ms


int32_t redDelayMax = 10000; // ms
//int32_t redCountDown = -1;   // ms, -1 is inactive
//SemaphoreHandle_t redMtx;
/* USER CODE END Variables */
/* Definitions for ChangeDirection */
osThreadId_t ChangeDirectionHandle;
const osThreadAttr_t ChangeDirection_attributes = {
  .name = "ChangeDirection",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ToogleBlueLight */
osThreadId_t ToogleBlueLightHandle;
const osThreadAttr_t ToogleBlueLight_attributes = {
  .name = "ToogleBlueLight",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Orange */
osThreadId_t OrangeHandle;
const osThreadAttr_t Orange_attributes = {
  .name = "Orange",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for PedestrianWait */
osThreadId_t PedestrianWaitHandle;
const osThreadAttr_t PedestrianWait_attributes = {
  .name = "PedestrianWait",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for RenderState */
osThreadId_t RenderStateHandle;
const osThreadAttr_t RenderState_attributes = {
  .name = "RenderState",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for CheckInput */
osThreadId_t CheckInputHandle;
const osThreadAttr_t CheckInput_attributes = {
  .name = "CheckInput",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh4,
};
/* Definitions for queue_change_dir */
osMessageQueueId_t queue_change_dirHandle;
const osMessageQueueAttr_t queue_change_dir_attributes = {
  .name = "queue_change_dir"
};
/* Definitions for queue_pedestrian_wait */
osMessageQueueId_t queue_pedestrian_waitHandle;
const osMessageQueueAttr_t queue_pedestrian_wait_attributes = {
  .name = "queue_pedestrian_wait"
};
/* Definitions for queue_orange */
osMessageQueueId_t queue_orangeHandle;
const osMessageQueueAttr_t queue_orange_attributes = {
  .name = "queue_orange"
};
/* Definitions for queue_east_west_input */
osMessageQueueId_t queue_east_west_inputHandle;
const osMessageQueueAttr_t queue_east_west_input_attributes = {
  .name = "queue_east_west_input"
};
/* Definitions for queue_north_south_input */
osMessageQueueId_t queue_north_south_inputHandle;
const osMessageQueueAttr_t queue_north_south_input_attributes = {
  .name = "queue_north_south_input"
};
/* Definitions for myMutex01 */
osMutexId_t myMutex01Handle;
const osMutexAttr_t myMutex01_attributes = {
  .name = "myMutex01"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void change_direction(void *argument);
void toggle_blue_light(void *argument);
void orange_transition(void *argument);
void pedestrian_wait(void *argument);
void render_state(void *argument);
void check_input(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  state.t_north = green;
  state.t_south = green;
  state.t_east = red;
  state.t_west = red;
  state.p_north = red;
  state.p_west = red;
  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of myMutex01 */
  myMutex01Handle = osMutexNew(&myMutex01_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  stateMtx = xSemaphoreCreateMutex();

  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of queue_change_dir */
  queue_change_dirHandle = osMessageQueueNew (4, sizeof(direction_t), &queue_change_dir_attributes);

  /* creation of queue_pedestrian_wait */
  queue_pedestrian_waitHandle = osMessageQueueNew (4, sizeof(direction_t), &queue_pedestrian_wait_attributes);

  /* creation of queue_orange */
  queue_orangeHandle = osMessageQueueNew (4, sizeof(direction_t), &queue_orange_attributes);

  /* creation of queue_east_west_input */
  queue_east_west_inputHandle = osMessageQueueNew (4, sizeof(uint8_t), &queue_east_west_input_attributes);

  /* creation of queue_north_south_input */
  queue_north_south_inputHandle = osMessageQueueNew (4, sizeof(uint8_t), &queue_north_south_input_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ChangeDirection */
  ChangeDirectionHandle = osThreadNew(change_direction, NULL, &ChangeDirection_attributes);

  /* creation of ToogleBlueLight */
  ToogleBlueLightHandle = osThreadNew(toggle_blue_light, NULL, &ToogleBlueLight_attributes);

  /* creation of Orange */
  OrangeHandle = osThreadNew(orange_transition, NULL, &Orange_attributes);

  /* creation of PedestrianWait */
  PedestrianWaitHandle = osThreadNew(pedestrian_wait, NULL, &PedestrianWait_attributes);

  /* creation of RenderState */
  RenderStateHandle = osThreadNew(render_state, NULL, &RenderState_attributes);

  /* creation of CheckInput */
  CheckInputHandle = osThreadNew(check_input, NULL, &CheckInput_attributes);

  /* USER CODE BEGIN RTOS_THREADS */

  // Disable blinking for p_north and p_west

//xSemaphoreTake(okGoPedestrianNorthMtx, portMAX_DELAY);

// Allow inputs to be received
uint8_t input_goahead = 0b1;
osMessageQueuePut(queue_north_south_inputHandle, &input_goahead, 0, 0);
osMessageQueuePut(queue_east_west_inputHandle, &input_goahead, 0, 0);

  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */

	// NOTE: Test program here
	// test_program(&state);


  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_change_direction */
/**
  * @brief  Function implementing the ChangeDirection thread.
  * Checks if directions of current green lights should be changed.
  * Sends messages to orange_transition if lights are to be changed
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_change_direction */
void change_direction(void *argument)
{
  /* USER CODE BEGIN change_direction */

  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(PERIOD_CHANGE_DIRECTION); // ms to ticks
  // Initialize the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();

  /* Infinite loop */
  for(;;)
  {
	// Indicating whether or not traffic lights should stop as is to allow crossing to finish
	uint8_t stop = 0;

	// Check if any pedestrians want to cross;
	direction_t pedestrian;
	osStatus_t status = osMessageQueueGet(queue_change_dirHandle, &pedestrian, 0, 0);

	if(status == osOK){

		light_t *should_be_red;

		if(pedestrian == pnorth){
			should_be_red = &(state.t_north);
		} else{
			should_be_red = &(state.t_west);
		}

		if(*should_be_red == red){
			// Keep current state of car lights until crossing finished
			stop = 0b01;
		}
		else{
			// Try again next execution of task once directions had been switched
			stop = 0b10;
			osMessageQueuePut(queue_change_dirHandle, &pedestrian, 0, 0);
		}
	}

	uint8_t input = get_buttons();
	xSemaphoreTake(stateMtx, portMAX_DELAY);

	if(stop & 0b01){
		xSemaphoreGive(stateMtx);
		// pedestrian wants to cross
		direction_t pedestrian_go = ok;
		osMessageQueuePut(queue_pedestrian_waitHandle, &pedestrian_go, 0, portMAX_DELAY);

		// Wait until message is received signaling that crossing is done
		direction_t done;
		osMessageQueueGet(queue_change_dirHandle, &done, 0, portMAX_DELAY);

	}
	else if((((input & (BTN_T_NORTH | BTN_T_SOUTH) &&  !(input & (BTN_T_EAST | BTN_T_WEST))) && state.t_north == green) ||
			((input & (BTN_T_EAST | BTN_T_WEST) &&  !(input & (BTN_T_NORTH | BTN_T_SOUTH))) && state.t_west == green)) && !(stop & 0b10)){
		// Do not change lights since cars already has a green light and no other cars are waiting in other direction
	}
	else{
		// Switch lights to other direction since there is currently one of two things happening:
		// 		1. No cars are waiting and light switches back and fourth
		// 		2. Cars are waiting in both directions and light should switch back and fourth allowing all to pass
		direction_t from;
		direction_t to;
		if(state.t_north == green){
			from = north_south;
			to = east_west;
		}
		else{
			from = east_west;
			to = north_south;
		}
		xSemaphoreGive(stateMtx);

		// Turn other direction red
		osMessageQueuePut(queue_orangeHandle, &from, 0, 0);
		vTaskDelay(orangeDelay + 1000); // One extra second so that both are red for 1 second

		// Turn desired direction green
		osMessageQueuePut(queue_orangeHandle, &to, 0, 0);
		vTaskDelay(orangeDelay);
	}

	// Give back mutex if if-statement was not used
	xSemaphoreGive(stateMtx);

	  vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
  /* USER CODE END change_direction */
}

/* USER CODE BEGIN Header_toggle_blue_light */
/**
 * @brief Function implementing the ToogleBlueLight thread.
 * Toggles the blue light for each pedestrian light if there are pedestrians waiting there.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_toggle_blue_light */
void toggle_blue_light(void *argument)
{
  /* USER CODE BEGIN toggle_blue_light */
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(PERIOD_BLUE_LIGHT); // ms to ticks
  // Initialize the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for (;;) {

	 xSemaphoreTake(stateMtx, portMAX_DELAY);

	 if(north_p_waiting){
		 toggle_blue(&(state.p_north));
	 }
	 if(west_p_waiting){
		 toggle_blue(&(state.p_west));
	 }

	 xSemaphoreGive(stateMtx);


    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
  /* USER CODE END toggle_blue_light */
}

/* USER CODE BEGIN Header_orange_transition */
/**
* @brief Function implementing the Orange thread.
* If message is received, the task changes specified direction to orange and then to next state.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_orange_transition */
void orange_transition(void *argument)
{
  /* USER CODE BEGIN orange_transition */
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(PERIOD_ORANGE); // ms to ticks
  // Initialize the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {

	direction_t direction_to_change;
	osStatus_t status = osMessageQueueGet(queue_orangeHandle, &direction_to_change, 0, 0);

	if(status == osOK){
		xSemaphoreTake(stateMtx, portMAX_DELAY);

		light_t *light1;
		light_t *light2;

		// set what direction to change
		switch(direction_to_change){
			case north_south:
				light1 = &(state.t_north);
				light2 = &(state.t_south);
				break;
			case east_west:
				light1 = &(state.t_east);
				light2 = &(state.t_west);
				break;
			default:
				break;
		}

		light_t prev = *light1;
		light_t next;

		// If current lights should transition to green or to red
		if(prev == green)
			next = red;
		else
			next = green;

		*light1 = yellow;
		*light2 = yellow;

		xSemaphoreGive(stateMtx);
		vTaskDelay(orangeDelay);
		xSemaphoreTake(stateMtx, portMAX_DELAY);

		*light1 = next;
		*light2 = next;

		xSemaphoreGive(stateMtx);
	}

	vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
  /* USER CODE END orange_transition */
}

/* USER CODE BEGIN Header_pedestrian_wait */
/**
* @brief Function implementing the PedestrianWait thread.
* Checks if there are pedestrians waiting. If true then manages crossing and syncs with change_direction()
*
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pedestrian_wait */
void pedestrian_wait(void *argument)
{
  /* USER CODE BEGIN pedestrian_wait */
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(PERIOD_PEDESTRIAN_WAIT); // ms to ticks
  // Initialize the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {

	xSemaphoreTake(stateMtx, portMAX_DELAY);

	if(north_p_waiting || west_p_waiting){
		xSemaphoreGive(stateMtx);

		direction_t keep_direction;
		light_t *light_to_change;
		uint8_t * current_waiting;
		if(north_p_waiting){
			keep_direction = pnorth;
			light_to_change = &(state.p_north);
			current_waiting = &north_p_waiting;
		}
		else if(west_p_waiting){
			keep_direction = pwest;
			light_to_change = &(state.p_west);
			current_waiting = &west_p_waiting;
		}

		// Tell change direction task that pedestrian north/west wants to cross
		osMessageQueuePut(queue_change_dirHandle, &keep_direction, 0, 0);

		// Wait until car lights are in correct position, message will be sent from change_direction when state is ready
		direction_t go;
		osMessageQueueGet(queue_pedestrian_waitHandle, &go, 0, portMAX_DELAY);
		if(go == ok){

			xSemaphoreTake(stateMtx, portMAX_DELAY);
			// Stop blinking, also signifies that pedestrian crossing has been handled
			*current_waiting = 0;
			// Light is green
			*light_to_change = green;
			xSemaphoreGive(stateMtx);

			// Allow time pedestrians to cross
			vTaskDelay(walkingDelay);

			xSemaphoreTake(stateMtx, portMAX_DELAY);
			*light_to_change = red;
			xSemaphoreGive(stateMtx);

			// Tell change_direction task that crossing is done
			direction_t done = ok;
			osMessageQueuePut(queue_change_dirHandle, &done, 0, portMAX_DELAY);

		}

		// Stop blocking inputs from pedestrian crossing
		if(current_waiting == &north_p_waiting){
			uint8_t input_ok = 0b1;
			osMessageQueuePut(queue_north_south_inputHandle, &input_ok, 0, 0);
		}
		else{
			uint8_t input_ok = 0b1;
			osMessageQueuePut(queue_east_west_inputHandle, &input_ok, 0, 0);
		}

	}

	xSemaphoreGive(stateMtx);



	vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
  /* USER CODE END pedestrian_wait */
}

/* USER CODE BEGIN Header_render_state */
/**
 * @brief Function implementing the RenderState thread.
 * Renders the current logical state.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_render_state */
void render_state(void *argument)
{
  /* USER CODE BEGIN render_state */
  uint8_t bits[] = {0b0, 0b0, 0b0};

  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(PERIOD_RENDER_STATE); // ms to ticks
  // Initialize the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop*/
  for (;;) {

	// render current state
    xSemaphoreTake(stateMtx, portMAX_DELAY);
    format_lights(&state, bits);
    feed_shift_reg(bits);
    xSemaphoreGive(stateMtx);

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
  /* USER CODE END render_state */
}

/* USER CODE BEGIN Header_check_input */
/**
 * @brief Function implementing the CheckInput thread.
 * Checks if the buttons are pressed at either pedestrian crossing. Signals to other tasks if true.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_check_input */
void check_input(void *argument)
{
  /* USER CODE BEGIN check_input */

  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(PERIOD_CHECK_INPUT); // ms to ticks
  // Initialize the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();

  /* Infinite loop */
  for (;;) {
    uint8_t input = get_buttons();

    uint8_t input_goahead_n;
	osStatus_t status = osMessageQueueGet(queue_north_south_inputHandle, &input_goahead_n, 0, 0);

	// If input for the north pedestrian crossing is allowed
	if(status == osOK && input_goahead_n){

		if(input & BTN_P_NORTH){
			xSemaphoreTake(stateMtx, portMAX_DELAY);
			north_p_waiting = 1;
			xSemaphoreGive(stateMtx);

		}
		else{
			uint8_t input_goahead = 0b1;
			osMessageQueuePut(queue_north_south_inputHandle, &input_goahead, 0, 0);
		}
	}

    uint8_t input_goahead_w;
    osStatus_t status_again = osMessageQueueGet(queue_east_west_inputHandle, &input_goahead_w, 0, 0);

	// If input for the north pedestrian crossing is allowed
	if(status_again == osOK && input_goahead_w){

		if(input & BTN_P_WEST){
			xSemaphoreTake(stateMtx, portMAX_DELAY);
			west_p_waiting = 1;
			xSemaphoreGive(stateMtx);

		}
		else{
			uint8_t input_goahead = 0b1;
			osMessageQueuePut(queue_east_west_inputHandle, &input_goahead, 0, 0);
		}
	}

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
  /* USER CODE END check_input */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

