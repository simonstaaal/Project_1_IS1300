/*
 * trafficstates.c
 *
 *  Created on: 28 Nov 2025
 *      Author: simonstal
 */
#include "trafficstates.h"
#include "test.h"

uint8_t k = 0;
uint8_t testbits;
uint8_t toggle_bluelights(uint32_t *blinkstart, uint32_t blinkstop){

	static uint8_t blue_on = 0;

	if(blinkstop - *blinkstart >= toggleFreq){
		blue_on = !blue_on;
		*blinkstart = blinkstop;
	}

	return blue_on;

}

void update_lights_hardware(Trafficlights states, uint8_t blue_status){
	if(blue_status){
		testbits = 0b00100000;
	}else{
		testbits = 0b0;
	}
	switch(states){

	case NS_GREEN:
		write_to_register(EAST_RED, INIT_SOUTH_GREEN + testbits, INIT_NORTH_GREEN); //r1 r2 r3
		break;

	case NS_YELLOW:
		write_to_register(EAST_RED, SOUTH_YELLOW + testbits, NORTH_YELLOW);
		break;

	case NS_RED:
		write_to_register(EAST_RED, SOUTH_RED + testbits, NORTH_RED);
		break;

	case NS_PED_GREEN:
		write_to_register(ZERO,PED_GREEN+ testbits, NORTH_RED);
		break;

	case EW_GREEN:
		write_to_register(EAST_GREEN, SOUTH_RED + testbits, WEST_GREEN);
		break;

	case EW_YELLOW:
		write_to_register(EAST_YELLOW, SOUTH_RED + testbits, WEST_YELLOW);
			break;

	case EW_RED:
		write_to_register(EAST_RED, SOUTH_RED + testbits, WEST_RED);
			break;

	}


}


void update_traffic_states(uint32_t *start_time, uint32_t current_time, Trafficlights *states, uint8_t *n_ped_wait, uint8_t car_ns, uint8_t car_ew){

    uint32_t passed_time = current_time - *start_time;

    switch(*states){

    case NS_GREEN:
        k = 0;

        if(*n_ped_wait == 1){
        	if(passed_time >= pedestrianDelay){
               *states = NS_YELLOW;
               *start_time = current_time;
               }
        	}

        if(car_ns == 1 && car_ew == 1){
        	if(passed_time >= (redDelayMax / 4)){
        		  *states = NS_YELLOW;
        		   *start_time = current_time;
        	}
        }
        // 2. Normal tid har gått ut
        else if(passed_time >= greenDelay){

            // Prioritering: Fotgängare -> Förlängning -> Byt till gult
            //if(*n_ped_wait == 1){
               // *states = NS_YELLOW;
               // *start_time = current_time;
                //k = 0;

             if(car_ns == 1 && car_ew == 0){
                // Förläng grönt
                *states = NS_GREEN;
                *start_time = current_time; /// fel? tror jag //TEST
            }
            else {

                if(passed_time >= greenDelay){
                	*states = NS_YELLOW;
                	*start_time = current_time;
                	k = 0;
                }
            }
        }
        break;

    case NS_YELLOW:
        if(*n_ped_wait == 1){
        	if(passed_time >= pedestrianDelay){
              *states = NS_RED;
              *start_time = current_time;
        	}
        	}
    	 if(car_ns == 1 && car_ew == 1){
    		 if(passed_time >= (redDelayMax / 6)){
    			 if(k == 1){
    				 *states = NS_GREEN;
    				 *start_time = current_time;

    			 }else{
    				 	 *states = NS_RED;
    				 	 *start_time = current_time;
    			 	 }
    		 }
    	 }

    	 else if(passed_time >= orangeDelay){
    		*states = NS_RED;
    		*start_time = current_time;

    		if(k == 0){
    		*states = NS_RED;
    		*start_time = current_time;

    		}
    		else {
    		*states = NS_GREEN;
    		*start_time = current_time;
    		}
    	}
        break;

    case NS_RED:
        if(*n_ped_wait == 1){
              *states = NS_PED_GREEN;
              *start_time = current_time;
        	}
        if(car_ew == 1 && car_ns == 1){
        	if(passed_time >= (redDelayMax/ 12)){
        		if(k == 1){
        			*states = NS_YELLOW;
        			*start_time = current_time;
        		}else {
        			*states = EW_RED;
        			*start_time = current_time;
        		}
        	}
        }
        else if(passed_time >= redDelay){
    		*states = EW_RED;
    		*start_time = current_time;

    	if(k == 0){
    		*states = EW_RED;
    		*start_time = current_time;
    	}else {
    		*states = NS_YELLOW;
    		*start_time = current_time;
    	}
    	}
        break;

    case NS_PED_GREEN:
        *n_ped_wait = 0;
        if(passed_time >= walkingDelay){
            *states = EW_GREEN;
            *start_time = current_time;
        }
        break;

    case EW_GREEN:
        k = 1;
        if(*n_ped_wait == 1){
              *states = EW_GREEN;

        	}
        // 1. Rusningstrafik check
        else if(car_ew == 1 && car_ns == 1){
        	if(passed_time >= (redDelayMax /4)){
                *states = EW_YELLOW;
                *start_time = current_time;
        }
        }
        // 2. Normal tid har gått ut
        else if(passed_time >= greenDelay){

            if(car_ew == 1 && car_ns == 0){
                // Förläng grönt
                *states = EW_GREEN;
                *start_time = current_time;
            }
            else {
                *states = EW_YELLOW;
                *start_time = current_time;
            }
        }
        break;

    case EW_YELLOW:
        if(car_ew == 1 && car_ns == 1){
        	if(passed_time >= (redDelayMax / 6)){


        	if(k == 1){
                *states = EW_RED;
                *start_time = current_time;
        	}else{
        		*states = EW_GREEN;
                *start_time = current_time;
        	}
        }
        }
        else if(passed_time >= orangeDelay){
        		*states = EW_GREEN;
        		*start_time = current_time;

        	 if(k == 1){
        		*states = EW_RED;
        		*start_time = current_time;

        	}else {
        		*states = EW_GREEN;
        		*start_time = current_time;
        	}
       	}
            break;

    case EW_RED:

        if(car_ew == 1 && car_ns == 1){
        	if(passed_time >= (redDelayMax/12)){
        	if(k == 1){
        	*states = NS_RED;
        	*start_time = current_time;
        	}else {
        		*states = EW_YELLOW;
        		*start_time = current_time;
        	}
        }
        }

        else if(passed_time >= redDelay){
        		*states = NS_RED;
        		*start_time = current_time;

        	if(k == 1){
        		*states = NS_RED;
        		*start_time = current_time;
        	}else {
        		*states = EW_YELLOW;
        		*start_time = current_time;
        	}
    	}
        break;
    }

} // funktion slut


