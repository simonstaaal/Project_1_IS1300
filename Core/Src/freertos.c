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
#include "usart.h"
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

/** @name UART communication */
/** @{ */
uint8_t msg_failed = 0x00;  /**< Error code: Returns if Message-ID is invalid or value is outside limits */
uint8_t msg_success = 0x01; /**< Success code: Returns if received command is valid */
uint8_t rx_data[4];			/**< Buffer for incomming UART data. [0] = ID , [1] = 0x00 , [2],[3] = value */
ITStatus UartReady = RESET; /**< is set to SET by UART callback when a package is recieved. */
/** @} */

//Variables changed by UART
 uint32_t toggleFreq = 200;
 uint32_t pedestrianDelay = 5000;
 uint32_t walkingDelay = 4000;
 uint32_t orangeDelay = 3000;

//Delay variables for specific lights
uint32_t greenDelay = 3000;
uint32_t redDelay = 500;
uint32_t redDelayMax = 6000;


//flags for indication of incomming traffic/ pedestrians
uint8_t is_car_ns_waiting = 0;
uint8_t is_car_ew_waiting = 0;
uint8_t is_ped_ns_waiting = 0;
uint8_t ped_green = 0;



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
/* Definitions for SharedData */
osMutexId_t SharedDataHandle;
const osMutexAttr_t SharedData_attributes = {
  .name = "SharedData"
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
  /* Create the mutex(es) */
  /* creation of SharedData */
  SharedDataHandle = osMutexNew(&SharedData_attributes);

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

  // UART receive message
  HAL_UART_Receive_IT(&huart2, rx_data, 4);

  //Tick counter, this works as our clock in the system
  uint32_t start_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
  uint32_t current_time = 0;

  uint8_t blue_status = 0;
  uint8_t green_status = 0;

  static uint32_t blink_timer = 0;
  static uint32_t green_timer = 0;
  static Trafficlights states = NS_GREEN;

  static uint8_t n_ped_wait = NO;
  uint8_t car_ns = NO;
  uint8_t car_ew = NO;

  for(;;)
  {
	  if(UartReady == SET){

		  update_time(rx_data);
		  UartReady = RESET;
	  }


	  current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;

	  if(osMutexAcquire(SharedDataHandle, osWaitForever) == osOK){

		  car_ns = is_car_ns_waiting;
		  car_ew = is_car_ew_waiting;

		  if(is_ped_ns_waiting == YES){
		 	    n_ped_wait = YES;
		 	    is_ped_ns_waiting = NO;

		 }
		  osMutexRelease(SharedDataHandle);
	  }

	        update_traffic_states(&start_time, current_time,  &states,  &n_ped_wait,  car_ns,  car_ew);

	        if(n_ped_wait == YES){
	            blue_status = toggle_bluelights(&blink_timer, current_time);
	        }
	        else{
	        	blue_status = 0;
	        }
	        if(ped_green == YES){
	        	if(green_timer == 0){
	        		green_timer = current_time;
	        	}
	        	green_status = greenlights(&green_timer, current_time);

	        	if(green_status == 0){
	        		ped_green = NO;
	        		green_timer = 0;
	        	}
	        }else{
	        	green_status = 0;
	        }

	        update_lights_hardware(states, blue_status, green_status);


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
	uint8_t is_temp_nsPed_waiting = NO;
	uint8_t is_temp_ewCar_waiting = NO;
	uint8_t is_temp_nsCar_waiting = NO;

  /* Infinite loop */
  for(;;)
  {


	        if(read_input(NORTH_PEDESTRIAN) == DETECTED)
	        {
	            osDelay(20);
	            if(read_input(NORTH_PEDESTRIAN) == DETECTED)
	            {

	            	is_temp_nsPed_waiting = YES;
	            }


	        }

	        if((read_input(CAR_NORTH) == DETECTED) || (read_input(CAR_SOUTH) == DETECTED)){
	        	is_temp_nsCar_waiting = YES;
	        } else {
	        	is_temp_nsCar_waiting = NO;
	        }


	        if((read_input(CAR_WEST) == DETECTED) || (read_input(CAR_EAST) == DETECTED)){
	        	is_temp_ewCar_waiting = YES;
	        } else {
	        	is_temp_ewCar_waiting = NO;
	        }

	        if(osMutexAcquire(SharedDataHandle, osWaitForever) == osOK){
	        	is_car_ns_waiting = is_temp_nsCar_waiting;
	        	is_car_ew_waiting = is_temp_ewCar_waiting;

	        	if(is_temp_nsPed_waiting == YES){
	        		is_ped_ns_waiting = YES;
	        		is_temp_nsPed_waiting = NO;
	        	}
	        	osMutexRelease(SharedDataHandle);
	        }


	        osDelay(10); // Kör ca 100 gånger i sekunden
	    }


  /* USER CODE END Userinputs */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart2)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart2);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file.
   */
  UartReady = SET;


  HAL_UART_Receive_IT(huart2, rx_data, 4);
}

/* USER CODE END Application */

