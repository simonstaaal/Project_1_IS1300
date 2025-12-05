/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "trafficstates.h"
#include "io_handler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint32_t pedestrianDelay = 10000;
uint32_t walkingDelay = 4000;
uint32_t orangeDelay = 500;
uint32_t toggleFreq = 200;
uint32_t greenDelay = 5000;
uint32_t redDelay = 500;
uint32_t redDelayMax = 6000;


volatile uint8_t car_ns_waiting = 0;
volatile uint8_t car_ew_waiting = 0;
volatile uint8_t ped_ns_waiting = 0;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TrafficStates */
osThreadId_t TrafficStatesHandle;
const osThreadAttr_t TrafficStates_attributes = {
  .name = "TrafficStates",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UserInput */
osThreadId_t UserInputHandle;
const osThreadAttr_t UserInput_attributes = {
  .name = "UserInput",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void TrafficState(void *argument);
void Userinputs(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of TrafficStates */
  TrafficStatesHandle = osThreadNew(TrafficState, NULL, &TrafficStates_attributes);

  /* creation of UserInput */
  UserInputHandle = osThreadNew(Userinputs, NULL, &UserInput_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_TrafficState */
/**
* @brief Function implementing the TrafficStates thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TrafficState */
void TrafficState(void *argument)
{
  /* USER CODE BEGIN TrafficState */
  /* Infinite loop */

  //Timer for switching states
  uint32_t start_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
  uint32_t current_time = 0;
  uint8_t blue_status = 0;
  static uint32_t blink_timer = 0;
  static Trafficlights states = NS_GREEN;
  static uint8_t n_ped_wait = 0;

  for(;;)
  {

	  current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;



	        uint8_t car_ns = car_ns_waiting;
	        uint8_t car_ew = car_ew_waiting;

	        if(ped_ns_waiting == 1){
	             n_ped_wait = 1;
	             ped_ns_waiting = 0;

	        }


	        update_traffic_states(&start_time, current_time,  &states,  &n_ped_wait,  car_ns,  car_ew);



	        // Hantera blåljus här (som vi diskuterade förut)
	        if(n_ped_wait){
	            blue_status = toggle_bluelights(&blink_timer, current_time);
	        }
	        else{
	        	blue_status = 0;
	        }
	        update_lights_hardware(states, blue_status);
	       //n_ped_wait = 0;

	        osDelay(10);

  }
  /* USER CODE END TrafficState */
}

/* USER CODE BEGIN Header_Userinputs */
/**
* @brief Function implementing the UserInput thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Userinputs */
void Userinputs(void *argument)
{
  /* USER CODE BEGIN Userinputs */

  /* Infinite loop */
  for(;;)
  {
	        if(read_input(NORTH_PEDESTRIAN) == PRESSED)
	        {
	            osDelay(20); // Enkel avstudsning (vänta ut studsar)
	            if(read_input(NORTH_PEDESTRIAN) == PRESSED)
	            {

	                ped_ns_waiting = 1;
	            }


	        }

	        if((read_input(CAR_NORTH) == PRESSED) || (read_input(CAR_SOUTH) == PRESSED)){
	            car_ns_waiting = 1;
	        } else {
	            car_ns_waiting = 0;
	        }


	        if((read_input(CAR_WEST) == PRESSED) || (read_input(CAR_EAST) == PRESSED)){
	            car_ew_waiting = 1;
	        } else {
	            car_ew_waiting = 0;
	        }

	        osDelay(10); // Kör ca 100 gånger i sekunden
	    }


  /* USER CODE END Userinputs */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

