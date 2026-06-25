#include "conf.h"
#include "can.h"
#include "flash_conf.h"
#include "main.h"

status_data_t status_data = {0};
limit_t limits = {0};

void config_1()
{
  status_data.mode = 0;
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

void config_2()
{
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

void config_3()
{
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

void config_4()
{
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

void process_config(uint8_t config)
{
  uint8_t conf = ((config < 1 || config > NUM_CONF) ? DEFAULT_CONF : config);
  if (save_config(conf) != HAL_OK)
    Error_Handler();

  apply_config(conf);

  // NOTE: remove after testing
  uint8_t data[8] = {0};
  CanSend(data, conf);
  CanSend(data, 0x12);
}

void apply_config(uint8_t config)
{
  switch (config)
  {
  case 1:
    config_1();
    break;
  case 2:
    config_2();
    break;
  case 3:
    config_3();
    break;
  case 4:
    config_4();
    break;
  default:
    break;
  }
}

void load_config()
{
  uint8_t config = get_curr_conf();
  uint8_t conf = ((config < 1 || config > NUM_CONF) ? DEFAULT_CONF : config);
  apply_config(conf);
}
