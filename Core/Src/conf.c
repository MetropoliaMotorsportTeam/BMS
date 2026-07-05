#include "conf.h"
#include "can.h"
#include "flash_conf.h"
#include "main.h"

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

void config_1()
{
  status_data.mode = 0;
  limits.tolerance = 0;
}

void config_2()
{
  status_data.mode = 0;
  limits.tolerance = 1000;
}

void config_3()
{
  status_data.mode = 1;
  limits.tolerance = 10;
}

void config_4()
{
  status_data.mode = 1;
  limits.tolerance = 0;
}

void process_config(uint8_t config)
{
  uint8_t conf = (valid_config(config) ? config : DEFAULT_CONF);
  if (save_config(conf) != HAL_OK)
    Error_Handler();

  apply_config(conf);
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
  uint8_t conf = (valid_config(config) ? config : DEFAULT_CONF);
  apply_config(conf);
}
