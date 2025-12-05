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

/*inputs*/
typedef enum{
	NORTH_PEDESTRIAN,
	WEST_PEDESTRIAN,
	CAR_NORTH,
	CAR_SOUTH,
	CAR_WEST,
	CAR_EAST
}Inputs;



void write_to_register(uint8_t r1,uint8_t r2, uint8_t r3);
bool read_input(Inputs button);




#endif /* INC_IO_HANDLER_H_ */
