/*
 * trafficstates.c
 *
 *  Created on: 28 Nov 2025
 *      Author: simonstal
 */
#include "trafficstates.h"
#include "test.h"


/**
 * @brief Flag to control if the light should switch direction.
 */
uint8_t changedirection = 0;


/**
 * @brief Bitmask for pedestrianlight signal.
 *
 * This variabel stores the bitcombination that are sent to the shiftregister to
 * control the pedestrian lights
 */
uint8_t pedestrianlight;


/**
 * @brief toggles status for the blue waiting light with toggleFreq ms.
 * * The function creates a blinking effect by comparing the time difference between blinkstart
 * and blinkstop. If the difference is greater than or equal to toggleFreq the status is inverted
 * for the blue light.
 *
 * @param blinkstart pointer
 * @param blinkstop
 * @return uint8_t Returns the current status for the blue lgiht.
 */

uint8_t toggle_bluelights(uint32_t *blinkstart, uint32_t blinkstop){

	static uint8_t blue_on = 0;

	if(blinkstop - *blinkstart >= toggleFreq){
		blue_on = !blue_on;
		*blinkstart = blinkstop;
	}

	return blue_on;

}

/**
 * @brief Keeps track of how long it has been green for the pedestrian
 *
 * @param greenstart pointer
 * @param greenstop
 * @return uint8_t Returns 1 as the pedestrian is allowed to walk. And 0 when time is up.
 */

uint8_t greenlights(uint32_t *greenstart, uint32_t greenstop){
	uint8_t green_on = 1;

	if(greenstop - *greenstart >= walkingDelay){
		green_on = 0;
	}
	return green_on;
}


/**
 * @brief Updates the lights based on the current state.
 * * The function first decides the status for the pedestrian light and sends
 * the correct bitcombination for both car and pedestrian signals to the shiftregisters.
 * @param states       current state for the trafficlights (t.ex. NS_GREEN, EW_RED)
 * @param blue_status  If the blue light should be on or off.
 * @param green_status If the green pedestrian light should be on or off.
 */

void update_lights_hardware(Trafficlights states, uint8_t blue_status, uint8_t green_status){
	if(blue_status && !ped_green){
		pedestrianlight = 0b00100000;
	}
	else if(ped_green){
		pedestrianlight = 0b00001000;
	}else{
		pedestrianlight = 0b0;
	}

	switch(states){

	case NS_GREEN:
		write_to_register(EAST_RED, SOUTH_GREEN + pedestrianlight , NORTH_GREEN);
		break;

	case NS_YELLOW:
		write_to_register(EAST_RED, SOUTH_YELLOW + pedestrianlight , NORTH_YELLOW);
		break;

	case NS_RED:
		write_to_register(EAST_RED, SOUTH_RED + pedestrianlight , NORTH_RED);
		break;


	case EW_GREEN:
		write_to_register(EAST_GREEN, SOUTH_RED + pedestrianlight , WEST_GREEN);
		break;

	case EW_YELLOW:
		write_to_register(EAST_YELLOW, SOUTH_RED + pedestrianlight , WEST_YELLOW);
			break;

	case EW_RED:
		write_to_register(EAST_RED, SOUTH_RED + pedestrianlight , WEST_RED);
			break;

	}
}


/**
 * @brief Main logic for changing between the trafficlights different states.
 * The function takes action based on different flags and passed time to decide
 * when to change to a different state. It handles priority rules and makes sure
 * that the timedelays are followed
 *
 * @param start_time   Pointer to the time when the current state started.
 * @param current_time The actual system time.
 * @param states       Pointer to current state
 * @param n_ped_wait   Pointer to flag that indicates if a pedestrian is waiting.
 * @param car_ns       Flag for cars in North-South direction (1 = car is present)
 * @param car_ew       Flag for cars in East-West direction (1 = car is present)
 */

void update_traffic_states(uint32_t *start_time, uint32_t current_time, Trafficlights *states, uint8_t *n_ped_wait, uint8_t car_ns, uint8_t car_ew){

    uint32_t passed_time = current_time - *start_time;

    switch(*states){

    case NS_GREEN:
    	changedirection = 0;

        if(*n_ped_wait == 0 && car_ns == 0 && car_ew == 1){
        	*states = EW_GREEN;
        	*start_time = current_time;
        }

        else if(*n_ped_wait == 1 && car_ns == 1 && car_ew == 1){
        	if(passed_time >= pedestrianDelay / 4){
        		*states = NS_YELLOW;
        		*start_time = current_time;
        	}
        }

        //Scenario 1
        else if(*n_ped_wait == 1 && car_ns == 0 && car_ew == 0){
        	if(passed_time >= pedestrianDelay / 4){
               *states = NS_YELLOW;
               *start_time = current_time;

               }
        	}

        //Scenario 2
        else if(car_ns == 1 && car_ew == 1 && *n_ped_wait == 0){
        	if(passed_time >= (redDelayMax / 4)){
        		  *states = NS_YELLOW;
        		   *start_time = current_time;
        	}
        }

        //Scenario 3
        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 0){
                *states = NS_GREEN;
                *start_time = current_time;
        }

        //Scenario 4
        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 1){
               if(passed_time >= pedestrianDelay / 4){
                	*states =  NS_YELLOW;
                	*start_time = current_time;
                	}
                }
        //Scenario 5
        else if(car_ns == 0 && car_ew == 1 && *n_ped_wait == 1){
        	if(passed_time >= pedestrianDelay / 4){
        	     *states =  NS_YELLOW;
        	     *start_time = current_time;
        	     }
        }
        //Scenario 6
        else if(passed_time >= greenDelay){
                *states = NS_YELLOW;
                *start_time = current_time;
                }

        break;


    case NS_YELLOW:

        if(*n_ped_wait == 0 && car_ns == 0 && car_ew == 1){
        	*states = EW_GREEN;
        	*start_time = current_time;
        }

        else if(*n_ped_wait == 1 && car_ns == 1 && car_ew == 1){

        	if(passed_time >= pedestrianDelay / 4){
        		if(changedirection) {
        			*states = NS_GREEN;
        			*start_time = current_time;
        		}else{
        			*states = NS_RED;
        			*start_time = current_time;
        		}
        	}
        	}

        //Scenario 1
        else if(*n_ped_wait == 1 && car_ns == 0 && car_ew == 0){
        	if(passed_time >= pedestrianDelay / 4){
        		if(changedirection){
        			*states = NS_GREEN;
        			*start_time = current_time;
        		}else{
        			*states = NS_RED;
        			*start_time = current_time;
        		}
               }
        	}

        //Scenario 2
        else if(car_ns == 1 && car_ew == 1 && *n_ped_wait == 0){
        	if(passed_time >= (redDelayMax / 6)){
        		if(changedirection){
        		  *states = NS_GREEN;
        		   *start_time = current_time;
        		}else{
        			*states = NS_RED;
        			*start_time = current_time;
        		}
        	}
        }
        //Scenario 3
        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 0){
                *states = NS_GREEN;
                *start_time = current_time;
        }

        //Scenario 4
        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 1){
               if(passed_time >= pedestrianDelay / 4){
            	   if(changedirection){
                	*states =  NS_GREEN;
                	*start_time = current_time;
            	   }else{
            		   *states =  NS_RED;
            		   *start_time = current_time;
            	   }
                	}
                }
        //Scenario 5
        else if(car_ns == 0 && car_ew == 1 && *n_ped_wait == 1){
        	if(passed_time >= pedestrianDelay / 4){
        		if(changedirection){
        	     *states =  NS_GREEN;
        	     *start_time = current_time;
        	     }else {
        	    	 *states =  NS_RED;
        	    	 *start_time = current_time;
        	     }
        }
        }
        //Scenario 6
        else if(passed_time >= orangeDelay){
        	if(changedirection){
        		*states = NS_GREEN;
        		*start_time = current_time;
        	}else{
        		*states = NS_RED;
        		*start_time = current_time;
        	}
        }

        break;

    case NS_RED:


        if(*n_ped_wait == 0 && car_ns == 0 && car_ew == 1){
        	*states = EW_GREEN;
        	*start_time = current_time;
        }
        else if(changedirection == 1 && ped_green == 1){
        	*states = NS_RED;
        	*start_time = current_time;
        }
        else if(*n_ped_wait == 1 && car_ns == 1 && car_ew == 1){

        	if(passed_time >= pedestrianDelay / 4){
        		if(changedirection) {
        			*states = NS_YELLOW;
        			*start_time = current_time;
        		}else{
        			*states = EW_RED;
        			*start_time = current_time;
        			ped_green = 1;
        			*n_ped_wait = 0;
        		}
        	}
        	}

        //Scenario 1
        else if(*n_ped_wait == 1 && car_ns == 0 && car_ew == 0){
        	if(passed_time >= pedestrianDelay / 4){
        		if(changedirection){
        			*states = NS_YELLOW;
        			*start_time = current_time;
        		}else{
        			*states = EW_RED;
        			*start_time = current_time;
        			ped_green = 1;
        			*n_ped_wait = 0;
        		}
               }
        	}

        //Scenario 2
        else if(car_ns == 1 && car_ew == 1 && *n_ped_wait == 0){
        	if(passed_time >= (redDelayMax / 12)){
        		if(changedirection){
        		  *states = NS_YELLOW;
        		   *start_time = current_time;
        		}else{
        			*states = EW_RED;
        			*start_time = current_time;
        		}
        	}
        }
        //Scenario 3
        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 0){
        	if(passed_time >= redDelay)
        		changedirection = 1; //??? TEST Kan vara rätt testa ordentligt senare, Vi fastnade i rött läge o ändligheten
                *states = NS_YELLOW;
                *start_time = current_time;
        }

        //Scenario 4
        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 1){
               if(passed_time >= pedestrianDelay / 4){
            	   if(changedirection){
                	*states =  NS_YELLOW;
                	*start_time = current_time;
            	   }else{
            		   *states =  EW_RED;
            		   *start_time = current_time;
            		   ped_green = 1;
            		   *n_ped_wait = 0;
            	   }
                	}
                }
        //Scenario 5
        else if(car_ns == 0 && car_ew == 1 && *n_ped_wait == 1){
        	if(passed_time >= pedestrianDelay / 4){
        	    	 *states =  EW_RED;
        	    	 *start_time = current_time;
        		}
        }
        //Scenario 6
        else if(passed_time >= redDelay){
        	if(changedirection){
        		*states = NS_YELLOW;
        		*start_time = current_time;
        	}else{
        		*states = EW_RED;
        		*start_time = current_time;
        	}
        }

        break;


    case EW_GREEN:
    	changedirection= 1;

    	        if(*n_ped_wait == 0 && car_ns == 0 && car_ew == 1){
    	        	*states = EW_GREEN;
    	        	*start_time = current_time;
    	        }

    	        else if(*n_ped_wait == 1 && car_ns == 1 && car_ew == 1){
    	        	ped_green = 1;
    	        	*n_ped_wait = 0;
    	        	if(passed_time >= greenDelay){
    	        		*states = EW_YELLOW;
    	        		*start_time = current_time;
    	        	}
    	        }

    	        //Scenario 1
    	        else if(*n_ped_wait == 1 && car_ns == 0 && car_ew == 0){
    	        	ped_green = 1;
    	            *n_ped_wait = 0;
    	        	if(passed_time >= pedestrianDelay / 4){
    	               *states = EW_YELLOW;
    	               *start_time = current_time;

    	               }
    	        	}

    	        //Scenario 2
    	        else if(car_ns == 1 && car_ew == 1 && *n_ped_wait == 0){
    	        	if(passed_time >= (redDelayMax / 4)){
    	        		  *states = EW_YELLOW;
    	        		   *start_time = current_time;
    	        	}
    	        }

    	        //Scenario 3
    	        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 0){
    	        	if(ped_green == 0){
    	                *states = NS_GREEN;
    	                *start_time = current_time;
    	        	}
    	        }

    	        //Scenario 4
    	        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 1){
    	        	ped_green = 1;
    	            *n_ped_wait = 0;
    	               if(passed_time >= pedestrianDelay / 4){
    	                	*states =  EW_YELLOW;
    	                	*start_time = current_time;
    	                	}
    	                }
    	        //Scenario 5
    	        else if(car_ns == 0 && car_ew == 1 && *n_ped_wait == 1){
    	        	ped_green = 1;
    	            *n_ped_wait = 0;
    	        	     *states =  EW_GREEN;
    	        	     *start_time = current_time;
    	        }
    	        //Scenario 6
    	        else if(passed_time >= greenDelay){
    	                *states = EW_YELLOW;
    	                *start_time = current_time;
    	                }

        break;

    case EW_YELLOW:


        if(*n_ped_wait == 0 && car_ns == 0 && car_ew == 1){
        	*states = EW_GREEN;
        	*start_time = current_time;
        }

        else if(*n_ped_wait == 1 && car_ns == 1 && car_ew == 1){

        	if(passed_time >= pedestrianDelay / 4){
        		if(changedirection) {
        			*states = EW_RED;
        			*start_time = current_time;
        		}else{
        			*states = EW_GREEN;
        			*start_time = current_time;
        		}
        	}
        	}

        //Scenario 1
        else if(*n_ped_wait == 1 && car_ns == 0 && car_ew == 0){
        	if(passed_time >= pedestrianDelay / 4){
        		if(changedirection){
        			*states = EW_RED;
        			*start_time = current_time;
        		}else{
        			*states = EW_GREEN;
        			*start_time = current_time;
        		}
               }
        	}

        //Scenario 2
        else if(car_ns == 1 && car_ew == 1 && *n_ped_wait == 0){
        	if(passed_time >= (redDelayMax / 6)){
        		if(changedirection){
        		  *states = EW_RED;
        		   *start_time = current_time;
        		}else{
        			*states = EW_GREEN;
        			*start_time = current_time;
        		}
        	}
        }
        //Scenario 3
        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 0){
        	if(ped_green == 0){
                *states = NS_GREEN;
                *start_time = current_time;
        	}
        }

        //Scenario 4
        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 1){
               if(passed_time >= pedestrianDelay / 4){
            	   if(changedirection){
                	*states =  EW_RED;
                	*start_time = current_time;
            	   }else{
            		   *states =  EW_GREEN;
            		   *start_time = current_time;
            	   }
                	}
                }
        //Scenario 5
        else if(car_ns == 0 && car_ew == 1 && *n_ped_wait == 1){
        	if(passed_time >= pedestrianDelay / 4){
        		if(changedirection){
        	     *states = EW_RED;
        	     *start_time = current_time;
        	     }else {
        	    	 *states =  EW_GREEN;
        	    	 *start_time = current_time;
        	     }
        }
        }
        //Scenario 6
        else if(passed_time >= orangeDelay){
        	if(changedirection){
        		*states = EW_RED;
        		*start_time = current_time;
        	}else{
        		*states = EW_GREEN;
        		*start_time = current_time;
        	}
        }
     break;

    case EW_RED:

    	if(*n_ped_wait == 0 && car_ns == 0 && car_ew == 1){
    	        	*states = EW_GREEN;
    	        	*start_time = current_time;
    	        }

    	        else if(*n_ped_wait == 1 && car_ns == 1 && car_ew == 1){

    	        	if(passed_time >= pedestrianDelay / 4){
    	        		if(changedirection) {
    	        			*states = NS_RED;
    	        			*start_time = current_time;
    	        		}else{
    	        			*states = EW_YELLOW;
    	        			*start_time = current_time;
    	        			ped_green = 1;
    	        			*n_ped_wait = 0;
    	        		}
    	        	}
    	        	}

    	        //Scenario 1
    	        else if(*n_ped_wait == 1 && car_ns == 0 && car_ew == 0){
    	        	if(passed_time >= pedestrianDelay / 4){
    	        		if(changedirection){
    	        			*states = NS_RED;
    	        			*start_time = current_time;
    	        			ped_green = 1;
    	        			*n_ped_wait = 0;
    	        		}else{
    	        			*states = EW_YELLOW;
    	        			*start_time = current_time;
    	        		}
    	               }
    	        	}

    	        //Scenario 2
    	        else if(car_ns == 1 && car_ew == 1 && *n_ped_wait == 0){
    	        	if(passed_time >= (redDelayMax / 12)){
    	        		if(changedirection){
    	        		  *states = NS_RED;
    	        		   *start_time = current_time;
    	        		}else{
    	        			*states = EW_YELLOW;
    	        			*start_time = current_time;
    	        		}
    	        	}
    	        }
    	        //Scenario 3
    	        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 0){
    	        	if(ped_green == 0){
    	                *states = NS_RED;
    	                *start_time = current_time;
    	        	}
    	        }

    	        //Scenario 4
    	        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 1){
    	               if(passed_time >= pedestrianDelay / 4){
    	            	   if(changedirection){
    	                	*states =  NS_RED;
    	                	*start_time = current_time;
    	                	   ped_green = 1;
    	                	   *n_ped_wait = 0;
    	            	   }else{
    	            		   *states =  EW_YELLOW;
    	            		   *start_time = current_time;
    	            		   ped_green = 1;
    	            		   *n_ped_wait = 0;
    	            	   }
    	                	}
    	                }
    	        //Scenario 5
    	        else if(car_ns == 0 && car_ew == 1 && *n_ped_wait == 1){
    	        	if(passed_time >= pedestrianDelay / 4){
    	        	    	 *states =  EW_YELLOW;
    	        	    	 *start_time = current_time;
    	        	    	 ped_green = 1;
    	        	    	*n_ped_wait = 0;
    	        		}
    	        }
    	        //Scenario 6
    	        else if(passed_time >= redDelay){
    	        	if(changedirection){
    	        		*states = NS_RED;
    	        		*start_time = current_time;
    	        	}else{
    	        		*states = EW_YELLOW;
    	        		*start_time = current_time;
    	        	}
    	        }

        break;
    }

}


