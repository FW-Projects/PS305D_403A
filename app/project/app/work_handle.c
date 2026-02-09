#include "work_handle.h"
#include "PS305D_handle.h"
#include "perf_counter.h"
#include "adc_filter.h"

static int cont_ocp_protect_time = CONT_OCP_PROTECT_TIME;

void voltage_transformer_control(void);
void output_control(void);
void CV_handle(void);
void CC_handle(void);
void ocp_handle(void);
void CC_CV_OCP_handle(void);
void FAN_Control(void);
void Check_Ammeter(void);
void FAN_Control(void);

void work_handle(void)
{
	voltage_transformer_control();
	
	Check_Ammeter();
	
	output_control();
	
	FAN_Control();
	
	if(ps305d.output_state == OUTPUT)
		CC_CV_OCP_handle();
	else
		ps305d.work_mode = CV;
	
	
}

void Check_Ammeter(void)
{
	/* 查看电流表状态 */
	/* 上电默认无表状态 */
	/* 操作电流表前需要自动断开输出 否则容易误触发 */
	
	static uint8_t check_times = 0,check_times1 = 0;
	static uint8_t first_in = false;
	static uint8_t check_vol_times = 0;
	
	if(ps305d.General_parameters.current_limit_flag == true)
	{
		if(ps305d.output_state == OUTPUT)
		{
			//输出模式默认查输出模式
			ps305d.check_ammeter_mode = CHECK_OUPUT;
			
			if(ps305d.last_output_state == NO_OUTPUT)			
			{
				gpio_bits_set(GPIOB, GPIO_PINS_9);//切换500mA档位
			}
		}
		else
		{
			//无输出模式默认查GPIO模式
			
			gpio_bits_reset(GPIOB, GPIO_PINS_9);//切换5A档位
			
			ps305d.check_ammeter_mode = CHECK_GPIO_STATE;
		}
	}
	else
	{
		ps305d.check_ammeter_mode = CHECK_GPIO_STATE;
	}
	
	
	if(first_in == false)
	{
		/* 上电默认无表状态 */
		/* 后续通过查看电表状态操作继电器 */
		first_in = true;
		ps305d.last_output_state = NO_OUTPUT;
		OUTPUT_LED_CLOSE;
		OUTPUT_CLOSE;

		gpio_bits_reset(GPIOA,GPIO_PINS_11);  
	}
	else
	{
		if(ps305d.check_ammeter_mode == CHECK_GPIO_STATE)
		{
			/* 查看GPIOda
			状态确定电流表状态 */
			if(gpio_input_data_bit_read(GPIOB,GPIO_PINS_2) == false)
			{
				/* 有表状态  */
				check_times++;
				check_times1 = 0x00;
				if(check_times > 5)
				{
					check_times = 0;
					ps305d.ammeter_state = HAVE_AMMETER;
					if(ps305d.last_ammeter_state != ps305d.ammeter_state)
					{
						ps305d.last_ammeter_state = HAVE_AMMETER; 
						
						ps305d.output_state = NO_OUTPUT;
						ps305d.last_output_state = NO_OUTPUT;
						OUTPUT_LED_CLOSE;
						OUTPUT_CLOSE;
						
						/* 断开输出500ms再操作继电器*/
						delay_ms(500);
						
						gpio_bits_set(GPIOA,GPIO_PINS_11);
						
					}
				 	
				}
			}
			else
			{
				check_times1++;
				check_times = 0x00;
				if(check_times1 > 5)
				{
					check_times1 = 0;
					ps305d.ammeter_state = NO_AMMETER;
					if(ps305d.last_ammeter_state != ps305d.ammeter_state)
					{
 						ps305d.last_ammeter_state= NO_AMMETER;
						
						ps305d.last_output_state = NO_OUTPUT;
						ps305d.output_state = NO_OUTPUT;
						OUTPUT_LED_CLOSE;
						OUTPUT_CLOSE;
						
						/* 断开输出再操作继电器*/
						delay_ms(1000);
						
						gpio_bits_reset(GPIOA,GPIO_PINS_11);
						

					}
				}
			}
		}
		else if(ps305d.check_ammeter_mode == CHECK_OUPUT && ps305d.system_parameters.set_current_data != 0x00)
		{
			if(ps305d.system_parameters.actual_voltage_data <= 0x00)
			{
				check_vol_times++;
				if(check_vol_times > 20)
				{
					check_vol_times = 0;
					
					ps305d.last_output_state = NO_OUTPUT;
					ps305d.output_state = NO_OUTPUT;
					OUTPUT_LED_CLOSE;
					OUTPUT_CLOSE;
				}
				
			}
			else
				check_vol_times = 0;

		}
	}
}

void FAN_Control(void)
{
	/* 风扇控制 */
	static int check_tmep_times = 3000;
	static bool in_first = false;
	if(in_first == false)
	{
		in_first = true;
		tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_2, 5599 * 0.4);
	}
		
	check_tmep_times--;
	if(check_tmep_times <= 0)
	{
		check_tmep_times = 3000;
		ps305d.system_parameters.ntc_value = get_adcval_average(ADC_NTC,10);
		
		if (ps305d.system_parameters.ntc_value < 0x250)
			tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_2, 5599 * 0.4);
		else if (ps305d.system_parameters.ntc_value < 0x300)
			tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_2, 5599 * 0.6);
		else if (ps305d.system_parameters.ntc_value < 0x350)
			tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_2, 5599 * 0.8);
		else 
			tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_2, 5599);
	}
}

void CC_CV_OCP_handle(void)
{
	/* 恒压恒流过流处理 */
	switch(ps305d.work_mode)
	{
		case CV:
			CV_handle();
			break;
		case CC:
			CC_handle();
			break;
		case OCP:
			ocp_handle();
			break;
		default:break;
	}
}

void ocp_handle(void)
{
	/*过流处理 */
	ps305d.General_parameters.ocp_triggered_flag = true;
	
	if(ps305d.ocp_mode == CONT_OCP_MODE)
	{
		/* 多次过流触发立马断开输出 */
		/* 断开输出持续一段时间后恢复输出且退出过流模式 */
		/* 多次过流触发 灯常亮 & 蜂鸣器响三次 */
		cont_ocp_protect_time--;
		
		//关输出
//		ps305d.last_output_state = OUTPUT;
//		ps305d.output_state = NO_OUTPUT;
		OUTPUT_CLOSE; //断输出
		OUTPUT_LED_CLOSE;

		if(cont_ocp_protect_time <= 0)
		{
			ps305d.work_mode = CV;
			
			//开输出
			ps305d.last_output_state = NO_OUTPUT;
			ps305d.output_state = OUTPUT;

			ps305d.General_parameters.ocp_triggered_flag = false;
		}
	}
	else if(ps305d.ocp_mode == ONCE_OCP_MODE)
	{
		/* 单次过流直接断开输出 */
		/* 且复位单次过流后才可重新打开输出 */
		/* 单次过流触发 灯闪烁 & 蜂鸣器响三次 */
//		ps305d.last_output_state = NO_OUTPUT;
//		OUTPUT_LED_CLOSE;
//		OUTPUT_CLOSE;
		
		//关输出
		ps305d.last_output_state = OUTPUT;
		ps305d.output_state = NO_OUTPUT;
		OUTPUT_CLOSE;
		OUTPUT_LED_CLOSE;
	}
}

void CV_handle(void)
{
	/* 恒压处理 */
	static uint8_t ocp_times = 0;
	if(gpio_input_data_bit_read(GPIOC,GPIO_PINS_7) == false)
	{
		/* 过流触发 */
		ocp_times++;
		if(ocp_times > OCP_TRIGGER_NUMBER)
		{
			/* 进入 恒流/过流 */
			ocp_times = 0x00;
			if(ps305d.ocp_mode != NO_OCP_MODE)
			{
				/* 进入过流 */
				ps305d.work_mode = OCP;
				if(ps305d.ocp_mode == CONT_OCP_MODE)
					cont_ocp_protect_time = CONT_OCP_PROTECT_TIME;
			}
			else
			{
				/* 进入恒流 */
				ps305d.work_mode = CC;
			}
		}
	}
	else
	{
		/* 恒压状态 */
		ocp_times = 0x00;
		ps305d.work_mode = CV;
	}
}

void CC_handle(void)
{
	/* 恒流处理 */
	static uint8_t cv_times = 0;
	if(gpio_input_data_bit_read(GPIOC,GPIO_PINS_7) == true)
	{
		cv_times++;
		if(cv_times > CV_TRIGGER_NUMBER)
		{
			cv_times = 0;
			ps305d.work_mode = CV;
		}
		
	}
	else
	{
		cv_times = 0;
		if(ps305d.ocp_mode != NO_OCP_MODE)
		{
			ps305d.work_mode = OCP;
			if(ps305d.ocp_mode == CONT_OCP_MODE)
				cont_ocp_protect_time = CONT_OCP_PROTECT_TIME;
		}
		else
			ps305d.work_mode = CC;
	}
}

void output_control(void)
{
	/* 输出控制 */
	if(ps305d.last_output_state != ps305d.output_state)
	{
		if(ps305d.last_output_state == NO_OUTPUT && ps305d.output_state == OUTPUT)
		{
			if(ps305d.init_transformer_control == true)
			{
				/* 继电器操作完毕 */
				OUTPUT_LED_OPEN;
				OUTPUT_OPEN;
				ps305d.init_transformer_control = false;
				ps305d.last_output_state = OUTPUT;
				
			}
		}
		else if(ps305d.last_output_state == OUTPUT && ps305d.output_state == NO_OUTPUT)
		{
			ps305d.last_output_state = NO_OUTPUT;
			OUTPUT_LED_CLOSE;
			OUTPUT_CLOSE;
		}
	}
}

void voltage_transformer_control(void)
{
	static uint16_t last_voltage = 0x00;
	static uint8_t first_in = false;
	
	/* 变压器输出控制
	 *  交流         直流
	 * 16 - 26V =  0 - 6V
	 * 16 - 34V =  6 - 14V
	 *  0 - 26V = 14 - 21V
	 *  0 - 34V = 21 - 30V
	*/

	if((last_voltage != ps305d.system_parameters.set_voltage_data || first_in == false))
	{
		last_voltage = ps305d.system_parameters.set_voltage_data;
		if(ps305d.system_parameters.set_voltage_data < 500)
		{
			JK_1_LOW;
			JK_2_HIGH;
		}
		else if(ps305d.system_parameters.set_voltage_data < 1400)
		{
			JK_1_HIGH;
			JK_2_HIGH;
		}
		else if(ps305d.system_parameters.set_voltage_data < 2100)
		{
			JK_1_LOW;
			JK_2_LOW;
		}
		else
		{
			JK_1_HIGH;
			JK_2_LOW;
		}		
//		delay_ms(1);
		//继电器闭合时间MAX 10ms
		//继电器打开时间MAX 5ms
		
		//变压器输出换档时先操作继电器后打开输出
		ps305d.init_transformer_control = true;
		
		
	}	
}