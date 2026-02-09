/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_adc.c
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
#include "wk_adc.h"

/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief  init adc1 function.
  * @param  none
  * @retval none
  */
void wk_adc1_init(void)
{
  /* add user code begin adc1_init 0 */

  /* add user code end adc1_init 0 */

  gpio_init_type gpio_init_struct;
  adc_base_config_type adc_base_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin adc1_init 1 */

  /* add user code end adc1_init 1 */

  /*gpio--------------------------------------------------------------------*/ 
  /* configure the IN10 pin */
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = NTC_ADC_PIN;
  gpio_init(NTC_ADC_GPIO_PORT, &gpio_init_struct);

  /* configure the IN6 pin */
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = ADC_I_PIN;
  gpio_init(ADC_I_GPIO_PORT, &gpio_init_struct);

  /* configure the IN7 pin */
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = ADC_V_PIN;
  gpio_init(ADC_V_GPIO_PORT, &gpio_init_struct);

  /* configure the IN14 pin */
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = TYPEC_A_PIN;
  gpio_init(TYPEC_A_GPIO_PORT, &gpio_init_struct);

  /* configure the IN15 pin */
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = TYPEC_V_PIN;
  gpio_init(TYPEC_V_GPIO_PORT, &gpio_init_struct);

  /* configure the IN8 pin */
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = USBA_A_PIN;
  gpio_init(USBA_A_GPIO_PORT, &gpio_init_struct);

  /* configure the IN9 pin */
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = USBA_V_PIN;
  gpio_init(USBA_V_GPIO_PORT, &gpio_init_struct);

  crm_adc_clock_div_set(CRM_ADC_DIV_4);

  /*adc_common_settings-------------------------------------------------------------*/ 
  adc_combine_mode_select(ADC_INDEPENDENT_MODE);

  /*adc_settings--------------------------------------------------------------------*/ 
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = FALSE;
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 1;
  adc_base_config(ADC1, &adc_base_struct);

  /* adc_ordinary_conversionmode-------------------------------------------- */
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_6, 1, ADC_SAMPLETIME_239_5);

  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);

  adc_ordinary_part_mode_enable(ADC1, FALSE);

  /* add user code begin adc1_init 2 */

  /* add user code end adc1_init 2 */
  
  adc_enable(ADC1, TRUE);
  
  /* adc calibration-------------------------------------------------------- */
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));

  /* add user code begin adc1_init 3 */

  /* add user code end adc1_init 3 */
}

/* add user code begin 1 */

/* add user code end 1 */
