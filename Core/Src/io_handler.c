/*
 * io_handler.c
 *
 *  Created on: 28 Nov 2025
 *      Author: simonstal
 */
#include "io_handler.h"



/**
 * @brief Sends lightstatus to the traffic shields shiftregister via SPI.
 * Function packets tre bytes with data and transfer them to the shiftregister.

 *
 * @param r1 Data for the first register.
 * @param r2 Data for the second register.
 * @param r3 Data for the third register.
 */

void write_to_register(uint8_t r1,uint8_t r2, uint8_t r3) {

			uint8_t databits[3];

			databits[0] = r3;
			databits[1] = r2;
			databits[2] = r1;

		HAL_GPIO_WritePin(_595_Enable_GPIO_Port, _595_Enable_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(_595_Reset_GPIO_Port, _595_Reset_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(_595_STCP_GPIO_Port, _595_STCP_Pin, GPIO_PIN_RESET);


		HAL_SPI_Transmit(&hspi3, databits, 3, 1000);

		HAL_GPIO_WritePin(_595_STCP_GPIO_Port, _595_STCP_Pin, GPIO_PIN_SET);



}


/**
 * @brief Reads status from a specific button or switch.
 * Function returns status for the chosen input.
 *
 * @param button The input that should be read (t.ex. NORTH_PEDESTRIAN eller CAR_NORTH).
 * * @return bool
 * - true: Switch is on / button is pressed.
 * - false: Switch not on / button is not pressed.
 */


bool read_input(Inputs button){

	switch(button){

	case NORTH_PEDESTRIAN:
		return(HAL_GPIO_ReadPin(PL2_switch_GPIO_Port, PL2_switch_Pin));
		break;


	case CAR_NORTH:

		return(HAL_GPIO_ReadPin(TL4_Car_GPIO_Port, TL4_Car_Pin));
		break;
	case CAR_SOUTH:

		return(HAL_GPIO_ReadPin(TL2_Car_GPIO_Port, TL2_Car_Pin));
		break;

	case CAR_WEST:
		return(HAL_GPIO_ReadPin(TL1_Car_GPIO_Port, TL1_Car_Pin));
		break;

	case CAR_EAST:
		return(HAL_GPIO_ReadPin(TL3_Car_GPIO_Port, TL3_Car_Pin));
		break;



	}

	return false;


}


/**
 * @brief Updates time variables via UART.
 * Functions checks if value is not above the max limit. If the value is accepted the time variables are updated.
 * Otherwise a error message will be sent.
 * @param rx_data Pointer to recieved data:
 */

void update_time(uint8_t *rx_data){

	uint16_t value = (rx_data[2] << 8) | rx_data[3];
	uint16_t max_limit = 40000;

	if(value > max_limit){
		HAL_UART_Transmit(&huart2, &msg_failed, 1, HAL_MAX_DELAY);
		return;
	}

	if(rx_data[0] == 0x01){
		toggleFreq =  value;
		HAL_UART_Transmit(&huart2, &msg_success, 1,HAL_MAX_DELAY);
	}
	else if(rx_data[0] == 0x02){
		pedestrianDelay = value;
		HAL_UART_Transmit(&huart2, &msg_success, 1, HAL_MAX_DELAY);
	}
	else if(rx_data[0] ==0x03){
		walkingDelay = value;
		HAL_UART_Transmit(&huart2, &msg_success, 1, HAL_MAX_DELAY);
	}
	else if(rx_data[0] == 0x04){
		orangeDelay = value;
		HAL_UART_Transmit(&huart2, &msg_success, 1, HAL_MAX_DELAY);
	}
	else{
		HAL_UART_Transmit(&huart2, &msg_failed, 1, HAL_MAX_DELAY);
	}
}

