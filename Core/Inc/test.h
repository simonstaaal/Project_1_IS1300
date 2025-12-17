/*
 * test.h
 *
 *  Created on: 1 Dec 2025
 *      Author: simonstal
 */

#ifndef INC_TEST_H_
#define INC_TEST_H_

#include "main.h"
#include "io_handler.h"
#include "spi.h"
#include "trafficstates.h"
#include "trafficsystem.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32l4xx_hal.h"
#include "FreeRTOS.h"
#include "io_handler.h"
/*Bitcombinations for LEDS*/










#define PRESSED 0




// Rätt


// test defines
#define REGISTER_1












//void write_to_register(uint8_t r1,uint8_t r2, uint8_t r3);
//bool read_pedestrian_input(Pedestrian button);
void statemachine(void);
void initialstate(void);
void green_to_red(void);
void test_program(void);
void blink_blue(void);


#endif /* INC_TEST_H_ */
