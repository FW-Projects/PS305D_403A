/**
  **************************************************************************
  * @file     flash.c
  * @brief    flash program
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

#include "flash.h"



/**
  * @brief  flash write.
  * @param  none
  * @retval none
  */
void flash_write(uint8_t data,uint32_t addr)
{
    flash_unlock();
    flash_sector_erase(addr);
    flash_byte_program(addr, data);
    flash_lock();
}

/**
  * @}
  */
uint8_t flash_read(uint32_t addr)
{
	uint8_t data;
	data = (*(uint32_t*)addr);
    return data;
}

void flash_write_halfword(uint16_t data,uint32_t addr)
{
    flash_unlock();
    flash_sector_erase(addr);
    flash_halfword_program(addr, data);
    flash_lock();
}

uint16_t flash_read_halfword(uint32_t addr)
{
	volatile uint16_t data;
	volatile uint8_t data_h;
	volatile uint8_t data_l;
	data_l = (*(uint32_t*)addr);
	addr = addr +1;
	data_h = (*(uint32_t*)addr);
	data = (data_h*256) + data_l;
    return data;
}

/**
  * @}
  */
