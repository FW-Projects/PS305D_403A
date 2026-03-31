/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f403a_407_int.c
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32f403a_407_int.h"
#include "wk_system.h"
/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */
#include "iap.h"
#include "tmt.h"

//#include "wk_usart.h"

#include "at32_Usart.h"
#include "PS305D_handle.h"
#include "ec11_handle.h"
#include "adc_filter.h"
#include "work_handle.h"
/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */
#define TIMES_100MS 100
#define TIMES_300MS 300
#define TIMES_500MS 500
#define TIMES_1S    1000
uint32_t us_tick = 0;        // 全局μs计时（供状态机用）
uint32_t tmt_us_tick = 0;
uint8_t ocp_exint_state = 0;
#define TMT_BASE_TICK_US 100         // TMT基础tick（1000μs=1ms，可按需调整）
/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */
void delay_tick_us(uint32_t us);
/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */
/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/* external variables ---------------------------------------------------------*/
/* add user code begin external variables */

/* add user code end external variables */

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
  /* add user code begin NonMaskableInt_IRQ 0 */
    /* add user code end NonMaskableInt_IRQ 0 */

  /* add user code begin NonMaskableInt_IRQ 1 */
    /* add user code end NonMaskableInt_IRQ 1 */
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
void HardFault_Handler(void)
{
  /* add user code begin HardFault_IRQ 0 */

    /* add user code end HardFault_IRQ 0 */
  /* go to infinite loop when hard fault exception occurs */
  while (1)
  {
    /* add user code begin W1_HardFault_IRQ 0 */
        /* add user code end W1_HardFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* add user code begin MemoryManagement_IRQ 0 */

    /* add user code end MemoryManagement_IRQ 0 */
  /* go to infinite loop when memory manage exception occurs */
  while (1)
  {
    /* add user code begin W1_MemoryManagement_IRQ 0 */
        /* add user code end W1_MemoryManagement_IRQ 0 */
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* add user code begin BusFault_IRQ 0 */

    /* add user code end BusFault_IRQ 0 */
  /* go to infinite loop when bus fault exception occurs */
  while (1)
  {
    /* add user code begin W1_BusFault_IRQ 0 */
        /* add user code end W1_BusFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* add user code begin UsageFault_IRQ 0 */

    /* add user code end UsageFault_IRQ 0 */
  /* go to infinite loop when usage fault exception occurs */
  while (1)
  {
    /* add user code begin W1_UsageFault_IRQ 0 */
        /* add user code end W1_UsageFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
  /* add user code begin SVCall_IRQ 0 */
    /* add user code end SVCall_IRQ 0 */
  /* add user code begin SVCall_IRQ 1 */
    /* add user code end SVCall_IRQ 1 */
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
  /* add user code begin DebugMonitor_IRQ 0 */
    /* add user code end DebugMonitor_IRQ 0 */
  /* add user code begin DebugMonitor_IRQ 1 */
    /* add user code end DebugMonitor_IRQ 1 */
}

/**
  * @brief  this function handles pendsv_handler exception.
  * @param  none
  * @retval none
  */
void PendSV_Handler(void)
{
  /* add user code begin PendSV_IRQ 0 */
    /* add user code end PendSV_IRQ 0 */
  /* add user code begin PendSV_IRQ 1 */
    /* add user code end PendSV_IRQ 1 */
}

/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
void SysTick_Handler(void)
{
  /* add user code begin SysTick_IRQ 0 */
	static uint16_t flicker_times = TIMES_100MS; 
	static uint16_t ec_run_time = TIMES_100MS;
	
//	ec_run_time--;
//	if(ec_run_time <= 0x00)
//	{
//		ec_run_time = 0;
//		 ec11_handle();
//	}
	
	ec11_handle();
	
		
	if(ps305d.General_parameters.vol_display_position != 0 || ps305d.General_parameters.cur_display_position != 0 || \
		ps305d.General_parameters.ocp_triggered_flag == true)
	{
		flicker_times--;
		if(flicker_times <= 0)
		{
			flicker_times = TIMES_100MS;
			ps305d.General_parameters.vol_flicker_display_flag = !ps305d.General_parameters.vol_flicker_display_flag;
			ps305d.General_parameters.cur_flicker_display_flag = !ps305d.General_parameters.cur_flicker_display_flag;
			ps305d.General_parameters.ocp_flicker_display_flag = !ps305d.General_parameters.ocp_flicker_display_flag;
			ps305d.General_parameters.led_error_flicker_display_flag = !ps305d.General_parameters.led_error_flicker_display_flag;
		}
	}
	
	
	
//    tmt.tick();
	USART1_TimeOutCounter();
    /* add user code end SysTick_IRQ 0 */

  wk_timebase_handler();

  /* add user code begin SysTick_IRQ 1 */
    /* add user code end SysTick_IRQ 1 */
}

/**
  * @brief  this function handles TMR1 Overflow and TMR10 handler.
  * @param  none
  * @retval none
  */
void TMR1_OVF_TMR10_IRQHandler(void)
{
  /* add user code begin TMR1_OVF_TMR10_IRQ 0 */
	if(tmr_interrupt_flag_get(TMR1, TMR_OVF_FLAG) != RESET)
	{
		tmr_flag_clear(TMR1, TMR_OVF_FLAG);
		
		// 1. 更新全局μs计时（供状态机用）
        us_tick++;
		
		tmt_us_tick++;
        if(tmt_us_tick >= TMT_BASE_TICK_US)
        {
            tmt.tick();       // 更新TMT任务计时（替代原SysTick）
            tmt_us_tick = 0;  // 重置累计值
        }
	}
  /* add user code end TMR1_OVF_TMR10_IRQ 0 */


  /* add user code begin TMR1_OVF_TMR10_IRQ 1 */

  /* add user code end TMR1_OVF_TMR10_IRQ 1 */
}



/* add user code begin 1 */

/**
 * @brief  微秒级延时函数（基于us_tick，可选）
 */
void delay_tick_us(uint32_t us)
{
	uint32_t start_tick = us_tick;
    // 等待us_tick累计到目标值（无忙等，CPU可执行其他操作）
    while((us_tick - start_tick) < us);
	
}
/* add user code end 1 */
