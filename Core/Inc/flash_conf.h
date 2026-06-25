#ifndef FLASH_CONFIG_H
#define FLASH_CONFIG_H
/*
  - Flash write file to store which configuration should be used
  - Uses the last Page of FLash memory to store
  - NOTE: Do not overwrite the last page

*/
#include "conf.h"
#include "stm32g4xx_hal.h"
#include <stdint.h>

#define CONFIG_FLASH_ADDR 0x0801F800U
#define CONFIG_FLASH_PAGE 127U
#define CONFIG_SLOT_SIZE 8
#define CONFIG_NUM_SLOTS (FLASH_PAGE_SIZE) / (CONFIG_SLOT_SIZE)
#define FLASH_EMPTY_U64 0xFFFFFFFFFFFFFFFFULL

static inline uint64_t flash_read_u64(uint32_t addr)
{
  return *(volatile uint64_t*)addr;
}

static inline uint8_t valid_config(uint8_t config)
{
  return config >= 1 && config <= NUM_CONF;
}

// converts 8bit into 64bit with empty flash
static inline uint64_t make_config_record(uint8_t config)
{
  return 0xFFFFFFFFFFFFFF00ULL | (uint64_t)config;
}

// converts 64bit into 8bit config
static inline uint8_t config_from_record(uint64_t record)
{
  return (uint8_t)(record & 0xFFU);
}

HAL_StatusTypeDef save_config(uint8_t config);
volatile uint8_t get_curr_conf();

#endif
