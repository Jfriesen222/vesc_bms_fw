/*
	Copyright 2019 - 2020 Benjamin Vedder	benjamin@vedder.se

	This file is part of the VESC BMS firmware.

	The VESC BMS firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The VESC BMS firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */
#include "comm_can.h"
static THD_WORKING_AREA(hw_stormcore_thread_wa, 512);
static THD_FUNCTION(hw_stormcore_thread, arg);


void hw_init(void){
	palSetLineMode(LINE_BUCK_EN, PAL_MODE_OUTPUT_PUSHPULL);
	palSetLineMode(LINE_BRAKE_LIGHT_EN, PAL_MODE_OUTPUT_PUSHPULL);



	chThdCreateStatic(hw_stormcore_thread_wa, sizeof(hw_stormcore_thread_wa), NORMALPRIO, hw_stormcore_thread, NULL);
}

static THD_FUNCTION(hw_stormcore_thread, p) {
	(void)p;
	chRegSetThreadName("HW_Stormcore");
	while (!chThdShouldTerminateX()) {
		can_status_msg *msg = comm_can_get_status_msg_index(0);
		if (msg->id >= 0 && UTILS_AGE_S(msg->rx_time) < 1.0) {
			BUCK_12V_ON();
			if(msg->current < -1.0){
				BRAKE_LIGHT_ON();
			}else{
				BRAKE_LIGHT_OFF();
			}
			chThdSleepMilliseconds(50);
		}else{
			BUCK_12V_OFF();
			chThdSleepMilliseconds(1000);
		}
	}
}
