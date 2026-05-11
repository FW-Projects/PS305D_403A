#include "PS305D_handle.h"
#include "wk_dac.h"
#include "PID_handle.h"
#include "round_data.h"
#include "output_handle.h"
#include "work_handle.h"
// 补偿计时阈值
#define COMPENSATION_TIMER_THRESHOLD 500
// DAC输出最值限制（12位DAC，0~4095）
#define DAC_OUTPUT_MAX_VALUE 4095
#define DAC_VOLTAGE_MIN_VALUE 1 // 电压DAC最小输出值
#define DAC_CURRENT_MIN_VALUE 0 // 电流DAC最小输出值

// 补偿模式枚举
typedef enum
{
	COMP_MODE_CALC = 0,	 // 计算模式（PID补偿）
	COMP_MODE_LOOKUP = 1 // 查表模式（快速调用）
} CompensateMode;

unsigned int g_last_set_voltage_data = 0x00; // 上一次设置的电压值
unsigned int g_last_set_current_data = 0x00; // 上一次设置的电流值

// 补偿计时变量（原分散定义，集中管理）
static uint32_t g_compensation_timer = 0;	 // 补偿模式切换计时
static uint16_t g_volt_pid_update_times = 0; // 电压PID更新计数
static uint16_t g_cur_comp_timer = 0x00;	 // 电流补偿计时（原cur_com_time）

void dac_output_control(uint16_t base_voltage, uint16_t base_current, int com_vol, int com_cur);
int check_compensation(uint16_t set_v, uint16_t act_v);
void dac_buffer_auto_switch(uint8_t dac_channel, uint16_t target_data, uint16_t threshold_value);

/**
 * @brief 主控制循环（支持补偿存储与调用）
 */
void Power_Output_Control_Loop(void)
{
	static CompensateMode g_voltage_comp_mode = COMP_MODE_CALC;
	static CompensateMode g_current_comp_mode = COMP_MODE_CALC;
	static uint16_t g_voltage_compensation = 0;
	static uint16_t g_current_compensation = 0;

#if 0
    // 1. 采集实际输出
    uint16_t actual_voltage = ps305d.system_parameters.actual_voltage_data;
    uint16_t actual_current = ps305d.system_parameters.actual_current_data;
    
    uint16_t dac_voltage_output, dac_current_output;
    
    if (voltage_compensation_mode == 0) {
        // 2. PID补偿计算模式
        dac_voltage_output = Voltage_PID_Compensation(
            ps305d.system_parameters.set_voltage_data, actual_voltage);
    } else {
        // 3. 快速查表调用模式
        voltage_compensation = Fast_Call_Voltage_Compensation(
            ps305d.system_parameters.set_voltage_data);
    }
    
	if (current_compensation_mode == 0) {
        // 2. PID补偿计算模式
        dac_current_output = Current_PID_Compensation(
            ps305d.system_parameters.set_current_data, actual_current);
    } else {
        // 3. 快速查表调用模式
        current_compensation = Fast_Call_Current_Compensation(
            ps305d.system_parameters.set_current_data);
    }
	
    // 4. DAC输出控制
    dac_output_control(ps305d.system_parameters.set_voltage_data,ps305d.system_parameters.set_current_data,voltage_compensation,current_compensation);    
    // 5. 补偿模式切换逻辑（根据系统状态）
    compensation_timer++;
    if (compensation_timer >= 500) {  // 每5秒检查一次
        compensation_timer = 0;
        
        // 如果系统稳定且补偿表完整，切换到查表模式
		// 如果电压差值超过0.03V，则进入计算模式
		// 如果电流差值超过0.01A，则进入计算模式
		if((ps305d.system_parameters.set_voltage_data - ps305d.system_parameters.actual_voltage_data) <= 3 || 
			(ps305d.system_parameters.actual_voltage_data - ps305d.system_parameters.set_voltage_data) >= 3)
		{
			voltage_compensation_mode = 1; // 查表模式
		}
		else{
			voltage_compensation_mode = 0; // 计算模式
		}
		
		if((ps305d.system_parameters.set_current_data - ps305d.system_parameters.actual_current_data) <= 10 || 
			(ps305d.system_parameters.actual_current_data - ps305d.system_parameters.set_current_data) >= 10)
		{
			current_compensation_mode = 1; // 查表模式
		}
		else{
			current_compensation_mode = 0; // 计算模式
		}
    }
#endif

#if 0
	//无任何修正
	ps305d.system_parameters.compensation_voltage_data = 0x00;
	ps305d.system_parameters.compensation_current_data = 0x00;
	dac_output_control(ps305d.system_parameters.set_voltage_data, 
						ps305d.system_parameters.set_current_data,
						ps305d.system_parameters.compensation_voltage_data, 
						ps305d.system_parameters.compensation_current_data);
#endif

#if 1
	// 进行差值查询并补偿输出

	/************************** 电压补偿逻辑 **************************/
	//	if (ps305d.General_parameters.v_pid_update_flag == true && ps305d.General_parameters.ocp_triggered_flag == false)
	//	{
	//		g_volt_pid_update_times++;
	//		if (g_volt_pid_update_times >= 500)
	//		{
	//			ps305d.General_parameters.v_pid_update_flag = false;
	//			g_volt_pid_update_times = 0;
	//			g_last_set_voltage_data  = 0x00;
	//			ps305d.system_parameters.compensation_voltage_data = check_compensation(
	//				ps305d.system_parameters.set_voltage_data,
	//				ps305d.system_parameters.actual_voltage_data);
	//		}
	//	}
	//	else
	//	{
	//		g_volt_pid_update_times  = 0;
	//		ps305d.system_parameters.compensation_voltage_data = 0x00;
	//	}
	ps305d.system_parameters.compensation_voltage_data = 0x00;
	/************************** 电流补偿逻辑 **************************/
	if (ps305d.General_parameters.i_pid_update_flag == true && ps305d.work_mode == CC)
	{
		g_cur_comp_timer++;
		if (g_cur_comp_timer >= CURRENT_COMP_TIMES) //
		{
			g_cur_comp_timer = 0x00;
			ps305d.General_parameters.i_pid_update_flag = false;
			g_last_set_current_data = 0x00;

			// 电流≥30 、电流<3000时计算补偿，否则清零
			if (ps305d.system_parameters.set_current_data >= 30)
			{
				ps305d.system_parameters.compensation_current_data = check_compensation(
					ps305d.system_parameters.set_current_data,
					ps305d.system_parameters.actual_current_data);
			}
			else
			{
				ps305d.system_parameters.compensation_current_data = 0;
			}
		}
	}
	else
	{
		// 未触发更新，补偿值清零
		ps305d.system_parameters.compensation_current_data = 0x00;
	}

	/************************** DAC输出控制 **************************/
	dac_output_control(ps305d.system_parameters.set_voltage_data,
					   ps305d.system_parameters.set_current_data,
					   ps305d.system_parameters.compensation_voltage_data,
					   ps305d.system_parameters.compensation_current_data);
#endif
}

/************************** 补偿值计算 **************************/
int check_compensation(uint16_t set_v, uint16_t act_v)
{
	int com_value = set_v - act_v;
	return com_value;
}

/************************** DAC输出控制 **************************/
/**
 * @brief DAC输出控制（电压/电流）
 * @param base_voltage: 基准电压设置值
 * @param base_current: 基准电流设置值
 * @param com_vol: 电压补偿值
 * @param com_cur: 电流补偿值
 * @note  仅当设置值变化时更新DAC输出，避免重复操作
 */
void dac_output_control(uint16_t base_voltage, uint16_t base_current, int com_vol, int com_cur)
{
	// 静态变量：DAC输出缓存
	static float g_dac_current_output = 0.0f;	// 电流DAC输出计算值
	static float g_dac_voltage_output = 0.0f;	// 电压DAC输出计算值
	static uint16_t g_dac_current_value = 0x00; // 电流DAC最终输出值
	static uint16_t g_dac_voltage_value = 0x00; // 电压DAC最终输出值

	/************************** 电流DAC输出控制 **************************/
	if (g_last_set_current_data != ps305d.system_parameters.set_current_data)
	{
		g_last_set_current_data = ps305d.system_parameters.set_current_data;

		// 自动切换dac输出缓存
		dac_buffer_auto_switch(DAC2_SELECT, base_current, I_DAC_THRESHOLD_VALUE);

		// 计算电流补偿后总值
		int sum_current = base_current + com_cur;

		// 限制最小值
		if (sum_current <= DAC_CURRENT_MIN_VALUE)
			sum_current = DAC_CURRENT_MIN_VALUE;

		// 电流DAC系数计算
		//		g_dac_current_output  = sum_current * 0.819;//5A
		g_dac_current_output = sum_current * 0.6825; // 6A = 4050

		// 四舍五入取整
		g_dac_current_value = ROUND_TO_INT(g_dac_current_output);

		// 限制DAC输出最大值（12位DAC）
		if (g_dac_current_value >= DAC_OUTPUT_MAX_VALUE)
		{
			g_dac_current_value = DAC_OUTPUT_MAX_VALUE;
		}

		// 更新DAC2输出
		dac_2_data_set(DAC2_12BIT_RIGHT, g_dac_current_value);
	}

	/************************** 电压DAC输出控制 **************************/
	if (g_last_set_voltage_data != ps305d.system_parameters.set_voltage_data)
	{
		g_last_set_voltage_data = ps305d.system_parameters.set_voltage_data;

		// 自动切换dac输出缓存
		dac_buffer_auto_switch(DAC1_SELECT, base_voltage, V_DAC_THRESHOLD_VALUE);

		// 计算电压补偿后总值
		int sum_voltage = base_voltage + com_vol;

		if (sum_voltage <= DAC_VOLTAGE_MIN_VALUE)
			sum_voltage = DAC_VOLTAGE_MIN_VALUE;

		// 电压DAC系数计算（纯硬件线性）
		g_dac_voltage_output = sum_voltage * 1.365;

		// 四舍五入取整
		g_dac_voltage_value = ROUND_TO_INT(g_dac_voltage_output);

		// 限制DAC输出最值
		if (g_dac_voltage_value >= DAC_OUTPUT_MAX_VALUE)
		{
			g_dac_voltage_value = DAC_OUTPUT_MAX_VALUE;
		}
		if (g_dac_voltage_value <= DAC_VOLTAGE_MIN_VALUE)
		{
			g_dac_voltage_value = DAC_VOLTAGE_MIN_VALUE;
		}

		// 更新DAC1输出
		dac_1_data_set(DAC1_12BIT_RIGHT, g_dac_voltage_value);
	}
}

/************************** DAC缓冲自动切换 **************************/
/**
 * @brief 根据设置值切换DAC输出缓冲器状态
 * @param dac_channel: DAC通道选择 (DAC1_SELECT 或 DAC2_SELECT)
 * @param target_data: 设置输出值
 * @param threshold_value: 临界值（0.1V/0.1A）
 * @note  目标值 < 临界值 → 关闭缓冲器；否则开启
 */
void dac_buffer_auto_switch(uint8_t dac_channel, uint16_t target_data, uint16_t threshold_value)
{

	uint8_t buffer_state = (target_data < threshold_value) ? FALSE : TRUE;
	dac_output_buffer_enable(dac_channel, buffer_state);
}
