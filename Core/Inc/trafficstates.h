/*
 * trafficstates.h
 *
 *  Created on: 28 Nov 2025
 *      Author: simonstal
 */

#ifndef INC_TRAFFICSTATES_H_
#define INC_TRAFFICSTATES_H_

#include "io_handler.h"
#include "test.h"

/*global variables used */
extern uint32_t pedestrianDelay;
extern uint32_t walkingDelay;
extern uint32_t orangeDelay;
extern uint32_t toggleFreq;
extern uint32_t greenDelay;
extern uint32_t redDelay;
extern uint32_t redDelayMax;


/*Traffic lights*/
typedef enum{
	NS_GREEN,
	NS_YELLOW,
	NS_RED,

	EW_GREEN,
	EW_YELLOW,
	EW_RED,

	//ALL_RED,
	NS_PED_GREEN


}Trafficlights;




void update_lights_hardware(Trafficlights states, uint8_t blue_status);

//void update_lights_hardware(Trafficlights states);
void update_traffic_states(uint32_t *start_time, uint32_t current_time, Trafficlights *states, uint8_t *n_ped_wait, uint8_t car_ns, uint8_t car_ew);
//void toggle_bluelights(uint32_t *blinkstart, uint32_t blinkstop);
uint8_t toggle_bluelights(uint32_t *blinkstart, uint32_t blinkstop);
#endif /* INC_TRAFFICSTATES_H_ */
