/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_dac.c
  * @brief    work bench config program
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
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "wk_dac.h"

/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief  init dac function.
  * @param  none
  * @retval none
  */
void wk_dac_init(void)
{
  /* add user code begin dac_init 0 */

  /* add user code end dac_init 0 */
  gpio_init_type gpio_init_struct;
  gpio_default_para_init(&gpio_init_struct);
  
  /* add user code begin dac_init 1 */

  /* add user code end dac_init 1 */

  /*gpio------------------------------------------------------------------------------*/ 
  /* configure the OUT1 pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_4;
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure the OUT2 pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_5;
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  /*dac OUT1 init--------------------------------------------------------------------*/
  dac_trigger_enable(DAC1_SELECT, FALSE);

  dac_output_buffer_enable(DAC1_SELECT, FALSE);

  dac_enable(DAC1_SELECT, TRUE);

  /*dac OUT2 init--------------------------------------------------------------------*/
  dac_trigger_enable(DAC2_SELECT, FALSE);

  dac_output_buffer_enable(DAC2_SELECT, FALSE);

  dac_enable(DAC2_SELECT, TRUE);

  /* add user code begin dac_init 2 */

  /* add user code end dac_init 2 */
}

/* add user code begin 1 */

/* add user code end 1 */
