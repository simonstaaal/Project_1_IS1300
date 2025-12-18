/*
 * trafficstates.h
 *
 *  Created on: 28 Nov 2025
 *      Author: simonstal
 */

#ifndef INC_TRAFFICSTATES_H_
#define INC_TRAFFICSTATES_H_

#include "io_handler.h"

/** @name Traffic Light Bitmasks - East
 * @{ */
#define EAST_GREEN 0b00001100
#define EAST_YELLOW 0b00001010
#define EAST_RED 0b00001001
/** @} */


/** @name Traffic Light Bitmasks - West
 * @{ */
#define WEST_GREEN 0b00001100
#define WEST_YELLOW 0b00001010
#define WEST_RED 0b00001001
/** @} */

/** @name Traffic Light Bitmasks - North
 * @{ */
#define NORTH_GREEN 0b00100001
#define NORTH_YELLOW 0b00010001
#define NORTH_RED 0b000001001
/** @} */

/** @name Traffic Light Bitmasks - South
 * @{ */
#define SOUTH_GREEN 0b00001100
#define SOUTH_YELLOW 0b00001010
#define SOUTH_RED 0b00001001
/** @} */

/** @name Help constants
 * Readability macros for states and logic.
 * @{ */
#define DETECTED 0
#define NOT_DETECTED 1
#define YES 1
#define NO 0
/** @} */

/*global variables used */
extern uint32_t pedestrianDelay;
extern uint32_t walkingDelay;
extern uint32_t orangeDelay;
extern uint32_t toggleFreq;
extern uint32_t greenDelay;
extern uint32_t redDelay;
extern uint32_t redDelayMax;

extern uint8_t ped_green;
extern uint8_t msg_success;
extern uint8_t msg_failed;

/**
 * @brief Traffic lights states
 */
typedef enum{
	NS_GREEN,  	/**<  Green light for North and South trafficlight.*/
	NS_YELLOW,  /**< Yellow light for North and South trafficlight. */
	NS_RED,   	/**< Red light for North and South trafficlight. */
	EW_GREEN,	/**< Green light for East and West trafficlight. */
	EW_YELLOW,	/**< Yellow light for East and West trafficlight. */
	EW_RED		/**< Red light for East and West trafficlight. */
}Trafficlights;


/* Functions in trafficstate.c*/
uint8_t greenlights(uint32_t *greenstart, uint32_t greenstop);
void update_lights_hardware(Trafficlights states, uint8_t blue_status, uint8_t green_status);
void update_traffic_states(uint32_t *start_time, uint32_t current_time, Trafficlights *states, uint8_t *n_ped_wait, uint8_t car_ns, uint8_t car_ew);
uint8_t toggle_bluelights(uint32_t *blinkstart, uint32_t blinkstop);


#endif /* INC_TRAFFICSTATES_H_ */
