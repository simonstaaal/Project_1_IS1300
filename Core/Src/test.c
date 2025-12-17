/*
 * test.c
 *
 *  Created on: 1 Dec 2025
 *      Author: simonstal
 */
#include "test.h"
#include "trafficstates.h"
#include "io_handler.h"

/* */


/*
void write_to_register(uint8_t r1,uint8_t r2, uint8_t r3) {

			uint8_t databits[3];

			databits[0] = r3; //register 3
			databits[1] = r2;
			databits[2] = r1;

		HAL_GPIO_WritePin(_595_Enable_GPIO_Port, _595_Enable_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(_595_Reset_GPIO_Port, _595_Reset_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(_595_STCP_GPIO_Port, _595_STCP_Pin, GPIO_PIN_RESET);


		HAL_SPI_Transmit(&hspi3, databits, 3, 1000);

		HAL_GPIO_WritePin(_595_STCP_GPIO_Port, _595_STCP_Pin, GPIO_PIN_SET);

		  //HAL_Delay(100);

}
*/

/*
bool read_pedestrian_input(Pedestrian button){

	switch(button){

	case NORTH_PEDESTRIAN:
		return(HAL_GPIO_ReadPin(PL2_switch_GPIO_Port, PL2_switch_Pin));
		break;

	}

	return false;


}
*/

/*
void statemachine(void){

	static Trafficlights states = STATE_INIT;

	 bool northpedestrian = read_pedestrian_input(NORTH_PEDESTRIAN);

	 if(northpedestrian == PRESSED){
		 states = STATE_NS_PEDESTRIAN;
	 }

	switch(states){

	case STATE_INIT:
		//Userinputs();
		break;

	case STATE_NS_PEDESTRIAN:
		blink_blue();
		//green_to_red();
		states = STATE_INIT;
		break;

	}
}
*/

/*
void initialstate(void){

	write_to_register(ZERO, SOUTH_GREEN,NORTH_GREEN);

}

void blink_blue(void) {
		write_to_register(ZERO,ZERO,NORTH_RED);

}

void green_to_red(void){


	write_to_register(ZERO, SOUTH_YELLOW, NORTH_YELLOW);

}

void pedestrian_is_waiting(void){

}

void test_program(void)
{
	while(1){

	statemachine();
	//green_to_red();

	bool north_ped = read_pedestrian_input(NORTH_PEDESTRIAN);

	if(north_ped == 0){
		write_to_register(ZERO,NORTH_GREEN,SOUTH_GREEN);
	}

	}
	}
}
*/
