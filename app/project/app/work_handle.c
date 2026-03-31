#include "work_handle.h"
#include "PS305D_handle.h"
#include "perf_counter.h"
#include "adc_filter.h"
#include "at32f403a_407_conf.h"
#include "output_handle.h"
#include "collect_data_handle.h"
#include "at32f403a_407_int.h"
#include "at32f403a_407.h"            

int cont_ocp_protect_time = CONT_OCP_PROTECT_TIME_VAL; // 持续过流保护计时
bool dac_control_flag = false;


typedef enum {
    CV_STATE_IDLE = 0,    // 正常CV模式（初始状态）
    CV_STATE_LATCH,       // 瞬时锁存关输出（20μs窗口期）
    CV_STATE_SHORT,       // 短路锁定（200ms）
    CV_STATE_SURGE        // 浪涌限流（20ms）
} CV_StateTypeDef;
 

static CV_StateTypeDef cv_state = CV_STATE_IDLE; // 状态机当前状态
static uint32_t state_timer_us = 0;              // 状态计时（μs）
static uint16_t state_timer_ms = 0; // 改用ms级计时（适配10ms观察期）
#define LATCH_OFF_US       20      // 瞬时锁存时间（20μs，兼顾判定+兼容）
#define SHORT_LOCK_US      2000000  // 短路锁定时间（200ms=200000μs）
#define SURGE_HOLD_US      20000   // 浪涌限流时间（20ms=20000μs）
#define SHORT_CURR_THRESH  (ps305d.system_parameters.set_current_data - 50) // 短路电流阈值
#define OCPSENSE_DELAY_MS   5      // 观察时间（10ms，覆盖浪涌时长）
#define SURGE_CURR_THRESH   2000    // 浪涌电流阈值（2A，根据你电源调整）
/**
 * @brief 变压器档位控制（根据设置电压切换交流输入档位）
 */
void voltage_transformer_control(void);

/**
 * @brief 输出开关控制（继电器+指示灯）
 */
void output_control(void);

/**                                                                                                                                                                                                                                                                                                                                                                                
 * @brief 恒压模式处理逻辑
 */
void CV_handle(void);

/**
 * @brief 恒流模式处理逻辑
 */
void CC_handle(void);

/**
 * @brief 过流保护模式处理逻辑
 */
void ocp_handle(void);

/**
 * @brief 恒压/恒流/过流模式总处理
 */
void CC_CV_OCP_handle(void);

/**
 * @brief 风扇转速控制（根据NTC温度自动调节PWM）
 */
void FAN_Control(void);

/**
 * @brief 电流表状态检测（GPIO/输出状态双判断）
 */
void Check_Ammeter(void);

/**
 * @brief 电压错误检测
 * @param actual_volt: 实际电压值
 * @param set_volt: 设置电压值
 * @return 电压错误状态（正常/高压错误/低压错误）
 */
error_state_e checkVoltageError(float actual_volt, float set_volt);



void work_handle(void)
{
	 // 1. 交流档位控制（根据设置电压切换变压器输入）
	voltage_transformer_control();
	
	 // 2. 电流表状态检测（判断有无电流表，控制继电器）
	Check_Ammeter();
	
	// 3. 输出继电器+指示灯控制
	output_control();
	
	// 4. 风扇转速自动控制（根据NTC温度）
	FAN_Control();
	
	// 5. 恒压/恒流/过流模式处理（仅输出开启时执行）
	CC_CV_OCP_handle();

//	 // 6. 电压错误检测（实际电压vs设置电压）
//	ps305d.error_state = checkVoltageError(ps305d.system_parameters.actual_voltage_data,
//											ps305d.system_parameters.set_voltage_data);
	
}



/************************** 电压错误检测函数 **************************/
/**
 * @brief 电压错误检测（高压/低压判断）
 * @param actual_volt: 实际测量电压值（V）
 * @param set_volt: 目标设置电压值（V）
 * @return 电压错误状态枚举
 * @note  低压错误仅在输出开启时判定，无输出时默认正常
 */
error_state_e checkVoltageError(float actual_volt,float set_volt)
{
	static uint8_t error_times = 0x00;
	 // 电压差值 = 实际电压 - 设置电压
    float volt_diff = actual_volt - set_volt;
	
    if (volt_diff > HIGH_ERROR_THRESHOLD) 
	{
		error_times++;
		if(error_times > 10)
		{
			error_times = 0x00;
		// 高压错误：实际电压超过设置电压阈值
			return VOLTAGE_HIGH_ERROR;
		}
		else
			return VOLTAGE_NORMAL;
    }	
	 
	// else if (volt_diff < -LOW_ERROR_THRESHOLD)
	// {
    //     // 低压错误：实际电压低于设置电压阈值（仅输出开启时判定）
	// 	if(ps305d.output_state == OUTPUT)
	// 	{
	// 		return VOLTAGE_LOW_ERROR;
	// 	}
	// 	else
	// 		return VOLTAGE_NORMAL;
    // } 
	else 
	{
		error_times = 0x00;
        // 电压在允许范围内 → 正常
        return VOLTAGE_NORMAL;
    }
}

/************************** 电流表状态检测函数 **************************/
/**
 * @brief 电流表状态检测（双模式：GPIO检测/输出检测）
 * @note  1. GPIO模式：通过GPIO引脚判断有无电流表，控制继电器
 * @note  2. 输出模式：检测输出电压，异常时关闭输出
 * @note  操作继电器前先断开输出，避免误触发
 */
void Check_Ammeter(void)
{
	// 静态变量：检测计数（防抖）、首次进入标记
    static uint8_t check_times = 0;      // 有表检测计数
    static uint8_t check_times1 = 0;     // 无表检测计数
    static uint8_t first_in = false;     // 首次进入标记
    static uint8_t check_vol_times = 0;  // 输出电压检测计数
	
	if(ps305d.General_parameters.current_limit_flag == true)
	{
		if(ps305d.output_state == OUTPUT)
		{
			// 输出模式：默认检测输出状态
			ps305d.check_ammeter_mode = CHECK_OUPUT;
			
			if(ps305d.last_output_state == NO_OUTPUT)			
			{
				// 切换到500mA档位
				gpio_bits_set(GPIOB, GPIO_PINS_9);	
			}
		}
		else
		{
			// 无输出模式：默认检测GPIO状态
			ps305d.check_ammeter_mode = CHECK_GPIO_STATE;
			// 切换到5A档位
			gpio_bits_reset(GPIOB, GPIO_PINS_9);
			
			
		}
	}
	else
	{
		// 无限流标志：默认检测GPIO状态
		ps305d.check_ammeter_mode = CHECK_GPIO_STATE;
	}
	
	
	 // 首次进入初始化（上电默认无输出、无表状态）
	if(first_in == false)
	{
		first_in = true;
		ps305d.last_output_state = NO_OUTPUT;
		OUTPUT_LED_CLOSE; // 关闭输出指示灯 
		OUTPUT_CLOSE;     // 关闭输出
		gpio_bits_reset(GPIOA,GPIO_PINS_11);  // 复位电流表继电器
	}
	else
	{
		// 非首次进入：执行状态检测
		if(ps305d.check_ammeter_mode == CHECK_GPIO_STATE)
		{
			// 模式1：检测GPIO状态判断电流表
			if(gpio_input_data_bit_read(GPIOB,GPIO_PINS_2) == false)
			{
				// GPIO引脚为低：有电流表
				check_times++;
				check_times1 = 0x00;
				if(check_times > AMMETER_CHECK_TIMES)
				{
					 // 连续检测5次稳定：判定为有表
					check_times = 0;
					ps305d.ammeter_state = HAVE_AMMETER;
					
					// 状态变化时执行继电器操作
					if(ps305d.last_ammeter_state != ps305d.ammeter_state)
					{
						ps305d.last_ammeter_state = HAVE_AMMETER; 
						ps305d.output_state = NO_OUTPUT;
						ps305d.last_output_state = NO_OUTPUT;
						OUTPUT_LED_CLOSE;
						OUTPUT_CLOSE;
						delay_ms(AMMETER_RELAY_DELAY_HAVE);  // 断开输出500ms后操作
						gpio_bits_set(GPIOA,GPIO_PINS_11);   // 置位电流表继电器
					}
				}
			}
			else
			{
				// GPIO引脚为高：无电流表
				check_times1++;
				check_times = 0x00;
				if(check_times1 > AMMETER_CHECK_TIMES)
				{
					 // 连续检测5次稳定：判定为无表
					check_times1 = 0;
					ps305d.ammeter_state = NO_AMMETER;
					if(ps305d.last_ammeter_state != ps305d.ammeter_state)
					{
 						ps305d.last_ammeter_state= NO_AMMETER;
						ps305d.last_output_state = NO_OUTPUT;
						ps305d.output_state = NO_OUTPUT;
						OUTPUT_LED_CLOSE;
						OUTPUT_CLOSE;
						delay_ms(AMMETER_RELAY_DELAY_NO);    // 断开输出1000ms后操作
						gpio_bits_reset(GPIOA,GPIO_PINS_11); // 复位电流表继电器
					}
				}
			}
		}
		 // 模式2：检测输出状态（仅设置电流非零时执行）
		else if(ps305d.check_ammeter_mode == CHECK_OUPUT && ps305d.system_parameters.set_current_data != 0x00)
		{
			// 实际电压为0：判定输出A异常，累计计数
			if(ps305d.system_parameters.actual_voltage_data <= 0x00)
			{
				check_vol_times++;
				// 连续20次检测异常：关闭输出
				if(check_vol_times > VOLTAGE_CHECK_TIMES)
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


/************************** 风扇控制函数 **************************/
/**
 * @brief 风扇PWM调速控制（根据NTC温度自动调节）
 * @note  1. 首次进入初始化PWM为40%
 * @note  2. 每3000次循环检测一次NTC温度
 * @note  3. 温度越高，PWM占空比越大（40%→60%→80%→100%）
 */
void FAN_Control(void)
{
	/* 风扇控制 */
	static int check_tmep_times = FAN_CHECK_INTERVAL; // 温度检测计时
    static bool in_first = false;                    // 首次进入标记
	
	// 首次进入：初始化风扇PWM为40%
	if(in_first == false)
	{
		in_first = true;
		tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_2, FAN_PWM_BASE_VAL * FAN_PWM_SCALE_40);
	}
	
	
	check_tmep_times--;
	if(check_tmep_times <= 0)
	{                                                                                      
		check_tmep_times = FAN_CHECK_INTERVAL;
		
		ps305d.system_parameters.ntc_value = get_adcval_average(ADC_NTC,10);
		
		if (ps305d.system_parameters.ntc_value < NTC_TEMP_THRESHOLD_1)
		{
			 // 低温：40%转速
			tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_2, FAN_PWM_BASE_VAL * FAN_PWM_SCALE_40);
		}
		else if (ps305d.system_parameters.ntc_value < NTC_TEMP_THRESHOLD_2)
		{
			 // 中温：60%转速
			tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_2, FAN_PWM_BASE_VAL * FAN_PWM_SCALE_60);
		}
		else if (ps305d.system_parameters.ntc_value < NTC_TEMP_THRESHOLD_3)
		{
			// 高温：80%转速
			tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_2, FAN_PWM_BASE_VAL * FAN_PWM_SCALE_80);
		}
		else 
		{
			// 超高温：100%转速
			tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_2, FAN_PWM_BASE_VAL * FAN_PWM_SCALE_100);
		}
	}
}


/************************** 模式总处理函数 **************************/
/**
 * @brief 恒压/恒流/过流模式分发处理
 * @note  根据当前工作模式调用对应处理函数
 */
void CC_CV_OCP_handle(void)
{
	switch(ps305d.work_mode)
	{
		case CV: 
//			CV_handle();
			break;
		case CC:     // 恒流模式
			CC_handle();
			break;
		case OCP:    // 过流保护模式
			ocp_handle();
			break;
		default:break;
	}
}

/************************** 过流保护处理函数 **************************/
/**
 * @brief 过流保护模式处理（分持续/单次模式）
 * @note  1. 持续模式：自动关闭输出→计时→恢复输出→切回恒压
 * @note  2. 单次模式：永久关闭输出，需手动复位
 */
void ocp_handle(void)
{
	ps305d.General_parameters.ocp_triggered_flag = true; // 标记过流已触发
	
	  // 模式1：持续过流保护（自动恢复）
	if(ps305d.ocp_mode == CONT_OCP_MODE)
	{
		cont_ocp_protect_time--;
	
		OUTPUT_CLOSE; //断输出
		OUTPUT_LED_CLOSE;
		 ps305d.output_state = NO_OUTPUT;
		// 计时结束：恢复输出并切回恒压模式
		if(cont_ocp_protect_time <= 0)
		{
			ps305d.work_mode = CV;		
			ps305d.last_output_state = NO_OUTPUT;  
			ps305d.output_state = OUTPUT;
			ps305d.General_parameters.ocp_triggered_flag = false;
			cv_state = CV_STATE_IDLE;
		}
	}
	// 模式2：单次过流保护（需手动复位）
	else if(ps305d.ocp_mode == ONCE_OCP_MODE)
	{
		 // 永久关闭输出（需外部复位）
		ps305d.last_output_state = OUTPUT;
		ps305d.output_state = NO_OUTPUT;
		OUTPUT_CLOSE;
		OUTPUT_LED_CLOSE;
	}
}


/************************** 恒压模式处理函数 **************************/
/**
 * @brief 恒压模式逻辑（检测过流，切换模式）
 * @note  GPIO7为低表示过流，累计触发5次后切换模式：
 * @note  - 有过流保护：切OCP模式
 * @note  - 无过流保护：切CC模式
 */

void CV_handle(void)
{
    static uint8_t ocp_pin = 0;
    static uint16_t current_data = 0x00;
	
    switch(cv_state)
    {
        // 状态1：正常CV模式（初始/恢复状态）
        case CV_STATE_IDLE:
			ocp_pin = gpio_input_data_bit_read(GPIOC,GPIO_PINS_7);
            if(ocp_pin == false) // OCPSENSE触发过流
            {
                cv_state = CV_STATE_LATCH;
                state_timer_us = us_tick;
				ocp_exint_state = 0;
            }
            else
            {
                if(ps305d.General_parameters.ocp_triggered_flag == false)
                    ps305d.work_mode = CV;
            }
            break;

        // 状态2：瞬时锁存（20μs窗口期，等电流上升到位）
        case CV_STATE_LATCH:
            if((us_tick - state_timer_us) >= LATCH_OFF_US)
            {
                current_data = get_adcval(ADC_CHANNEL_6) * 1000 / 819;
                if(current_data >= SHORT_CURR_THRESH)
                {
                    // 真短路：切换到OCP锁定状态，避免流转到浪涌
					
                    state_timer_us = us_tick;
                   if(ps305d.ocp_mode != NO_OCP_MODE)
					{
						// 有过流保护：切OCP模式
						ps305d.work_mode = OCP;
						if(ps305d.ocp_mode == CONT_OCP_MODE)
						{
							// 持续过流模式：重置保护计时
							cont_ocp_protect_time = CONT_OCP_PROTECT_TIME_VAL;
						}
					}
					 // 无过流保护：切CC模式
					else
					{
						/* 进入恒流 */
						ps305d.work_mode = CC;
 					}
                }
                else
                {
                    // 浪涌：仅浪涌场景进入SURGE状态
                    cv_state = CV_STATE_SURGE;
                    state_timer_us = us_tick;
                }
            }
            break;

        // 状态4：浪涌限流（20ms，仅浪涌触发）
        case CV_STATE_SURGE:
            if((us_tick - state_timer_us) >= SURGE_HOLD_US)
            {
                cv_state = CV_STATE_IDLE;
                ps305d.work_mode = CV;
            }
            break;

        // 兜底：异常状态回归IDLE
        default:
            cv_state = CV_STATE_IDLE;
            state_timer_us = us_tick;
            ps305d.work_mode = CV;                       
            ps305d.General_parameters.ocp_triggered_flag = false;
            break;
    }
}


/************************** 恒流模式处理函数 **************************/
/**
 * @brief 恒流模式逻辑（检测过流恢复，切换模式）
 * @note  GPIO7为高表示过流恢复，累计触发5次后切回CV模式；
 * @note  GPIO7为低表示仍过流，根据过流保护配置切换模式
 */
void CC_handle(void)
{
	static uint8_t cv_times = 0;
	
	if(gpio_input_data_bit_read(GPIOC,GPIO_PINS_7) == true)
	{
		cv_times++;
		// 累计触发次数超过阈值：切回恒压模式
		if(cv_times > CV_TRIGGER_NUMBER_VAL)
		{
			cv_times = 0;
			ps305d.work_mode = CV;
		}
	}
	 // GPIO7为低：仍过流
	else
	{                                                                                                                                     
		cv_times = 0;
		if(ps305d.ocp_mode != NO_OCP_MODE)
		{
			ps305d.work_mode = OCP;
			if(ps305d.ocp_mode == CONT_OCP_MODE)
				cont_ocp_protect_time = CONT_OCP_PROTECT_TIME_VAL;
		}
		else
			ps305d.work_mode = CC;
	}
}


/************************** 输出控制函数 **************************/
/**
 * @brief 输出继电器+指示灯控制（仅状态变化时执行）
 * @note  1. 无输出→有输出：打开指示灯+继电器（需变压器初始化完成）
 * @note  2. 有输出→无输出：关闭指示灯+继电器
 */
void output_control(void)
{
	// 仅输出状态变化时执行操作
	if(ps305d.error_state == VOLTAGE_HIGH_ERROR || ps305d.error_state == VOLTAGE_LOW_ERROR)
	{
		// 电压错误时强制关闭输出
		ps305d.output_state = NO_OUTPUT;
	}
	else
	{
		if(ps305d.last_output_state != ps305d.output_state)
		{
			// 切换为有输出
			if(ps305d.last_output_state == NO_OUTPUT && ps305d.output_state == OUTPUT)
			{
				// 变压器初始化完成后执行
				if(ps305d.init_transformer_control == true)
				{
					g_last_set_voltage_data = 0xffff;
					OUTPUT_LED_OPEN;
					OUTPUT_OPEN;
					ps305d.init_transformer_control = false;
					ps305d.last_output_state = OUTPUT;
				}
			}
			// 切换为无输出
			else if(ps305d.last_output_state == OUTPUT && ps305d.output_state == NO_OUTPUT)
			{
				ps305d.last_output_state = NO_OUTPUT;
				OUTPUT_LED_CLOSE;
				OUTPUT_CLOSE;
			}
		}
	}
}


/************************** 变压器档位控制函数 **************************/
/**
 * @brief 变压器档位控制（根据设置电压切换交流输入档位）
 * @note  档位对应关系（直流输出→交流输入）：
 * @note  - 0~6V  → 16-26V
 * @note  - 6~14V → 16-34V
 * @note  -14~21V → 0-26V
 * @note  -21~30V → 0-34V
 */
void voltage_transformer_control(void)
{
	static uint8_t check_times = 0;      // 检测计数（防抖）
	static uint16_t last_voltage = 0x00; // 上一次设置电压（防抖）
    static uint8_t first_in = false;     // 首次进入标记

	if((last_voltage != ps305d.system_parameters.set_voltage_data || first_in == false))
	{
		last_voltage = ps305d.system_parameters.set_voltage_data;
//		first_in = true; // 首次进入标记置位
		
		if(ps305d.system_parameters.set_voltage_data < VOLT_GEAR_5V)
		{
			JK_1_LOW;
			JK_2_HIGH;
		}
		else if(ps305d.system_parameters.set_voltage_data < VOLT_GEAR_14V)
		{
			JK_1_HIGH;
			JK_2_HIGH;
		}
		else if(ps305d.system_parameters.set_voltage_data < VOLT_GEAR_21V)
		{
			JK_1_LOW;
			JK_2_LOW;
		}
		else
		{
			JK_1_HIGH;
			JK_2_LOW;
		}		
		// 标记变压器初始化完成（用于输出控制）

		ps305d.init_transformer_control = true;
		
	}	
}