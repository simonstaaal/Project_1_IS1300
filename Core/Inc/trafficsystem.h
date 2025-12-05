/*
 * trafficsystem.h
 *
 *  Created on: 28 Nov 2025
 *      Author: simonstal
 */

#ifndef INC_TRAFFICSYSTEM_H_
#define INC_TRAFFICSYSTEM_H_

/* Header files*/
#include "io_handler.h"
#include "main.h"
#include "trafficstates.h"
#include <stdbool.h>

/* Enum for statemachine */
/*typedef enum{
	STATE_INIT,
	STATE_NS_GREEN,
	STATE_NS_YELLOW,
	STATE_NS_RED,
}Trafficlights;
*/


/* Functions in trafficssystem*/
void statemachine(void);

#endif /* INC_TRAFFICSYSTEM_H_ */
