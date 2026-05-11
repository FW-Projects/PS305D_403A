/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f403a_407_wk_config.h
  * @brief    header file of work bench config
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

/* define to prevent recursive inclusion -----------------------------------*/
#ifndef __AT32F403A_407_WK_CONFIG_H
#define __AT32F403A_407_WK_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* includes -----------------------------------------------------------------------*/
#include "stdio.h"
#include "at32f403a_407.h"

/* private includes -------------------------------------------------------------*/
/* add user code begin private includes */

/* add user code end private includes */

/* exported types -------------------------------------------------------------*/
/* add user code begin exported types */
#define JC_COM1_210_PIN    GPIO_PINS_13
#define JC_COM1_210_GPIO_PORT    GPIOA

#define JC_COM2_210_PIN    GPIO_PINS_14
#define JC_COM2_210_GPIO_PORT    GPIOA
/* add user code end exported types */

/* exported constants --------------------------------------------------------*/
/* add user code begin exported constants */

/* add user code end exported constants */

/* exported macro ------------------------------------------------------------*/
/* add user code begin exported macro */

/* add user code end exported macro */

/* add user code begin dma define */
/* user can only modify the dma define value */
//#define DMA1_CHANNEL1_BUFFER_SIZE   0
//#define DMA1_CHANNEL1_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL1_PERIPHERAL_BASE_ADDR  0

//#define DMA1_CHANNEL2_BUFFER_SIZE   0
//#define DMA1_CHANNEL2_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL2_PERIPHERAL_BASE_ADDR   0

//#define DMA1_CHANNEL3_BUFFER_SIZE   0
//#define DMA1_CHANNEL3_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL3_PERIPHERAL_BASE_ADDR   0

//#define DMA1_CHANNEL4_BUFFER_SIZE   0
//#define DMA1_CHANNEL4_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL4_PERIPHERAL_BASE_ADDR   0

//#define DMA1_CHANNEL5_BUFFER_SIZE   0
//#define DMA1_CHANNEL5_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL5_PERIPHERAL_BASE_ADDR   0

//#define DMA1_CHANNEL6_BUFFER_SIZE   0
//#define DMA1_CHANNEL6_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL6_PERIPHERAL_BASE_ADDR   0

//#define DMA1_CHANNEL7_BUFFER_SIZE   0
//#define DMA1_CHANNEL7_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL7_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL1_BUFFER_SIZE   0
//#define DMA2_CHANNEL1_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL1_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL2_BUFFER_SIZE   0
//#define DMA2_CHANNEL2_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL2_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL3_BUFFER_SIZE   0
//#define DMA2_CHANNEL3_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL3_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL4_BUFFER_SIZE   0
//#define DMA2_CHANNEL4_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL4_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL5_BUFFER_SIZE   0
//#define DMA2_CHANNEL5_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL5_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL6_BUFFER_SIZE   0
//#define DMA2_CHANNEL6_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL6_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL7_BUFFER_SIZE   0
//#define DMA2_CHANNEL7_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL7_PERIPHERAL_BASE_ADDR   0
/* add user code end dma define */

/* Private defines -------------------------------------------------------------*/
#define LIMIN_8_4V_PIN    GPIO_PINS_14
#define LIMIN_8_4V_GPIO_PORT    GPIOC
#define ONCE_OCP_PIN    GPIO_PINS_15
#define ONCE_OCP_GPIO_PORT    GPIOC
#define JK_2_PIN    GPIO_PINS_1
#define JK_2_GPIO_PORT    GPIOC
#define JK_1_PIN    GPIO_PINS_2
#define JK_1_GPIO_PORT    GPIOC
#define CONT_ONCE_OCP_PIN    GPIO_PINS_0
#define CONT_ONCE_OCP_GPIO_PORT    GPIOA
#define SW_KEY6_PIN    GPIO_PINS_1
#define SW_KEY6_GPIO_PORT    GPIOA
#define EC11_2R_PIN    GPIO_PINS_2
#define EC11_2R_GPIO_PORT    GPIOA
#define EC11_2L_PIN    GPIO_PINS_3
#define EC11_2L_GPIO_PORT    GPIOA
#define ADC_I_PIN    GPIO_PINS_6
#define ADC_I_GPIO_PORT    GPIOA
#define AMT_SGN_PIN    GPIO_PINS_2
#define AMT_SGN_GPIO_PORT    GPIOB
#define EC11_1R_PIN    GPIO_PINS_10
#define EC11_1R_GPIO_PORT    GPIOB
#define EC11_1L_PIN    GPIO_PINS_11
#define EC11_1L_GPIO_PORT    GPIOB
#define OUT_LED_PIN    GPIO_PINS_12
#define OUT_LED_GPIO_PORT    GPIOB
#define OUTPUT_KEY_PIN    GPIO_PINS_13
#define OUTPUT_KEY_GPIO_PORT    GPIOB
#define KEY_CC_CV_PIN    GPIO_PINS_14
#define KEY_CC_CV_GPIO_PORT    GPIOB
#define CONT_OCP_PIN    GPIO_PINS_15
#define CONT_OCP_GPIO_PORT    GPIOB
#define SW_KEY5_PIN    GPIO_PINS_6
#define SW_KEY5_GPIO_PORT    GPIOC
#define SDA_PIN    GPIO_PINS_8
#define SDA_GPIO_PORT    GPIOC
#define SCL_PIN    GPIO_PINS_9
#define SCL_GPIO_PORT    GPIOC
#define LIMIN_500MA_PIN    GPIO_PINS_8
#define LIMIN_500MA_GPIO_PORT    GPIOA
#define REALY_CAT_PIN    GPIO_PINS_11
#define REALY_CAT_GPIO_PORT    GPIOA
#define SPK_PIN    GPIO_PINS_12
#define SPK_GPIO_PORT    GPIOA
#define MOD3_PIN    GPIO_PINS_15
#define MOD3_GPIO_PORT    GPIOA
#define MOD1_PIN    GPIO_PINS_10
#define MOD1_GPIO_PORT    GPIOC
#define MOD2_PIN    GPIO_PINS_11
#define MOD2_GPIO_PORT    GPIOC
#define MOD4_PIN    GPIO_PINS_12
#define MOD4_GPIO_PORT    GPIOC
#define LIMIN_5V_PIN    GPIO_PINS_2
#define LIMIN_5V_GPIO_PORT    GPIOD
#define REL_AMT_PIN    GPIO_PINS_9
#define REL_AMT_GPIO_PORT    GPIOB

/* exported functions ------------------------------------------------------- */
  /* system clock config. */
  void wk_system_clock_config(void);

  /* config periph clock. */
  void wk_periph_clock_config(void);

  /* nvic config. */
  void wk_nvic_config(void);

/* add user code begin exported functions */

/* add user code end exported functions */

#ifdef __cplusplus
}
#endif

#endif
