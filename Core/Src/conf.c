#include "conf.h"
#include "can.h"

status_data_t status_data = {0};
#define DEFAULT_CONF 1

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
  uint8_t data[8] = {0};
  CanSend(data, 0x1);

  status_data.mode = 0;
  limits = (limit_t){.max_voltage = 30000,
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
}

void Config_2()
{
  uint8_t data[8] = {0};
  CanSend(data, 0x2);
  status_data.mode = 1;
  limits = (limit_t){.max_voltage = 42000,
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
}

void Config_3()
{
  uint8_t data[8] = {0};
  CanSend(data, 0x3);
  status_data.mode = 1;
  limits = (limit_t){.max_voltage = 42000,
                     .min_voltage = 25000,
                     .max_charge_temp = 4400,
                     .max_temp = 69,
                     .min_temp = 0,
                     .power = 1e6,
                     .tolerance = 0, // Sets max voltage difference
                     .max_current = 180.0,
                     .accu_min_voltage = 450.0,
                     .precharge_min_start_voltage = 450.0,
                     .precharge_max_end_voltage = 450.0,
                     .limp_min_voltage = 34000};
}

void Config_4()
{
  uint8_t data[8] = {0};
  CanSend(data, 0x4);
  status_data.mode = 1;
  limits = (limit_t){.max_voltage = 42000,
                     .min_voltage = 25000,
                     .max_charge_temp = 4400,
                     .max_temp = 67,
                     .min_temp = 0,
                     .power = 2 * (1e6),
                     .tolerance = 0, // Sets max voltage difference
                     .max_current = 180.0,
                     .accu_min_voltage = 450.0,
                     .precharge_min_start_voltage = 450.0,
                     .precharge_max_end_voltage = 450.0,
                     .limp_min_voltage = 34000};
}

void Change_Config(uint8_t CAN_RxData[])
{

  uint8_t config = DEFAULT_CONF;
  if (CAN_RxData)
    config = CAN_RxData[0];

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
  uint8_t data[8] = {100};
  CanSend(data, 0x12);
}
