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
uint8_t greenlights(uint32_t *greenstart, uint32_t greenstop){
	uint8_t green_on = 1;
	//green_on = 1;

	if(greenstop - *greenstart >= walkingDelay){
		green_on = 0;
		//*greenstart = greenstop;
	}

	return green_on;
}

void update_lights_hardware(Trafficlights states, uint8_t blue_status, uint8_t green_status){
	if(blue_status){
		testbits = 0b00100000;
	}
	else if(ped_green){
		testbits = 0b00001000;
	}else{
		testbits = 0b0;
	}

	//else{
	//	testbits = 0b0;
	//}
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

	//case NS_PED_GREEN:
		//write_to_register(ZERO,PED_GREEN+ testbits, NORTH_RED);
		//break;

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

/*
 * k bestämmer vilket trafikljus vi går till om ingen input finns.
 */


void update_traffic_states(uint32_t *start_time, uint32_t current_time, Trafficlights *states, uint8_t *n_ped_wait, uint8_t car_ns, uint8_t car_ew){

    uint32_t passed_time = current_time - *start_time;

    switch(*states){

    case NS_GREEN:
        k = 0;

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

// OM ALLA ÄR 1 OCKSÅ
        break;

    case NS_YELLOW:

        if(*n_ped_wait == 0 && car_ns == 0 && car_ew == 1){
        	*states = EW_GREEN;
        	*start_time = current_time;
        }

        else if(*n_ped_wait == 1 && car_ns == 1 && car_ew == 1){

        	if(passed_time >= pedestrianDelay / 4){
        		if(k) {
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
        		if(k){
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
        	if(passed_time >= (redDelayMax / 4)){
        		if(k){
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
            	   if(k){
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
        		if(k){
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
        	if(k){
        		*states = NS_GREEN;
        		*start_time = current_time;
        	}else{
        		*states = NS_RED;
        		*start_time = current_time;
        	}
        }


    	/*if(*n_ped_wait == 1 && car_ns == 1) {//ÄNDRA DELAY


    		if(passed_time >= redDelayMax / 6) {
    			if(k){
    			*states = NS_GREEN;
    			*start_time = current_time;
    			}else{
    				*states = NS_GREEN;
    				*start_time = current_time;
    			}
    			}

        	}

    	else if(*n_ped_wait == 1 && k == 1) {

    	     if(passed_time >= orangeDelay) {//ÄNDRA DELAY
    	    	 *states = NS_GREEN;
    	    	 *start_time = current_time;
    	    	}
    	     }

        else if(*n_ped_wait == 1){

    		 if(passed_time >= pedestrianDelay){
    			 *states =  NS_RED;
    			 *start_time = current_time;
    		 	 }
        }

    	else if(passed_time >= orangeDelay){
    			if(k){
    				*states = NS_GREEN;
    				*start_time = current_time;

    			}else{
    				*states = NS_RED;
    				*start_time = current_time;
    			}

    	}


*/
        break;

    case NS_RED:


        if(*n_ped_wait == 0 && car_ns == 0 && car_ew == 1){
        	*states = EW_GREEN;
        	*start_time = current_time;
        }

        else if(*n_ped_wait == 1 && car_ns == 1 && car_ew == 1){

        	if(passed_time >= pedestrianDelay / 4){
        		if(k) {
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
        		if(k){
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
        	if(passed_time >= (redDelayMax / 4)){
        		if(k){
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
                *states = NS_YELLOW;
                *start_time = current_time;
        }

        //Scenario 4
        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 1){
               if(passed_time >= pedestrianDelay / 4){
            	   if(k){
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
        	if(k){
        		*states = NS_YELLOW;
        		*start_time = current_time;
        	}else{
        		*states = EW_RED;
        		*start_time = current_time;
        	}
        }
    	/*
    	if(car_ns == 1 && car_ew == 1){
    		if(passed_time >= redDelayMax / 12){
    			if(k){
    				*states = NS_YELLOW;
    				*start_time = current_time;
    			}else{
    				*states = EW_RED;
    				*start_time = current_time;
    			}
    		}
    	}
    	else if(ped_green == 1 && k == 1){
    		*states = NS_RED;
    		*start_time = current_time;
    	}
    	else if(*n_ped_wait == 1 && k == 1){
    	    		ped_green = 0;
    	    	if(passed_time >= pedestrianDelay){//ÄNDRA DELAY
    	    		*states = NS_YELLOW;
    	    		*start_time = current_time;
    	    	}
    	}else if(*n_ped_wait == 1 && k == 0){
    		if(passed_time >= pedestrianDelay){
    			 ped_green = 0;
    			*states = EW_RED ;
    			*start_time = current_time;
    		}

    	}
    	else if(passed_time >= redDelay){
    		if(k){
    			*states = NS_YELLOW;
    			*start_time = current_time;
    		}else{
    			*states = EW_RED;
    			*start_time = current_time;
    		}
    	}
*/
        break;


    case EW_GREEN:
    	 k = 1;

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

       /* k = 1;

        if(car_ns == 0 && car_ew == 0 && *n_ped_wait == 1){
        	*n_ped_wait = 0;
        	ped_green = 1;
        	if(passed_time >= greenDelay){
        		*states = EW_YELLOW;
        		*start_time =  current_time;
        	}
        }

        else if(car_ns == 0 && car_ew == 1 && *n_ped_wait == 0){
        	*states = EW_GREEN;
        	//*start_time = current_time;
        }
        else if(car_ns == 1 && car_ew == 0 && *n_ped_wait == 0){
        	if(passed_time >= greenDelay){
        		*states = EW_YELLOW;
        		*start_time = current_time;
        	}
        }
        else if(car_ns == 0 && car_ew == 1 && *n_ped_wait == 1){
        	*states = EW_GREEN;
        	*start_time = current_time;
        	*n_ped_wait = 0;
        	ped_green = 1;
        }
        else if(car_ns == 1 && car_ew == 1 && *n_ped_wait == 1){
        	*n_ped_wait = 0;
        	ped_green = 1;
        	if(passed_time >= pedestrianDelay){//DELA DEN MED NÅGOT
        		*states = EW_YELLOW;
        		*start_time = current_time;
        	}
        }else if(car_ns == 1 && car_ew == 1 && *n_ped_wait == 0){//NY
        	*n_ped_wait = 0;
        	ped_green = 1;
        	if(passed_time >= pedestrianDelay){//DELA DEN MED NÅGOT
        		*states = EW_YELLOW;
        		*start_time = current_time;
        	}
        }
        else if(passed_time >= greenDelay){
    	        	*states = EW_YELLOW;
    	        	*start_time = current_time;
    	        }
*/
        break;

    case EW_YELLOW:


        if(*n_ped_wait == 0 && car_ns == 0 && car_ew == 1){
        	*states = EW_GREEN;
        	*start_time = current_time;
        }

        else if(*n_ped_wait == 1 && car_ns == 1 && car_ew == 1){

        	if(passed_time >= pedestrianDelay / 4){
        		if(k) {
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
        		if(k){
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
        	if(passed_time >= (redDelayMax / 4)){
        		if(k){
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
            	   if(k){
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
        		if(k){
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
        	if(k){
        		*states = EW_RED;
        		*start_time = current_time;
        	}else{
        		*states = EW_GREEN;
        		*start_time = current_time;
        	}
        }
/*
    	if(*n_ped_wait == 1){
    			*n_ped_wait = 0;
    			ped_green = 1;
    	  }
    	  if(passed_time >= orangeDelay){
    	    	if(k){
    	    		*states = EW_RED;
    	    		*start_time = current_time;
    	    	}else{
    	    		*states = EW_GREEN;
    	    		*start_time = current_time;
    	    			}
    	   }
*/
            break;

    case EW_RED:

    	if(*n_ped_wait == 0 && car_ns == 0 && car_ew == 1){//KANSKE ÄNDRA?
    	        	*states = EW_GREEN;
    	        	*start_time = current_time;
    	        }

    	        else if(*n_ped_wait == 1 && car_ns == 1 && car_ew == 1){

    	        	if(passed_time >= pedestrianDelay / 4){
    	        		if(k) {
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
    	        		if(k){
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
    	        	if(passed_time >= (redDelayMax / 4)){
    	        		if(k){
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
    	            	   if(k){
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
    	        	if(k){
    	        		*states = NS_RED;
    	        		*start_time = current_time;
    	        	}else{
    	        		*states = EW_YELLOW;
    	        		*start_time = current_time;
    	        	}
    	        }
    	/*if(*n_ped_wait == 1 && k == 0){
    		*n_ped_wait = 0;
    		ped_green = 1;
    	}else if(*n_ped_wait == 1){
    		if(passed_time >=redDelay){
    			*states = NS_RED;
    			*start_time = current_time;
    		}
    	}
    	if(passed_time >= redDelay){
    		if(k){
    		*states = NS_RED;
    		*start_time = current_time;
    		}else{
    			*states = EW_YELLOW;
    			*start_time = current_time;
    		}
    	}*/
        break;
    }

} // funktion slut


