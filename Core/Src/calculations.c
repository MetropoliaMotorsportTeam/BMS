/*
 * calculations.c
 *
 *  Created on: May 2, 2023
 *      Author: dovlati
 */
#include "conf.h"
#include "calculations.h"
/*!
	\brief	Calculates the Sum of Cells.
*/


void calc_sum_of_cells(uint8_t total_ic, cell_data_t cell_data[][CELL_NUM], status_data_t *status_data)
{

	uint16_t soc = 0;

	for(uint8_t i = 0; i < total_ic; i++){
		for(uint8_t j = 0; j < CELL_NUM; j++){
			soc += (cell_data[i][j].voltage / 100);
		}
	}
	status_data->sum_of_cells = (float)soc /100;
}
/*!
	\brief	Calculate power from current data and Sum of Cells.
*/
float roundToTwoDecimal(float num) {
    return round(num * 100) / 100.0;
}
float round_to_nearest_five_cents(float num) {
    return round(num * 20) / 20.0;
}

float OCV[][2] = {
        {3.00, 0},
        {3.05, 5},
        {3.10, 10},
        {3.15, 15},
        {3.20, 20},
        {3.30, 25},
        {3.40, 30},
        {3.45, 35},
        {3.50, 40},
        {3.45, 45},
        {3.60, 50},
        {3.65, 55},
        {3.70, 60},
        {3.75, 65},
        {3.80, 70},
        {3.90, 75},
        {4.00, 80},
        {4.05, 85},
        {4.10, 90},
        {4.15, 95},
        {4.20, 100}
    };

void OCV_soc(status_data_t *status_data) {




    float voltage = roundToTwoDecimal(status_data->IVT_U3_f);
    float rounded = round_to_nearest_five_cents(voltage);
    int OCV_row = (rounded - 3)/0.05;
    status_data->soc = OCV[OCV_row][1];

}


void coulumb_soc(status_data_t *status_data){

	//float consumed = status_data->IVT_I * FREQUENCY/3600;

	//status_data->soc = 100 - (status_data->IVT_Wh / ACCU_Wh)*100;

	float used_Ah = status_data->IVT_As_f/3600;
	status_data->soc = status_data->soc + (used_Ah/13227.513)*100;
	//status_data->soc_init = status_data->soc;
	//8kWh 604.8V, 13227.513mAh
}


void calculate_soc(status_data_t *status_data){

	if (status_data->pre_s == true) {

		coulumb_soc(status_data);

	} else {

		OCV_soc(status_data);

	}

}
/*!
	Loops though all temp values and finds the highest and lowest temp values and corresponding
	cell IDs

	\param total_ic				Number of ltc6804 ICs in stack
	\param cell_data[][12]		Cell data array
	\param *status_data			Pointer to status_data struct
*/
void get_minmax_temperature(uint8_t total_ic, temp_data_t temp_data[][GPIO_NUM], status_data_t *status_data)
{
	int32_t min = temp_data[0][0].temp;
	int32_t max = temp_data[0][0].temp;
	uint8_t min_id = 0;
	uint8_t max_id = 0;

	for (uint8_t i = 0; i < total_ic; i++)
	{
		for (uint8_t j = 0; j < 5; j++)
		{

			if (temp_data[i][j].temp > max)
			{

				max = temp_data[i][j].temp;
				max_id = (i << 4) | (j & 0x0f);

			}
			if (temp_data[i][j].temp < min && j != 5)
			{

				min = temp_data[i][j].temp;
				min_id = (i << 4) | (j & 0x0f);

			}
		}
	}
	status_data->min_temp = min;
	status_data->max_temp = max;

	status_data->min_temp_id = min_id;
	status_data->max_temp_id = max_id;
}

/*!
	Loops though all cell values and finds the highest and lowest values and corresponding
	cell IDs

	\param total_ic				Number of ltc6804 ICs in stack
	\param cell_data[][CELL_NUM]		Cell data array
	\param *status_data			Pointer to status_data struct
*/
void get_minmax_voltage(uint8_t total_ic, cell_data_t cell_data[][CELL_NUM], status_data_t *status_data)
{
	uint16_t min = cell_data[0][0].voltage;
	uint16_t max = cell_data[0][0].voltage;
	uint8_t min_id = 0;
	uint8_t max_id = 0;





	for (uint8_t i = 0; i < total_ic; i++)
	{
		for (uint8_t j = 0; j < CELL_NUM; j++)
		{
			if (cell_data[i][j].voltage > max)
			{
				max = cell_data[i][j].voltage;
				max_id = (i << 4) | (j & 0x0f);
			}
			if (cell_data[i][j].voltage < min && cell_data[i][j].voltage > 5000) //ignore cells under .5V
			{
				min = cell_data[i][j].voltage;
				min_id = (i << 4) | (j & 0x0f);
			}
		}
	}
	status_data->min_voltage = min;
	status_data->max_voltage = max;
	status_data->delta = max - min;
	status_data->min_voltage_id = min_id;
	status_data->max_voltage_id = max_id;
}


/*!
	Parses data in cell_data array

	\param total_ic	Number of measurement ICs connected

	\return void
*/




void build_disch_cfg(uint8_t total_ic, cell_data_t cell_data[][CELL_NUM], uint8_t tx_config[][6],\
					 status_data_t *status_data, limit_t *limit){
	uint16_t DCCx = 0x0000;
	//discharge all cells

	for (uint8_t i = 0; i < total_ic; i++){
		for (uint8_t j = 0; j < 12; j++){
			if (cell_data[i][j].voltage > (status_data->min_voltage + limit->tolerance)){
				DCCx |= (1<<j);
			}
			else{
				DCCx &= ~(1<<j);
			}
		}
		tx_config[i][4] = (DCCx & 0x00ff);
		tx_config[i][5] = ((DCCx >> 8) & 0x0f);
	}

}

void build_disch_cfgb(uint8_t total_ic, cell_data_t cell_data[][CELL_NUM], uint8_t tx_config[][6],\
status_data_t *status_data, limit_t *limit)
{
	uint16_t DCCx = 0x0000;
		//discharge all cells

	for (uint8_t i = 0; i < total_ic; i++){
			for (uint8_t j = 12; j < CELL_NUM; j++){
				if (cell_data[i][j].voltage > (status_data->min_voltage + limit->tolerance)){
					DCCx |= (1<<(j - 12));
				}
				else{
					DCCx &= ~(1<<(j - 12));
				}
			}
			tx_config[i][0] = (DCCx & 0x0f) << 4 ;
			tx_config[i][1] = (DCCx >> 4);
		}
}
