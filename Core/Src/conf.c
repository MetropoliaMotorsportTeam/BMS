#include "conf.h"

// TODO: change these in the CAN bus configuration
status_data_t status_data = {0};

limit_t limits = {.max_voltage = 42000,
                  .min_voltage = 25000,
                  .max_charge_temp = 4400,
                  .max_temp = 59,
                  .min_temp = 0,
                  .power = (8 * (1e6)),
                  .tolerance = 0, // Sets max voltage difference
                  .max_current = 180.0,
                  .accu_min_voltage = 450.0,
                  .precharge_min_start_voltage = 450.0,
                  .precharge_max_end_voltage = 450.0,
                  .limp_min_voltage = 34000};

void Config_1()
{
}

void Config_2()
{
}

void Config_3()
{
}

void Config_4()
{
}

void Change_Config(uint8_t CAN_RxData[])
{
  uint8_t config = CAN_RxData[0];
  switch (config)
  {
  case 1:
    Config_1();
    break;
  case 2:
    Config_2();
    break;
  case 3:
    Config_3();
    break;
  case 4:
    Config_4();
    break;

  default:
    break;
  }
}
