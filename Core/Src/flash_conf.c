#include "flash_conf.h"
#include "stm32g4xx_it.h"
#include <stdint.h>

// TODO:EEPROM emulation for storing config to save flash write cycles
/*
scan page
find last valid slot
use that config
*/

static void erase_flash_memory(uint32_t mem_addr)
{
  if ((mem_addr % 8U) != 0U)
  {
    return;
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

  HAL_FLASHEx_Erase(&flash_erase, &page_error);

  // lock for safety
  __enable_irq();
  HAL_FLASH_Lock();
}

// TODO: make this function only erase whenever necessary
void store_flash_memory(uint32_t mem_addr, uint64_t data)
{
  __disable_irq();
  HAL_FLASH_Unlock();

  erase_flash_memory(mem_addr);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, mem_addr, data);

  __enable_irq();
  HAL_FLASH_Lock();
}

uint64_t read_flash_memory(uint32_t mem_addr)
{
  return *(volatile uint64_t*)mem_addr;
}
