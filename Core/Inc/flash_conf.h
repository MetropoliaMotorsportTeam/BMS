#ifndef FLASH_CONFIG_H
#define FLASH_CONFIG_H

#include "stm32g4xx_hal.h"
#include <stdint.h>

#define CONFIG_FLASH_ADDR 0x0801F800U
#define CONFIG_FLASH_PAGE 127U

#define CEIL(n, d) (((n) + (d) - 1) / (d))

void store_flash_memory(uint32_t mem_addr, uint64_t data);
uint64_t read_flash_memory(uint32_t mem_addr);

#endif
