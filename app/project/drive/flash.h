/**
  **************************************************************************
  * @file     flash.h
  * @brief    flash header file
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at32f403a_407.h"

/** @addtogroup UTILITIES_examples
  * @{
  */

/** @addtogroup USART_iap_bootloader
  * @{
  */

/** @defgroup bootloader_definition
  * @{
  */

#define FLASH_SIZE                2
#define FLASH_START_ADDRESS       (0x08000000 + 1024 * 1024)

/**
  * @}
  */

/** @defgroup bootloader_exported_functions
  * @{
  */

void flash_write(uint8_t data,uint32_t addr);
uint8_t flash_read(uint32_t addr);
void flash_write_halfword(uint16_t data,uint32_t addr);
uint16_t flash_read_halfword(uint32_t addr);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}

#endif

#endif
