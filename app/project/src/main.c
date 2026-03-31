/* add user code begin Header */
/**
 **************************************************************************
 * @file     main.c
 * @brief    main program
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
 * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABsILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 *
 **************************************************************************
 */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "at32f403a_407_wk_config.h"
#include "wk_adc.h"
#include "wk_crc.h"
#include "wk_dac.h"
#include "wk_debug.h"
#include "wk_tmr.h"
#include "wk_usart.h"
#include "wk_wdt.h"
#include "wk_gpio.h"
#include "wk_system.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */
#include "stdbool.h"
#include "iap.h"
#include "tmt.h"
#include "PC_comm_handle.h"
#include "EventRecorder.h"
#include "perf_counter.h"
#include "lcd_handle.h"
#include "ec11_handle.h"
#include "PS305D_handle.h"
#include "beep_handle.h"
#include "output_handle.h"
#include "key_handle.h"
#include "work_handle.h"
#include "collect_data_handle.h"
#include "adc_filter.h"
#include "PID_handle.h"
#include "flash_handle.h"
#include "at32f403a_407_int.h"
/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */
#define FEED_DOG_TASK_TIME 1000
#define OCP_TASK_TIME 1
/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */
static uint16_t cur_data = 0x00;
/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */
void iap_task(void);
void feed_dog_task(void);
void pc_task(void);
void lcd_task(void);
void ec11_task(void);
void beep_task(void);
void Output_task(void);
void key_task(void);
void work_task(void);
void collect_data_task(void);
void flash_task(void);
void check_ocp_task(void);
void task_ocp(void);
/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* add user code begin 1 */
  nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x4000);
  /* add user code end 1 */

  /* system clock config. */
  wk_system_clock_config();

  /* config periph clock. */
  wk_periph_clock_config();

  /* init debug function. */
  wk_debug_config();

  /* nvic config. */
  wk_nvic_config();

  /* timebase config. */
  wk_timebase_init();

  /* init gpio function. */
  wk_gpio_config();

  /* init usart1 function. */
  wk_usart1_init();

  /* init adc1 function. */
  wk_adc1_init();

  /* init tmr1 function. */
  wk_tmr1_init();

  /* init tmr4 function. */
  wk_tmr4_init();

  /* init dac function. */
  wk_dac_init();

  /* init crc function. */
  wk_crc_init();

  /* init wdt function. */
  wk_wdt_init();

  /* add user code begin 2 */
  tmt_init();

  tmt.create(iap_task, IAP_TASK_TIME );
  tmt.create(feed_dog_task, FEED_DOG_TASK_TIME );
  tmt.create(pc_task,        PC_HANDLE_TIME );
  tmt.create(lcd_task, LCD_TASK_TIME );
  tmt.create(beep_task, BEEP_TASK_TIME );
  tmt.create(Output_task, OUTPUT_TASK_TIME );
  tmt.create(key_task, KEY_TASK_TIME );
  tmt.create(collect_data_task, COLLECT_DATA_TASK_TIME );
  tmt.create(work_task, WORK_TASK_TIME );
  tmt.create(flash_task,FLASH_TASK_TIME );
  tmt.create(check_ocp_task,OCP_TASK_TIME );

  init_cycle_counter(true);
  EventRecorderInitialize(0, 1);
  PS305D_init(&ps305d);

  filter_init(&ADC_CRT, ADC_CHANNEL_6);
  filter_init(&ADC_VTG, ADC_CHANNEL_7);
  filter_init(&ADC_TYPEC_CRT, ADC_CHANNEL_14);
  filter_init(&ADC_USB_CRT, ADC_CHANNEL_8);
  Compensator_Init(&voltage_pid, 1, 0, 0, 60);
  Compensator_Init(&current_pid, 1, 0, 0, 60);
  TM1680Init();
  BSP_UsartInit();
  iap_init();
  /* wait for system ready */
  /* add user code end 2 */

  while(1)
  {
    /* add user code begin 3 */
    tmt.run();
    /* add user code end 3 */
  }
}

  /* add user code begin 4 */
void iap_task(void)
{
  iap_command_handle();
}
void feed_dog_task(void)
{
  static bool first_in = FALSE;

  if (first_in == FALSE)
  {
    first_in = TRUE;
    /* if enabled, please feed the dog through wdt_counter_reload() function */
    // wdt_enable();
  }

  wdt_counter_reload();
}

 void pc_task(void)
{
     pc_comm_handle();
 }

void lcd_task(void)
{  
  lcd_handle();
}


void flash_task(void)
{
	flash_handle();
}

void beep_task(void)
{
	beep_handle();
}

void Output_task(void)
{
#if 1
  if (ps305d.output_state == OUTPUT)
    Power_Output_Control_Loop();
#endif

#if 0
	Power_Output_Control_Loop();
#endif
}

void key_task(void)
{
  key_handle();
}

void work_task(void)
{
#if 1
  work_handle();
#endif
}

void collect_data_task(void)
{
#if 1
  collect_data_handle();
#endif
}

void check_ocp_task(void)
{
	if(ps305d.output_state == OUTPUT)
	{
		if( ps305d.work_mode == CV)
		{	
			CV_handle();
		}
	}
//	else
//	{
//		ps305d.work_mode = CV;
//	}
}




/* add user code end 4 */
