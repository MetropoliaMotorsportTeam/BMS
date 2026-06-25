#include "flash_conf.h"
#include "conf.h"
#include "main.h"
#include "stm32g4xx_it.h"
#include <stdint.h>

static HAL_StatusTypeDef flash_store(uint32_t addr, uint8_t config);
static HAL_StatusTypeDef flash_erase_page(uint32_t mem_addr);
static volatile uint32_t get_empty_conf_addr();

static HAL_StatusTypeDef flash_erase_page(uint32_t mem_addr)
{
  if ((mem_addr % 8U) != 0U)
  {
    return HAL_ERROR;
  }
  FLASH_EraseInitTypeDef flash_erase = {0};
  uint32_t page_error;

  __disable_irq();
  HAL_FLASH_Unlock();

  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  flash_erase.TypeErase = FLASH_TYPEERASE_PAGES;
  flash_erase.Banks = FLASH_BANK_1;
  flash_erase.Page = (mem_addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  flash_erase.NbPages = 1;

  HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&flash_erase, &page_error);

  __enable_irq();
  HAL_FLASH_Lock();
  return status;
}

static HAL_StatusTypeDef flash_store(uint32_t addr, uint8_t config)
{
  if (!valid_config(config))
  {
    return HAL_ERROR;
  }

  if ((addr % 8U) != 0U)
  {
    return HAL_ERROR;
  }

  __disable_irq();
  HAL_FLASH_Unlock();
  uint64_t record = make_config_record(config);

  HAL_StatusTypeDef status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, record);

  __enable_irq();
  HAL_FLASH_Lock();

  return status;
}

HAL_StatusTypeDef save_config(uint8_t config)
{
  if (!valid_config(config))
    config = DEFAULT_CONF;

  if (get_curr_conf() == config)
    return HAL_OK;

  uint32_t addr = get_empty_conf_addr();
  if (addr == 0)
  {
    // flash full
    if (flash_erase_page(CONFIG_FLASH_ADDR) != HAL_OK)
      return HAL_ERROR;

    if (flash_store(CONFIG_FLASH_ADDR, config) != HAL_OK)
      return HAL_ERROR;
  }
  return flash_store(addr, config);
}

static volatile uint32_t get_empty_conf_addr()
{
  uint32_t addr = CONFIG_FLASH_ADDR;

  for (uint32_t i = 0; i < CONFIG_NUM_SLOTS; i++)
  {
    volatile uint64_t* mem_ptr = (volatile uint64_t*)addr;
    if (*mem_ptr == FLASH_EMPTY_U64)
    {
      return addr;
    }
    addr += CONFIG_SLOT_SIZE;
  }
  return 0;
}
volatile uint8_t get_curr_conf(void)
{
  uint32_t addr = CONFIG_FLASH_ADDR;
  uint8_t last_valid_conf = DEFAULT_CONF;

  for (uint32_t i = 0; i < CONFIG_NUM_SLOTS; i++)
  {
    volatile uint64_t* mem_ptr = (volatile uint64_t*)addr;
    uint64_t record = *mem_ptr;

    if (record == FLASH_EMPTY_U64)
    {
      return last_valid_conf;
    }

    uint8_t conf = config_from_record(record);

    if (valid_config(conf))
    {
      last_valid_conf = conf;
    }

    addr += CONFIG_SLOT_SIZE;
  }

  return last_valid_conf;
}
