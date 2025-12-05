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
#define ZERO 0

#define NORTH_GREEN 0b00100000  //Shiftregister 3
#define NORTH_YELLOW 0b00010001 //Shiftregister 3
#define NORTH_RED 0b000001001//Shiftregister  3



#define SOUTH_GREEN 0b00001000   //Shiftregister 2
#define SOUTH_YELLOW 0b00001010 //Shiftregister 2
#define SOUTH_RED 0b00001001 //Shiftregister 2

#define NORTH_PEDESTRIAN_RED 0b00001000 //Shiftregister 2

#define NORTH_BLUE 0b00100000 //Shiftregister 2


#define INIT_NORTH_GREEN 0b00100001 //sr3
#define INIT_SOUTH_GREEN 0b00001100 //sr2 pl2_red & south_green

#define PED_GREEN 0b00010001

#define PRESSED 0


#define WEST_GREEN 0b00001100 //sr3 gjort rätt kanske?
#define WEST_YELLOW 0b00001010 //sr3
#define WEST_RED 0b00001001 //sr3


// Rätt
#define EAST_GREEN 0b00001100 //sr1  pl1_red //kanske rätt
#define EAST_YELLOW 0b00001010 //sr1 pl1_red
#define EAST_RED 0b00001001 //sr1 pl1_red


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
