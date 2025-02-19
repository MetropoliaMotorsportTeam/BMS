/*
 * calculations.c
 *
 *  Created on: May 2, 2023
 *      Author: dovlati
 */
#include "conf.h"
#include "calculations.h"
#include <math.h>
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

/*float roundToTwoDecimal(float num) {
    return round(num * 100) / 100.0;
}
float round_to_nearest_five_cents(float num) {
    return round(num * 20) / 20.0;
}*/

float OCV[][2] = {
        {413, 0},//413,03592
        {417, 5},//417,6472
        {421, 10},//421,627
        {424, 15},//424,2333
        {427, 20},//427,0077
        {430, 25},//430,5987
        {434, 30},//434,7212
        {438, 35},//438,8016
        {442, 40},//442,88
        {446, 45},//446,9306
        {450, 50},//450,2152
        {454, 55},//454,266
        {458, 60},//458,2858
        {462, 65},//462,3792
        {466, 70},//466,8557
        {472, 75},//472,9163
        {482, 80},//482,0984
        {491, 85},//491,1594
        {499, 90},//499,7353
        {508, 95},//508,1635
        {597, 100}//597,6
    };

void OCV_soc(status_data_t *status_data) {


	int size = sizeof(OCV) / sizeof(OCV[0]);  // Dynamic array size

	    if (status_data->IVT_U3_f < OCV[0][0]) {
	        status_data->soc = OCV[0][1];  // 0%
	    } else if (status_data->IVT_U3_f > OCV[size - 1][0]) {
	        status_data->soc = OCV[size - 1][1];  // 100%
	    } else {
	        for (int i = 0; i < size - 1; i++) {
	            if (status_data->IVT_U3_f >= OCV[i][0] && status_data->IVT_U3_f < OCV[i + 1][0]) {
	                float V1 = OCV[i][0], SOC1 = OCV[i][1];
	                float V2 = OCV[i + 1][0], SOC2 = OCV[i + 1][1];

	                // Linear Interpolation
	                status_data->soc = SOC1 + (status_data->IVT_U3_f - V1) * (SOC2 - SOC1) / (V2 - V1);
	                status_data->soc = round(status_data->soc);
	                break;
	            }
	        }
	    }



    /*float voltage = roundToTwoDecimal(status_data->IVT_U3_f);
    float rounded = round_to_nearest_five_cents(voltage);
    int OCV_row = (rounded - 3)/0.05;
    status_data->soc = OCV[OCV_row][1];*/

}


void coulomb_soc(status_data_t *status_data){

	//float consumed = status_data->IVT_I * FREQUENCY/3600;

	//status_data->soc = 100 - (status_data->IVT_Wh / ACCU_Wh)*100;

	float used_Ah = status_data->IVT_As_f/3600;
	status_data->soc = status_data->soc + (used_Ah/13227.513)*100;
	//status_data->soc_init = status_data->soc;
	//8kWh 604.8V, 13227.513mAh
}


void calculate_soc(status_data_t *status_data){

	if (status_data->pre_s == true) {

		coulomb_soc(status_data);

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
