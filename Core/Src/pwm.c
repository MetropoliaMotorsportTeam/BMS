/*
 * pwm.c
 *
 *  Created on: May 16, 2023
 *      Author: Dovlat
 */
#include "conf.h"
#include <stdint.h>
#include "stm32g4xx.h"
#include "pwm.h"


/** PWM frequency in Hz */
#define PWM_FREQUENCY      25000
/** Period value of PWM output waveform */
#define PERIOD_VALUE       100
/** Initial duty cycle value */
#define INIT_DUTY_VALUE    30


/**Target temperature*/
#define TEMP_SETPOINT 40
//Proportional coefficient
#define Kp  2.5
//Integral coefficient
#define Ki  0.1
#define MAX_PWM 40
#define MIN_PWM 3
//Sample Time(s)
#define DT 0.1


extern TIM_HandleTypeDef htim8;



// fan_control() and set_fan_duty_cycle() can be deleted after fan_pi_controller() is tested
void fan_control(status_data_t *status_data){
	if(status_data->max_temp > 50)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, SET);
	else
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, RESET);
}

void set_fan_duty_cycle(status_data_t *status_data){

	if(status_data->max_temp > 39){
		 __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, MAX_PWM);
	}
	else{
		 __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, 8);
	}
}



static float integral = 0.0;

void fan_pi_controller(status_data_t *status_data) {
	float error = status_data->max_temp - TEMP_SETPOINT;  //temp error

	//error = error < 0 ? 0 : error;

	if (error < 0) {
	   error = 0;
	   integral *= 0.9;
	} else {
	   integral += error * DT;
	}

    // integral
	//integral += error * DT;

    // calculating PI
    float control_signal = ((Kp * error) + (Ki * integral));

    // Limit PWM within acceptable range
    if (control_signal > MAX_PWM) control_signal = MAX_PWM;
    if (control_signal < MIN_PWM) control_signal = MIN_PWM;

    // fan speed
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, (uint16_t)control_signal);
    uint8_t pwm_tx[8] = {status_data->max_temp, (uint8_t)integral, (uint8_t)error, (uint8_t)control_signal, 0, 0, 0, 0};
    //uint8_t pwm_tx[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    CanSend(pwm_tx, CAN_PWM_DATA);
}
