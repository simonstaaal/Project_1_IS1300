/*
 * io_handler.h
 *
 *  Created on: 28 Nov 2025
 *      Author: simonstal
 */

#ifndef INC_IO_HANDLER_H_
#define INC_IO_HANDLER_H_

/*Header files*/
#include "stm32l4xx_hal.h"
#include "main.h"
#include "spi.h"
#include <stdbool.h>
#include "test.h"
#include "usart.h"


/**
 * @brief Different types of inputs from the traffic shield
 */
typedef enum{
	NORTH_PEDESTRIAN,	 /**< Pushbutton 7 and 8*/
	CAR_NORTH, 			/**< Switch 4 */
	CAR_SOUTH,		   /**< Switch 2 */
	CAR_WEST,		  /**< Switch 1 */
	CAR_EAST		 /**<  Switch 3*/
}Inputs;



void write_to_register(uint8_t r1,uint8_t r2, uint8_t r3);
bool read_input(Inputs button);
void update_time(uint8_t *rx_data);



#endif /* INC_IO_HANDLER_H_ */
