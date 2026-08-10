
#ifndef PARSE_TEMP_H_
#define PARSE_TEMP_H_


#include "stdlib.h"
#include "LTC681x.h"

int temp_ambient_calc(uint32_t voltage);
void temp_calc(uint8_t total_ic,  temp_data_t temp_data[][GPIO_NUM]);

#endif /* PARSE_TEMP_H_ */
