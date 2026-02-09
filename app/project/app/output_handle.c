#include "PS305D_handle.h"
#include "wk_dac.h"
#include "PID_handle.h"
#include "round_data.h"

static char vol_com[3] = {0};
static char cur_com[3] = {0};
static uint8_t num = 0;
static uint16_t last_set_voltage_data = 0x00;
static uint16_t last_set_current_data = 0x00;
void dac_output_control(uint16_t base_voltage, uint16_t base_current, int com_vol, int com_cur);
int check_compensation(uint16_t set_v, uint16_t act_v);
void dac_buffer_auto_switch(uint8_t dac_channel, uint16_t target_data, uint16_t threshold_value);
/**
 * @brief 主控制循环（支持补偿存储与调用）
 */
void Power_Output_Control_Loop(void)
{
	static uint32_t compensation_timer = 0;
	static uint8_t voltage_compensation_mode = 0; // 0=计算模式，1=查表模式
	static uint8_t current_compensation_mode = 0; // 0=计算模式，1=查表模式
	static uint16_t voltage_compensation = 0;
	static uint16_t current_compensation = 0;
	static int vol_comp = 0;
	static uint16_t last_set_vol = 0;
	static uint16_t cur_com_time = 0x00;
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
	dac_output_control(ps305d.system_parameters.set_voltage_data,ps305d.system_parameters.set_current_data,voltage_compensation,current_compensation);
	
#endif

#if 1
	// 进行差值查询并补偿输出
	
	
	/* Voltage */
	if (ps305d.General_parameters.v_pid_update_flag == true)
	{
		ps305d.General_parameters.pid_update_times++;
		if (ps305d.General_parameters.pid_update_times >= 500)
		{
			ps305d.General_parameters.v_pid_update_flag = false;
			ps305d.General_parameters.pid_update_times = 0;
			last_set_voltage_data = 0x00;
			ps305d.system_parameters.compensation_voltage_data = check_compensation(ps305d.system_parameters.set_voltage_data, ps305d.system_parameters.actual_voltage_data);
		}
	}
	else
	{
		ps305d.General_parameters.pid_update_times = 0;
		ps305d.system_parameters.compensation_voltage_data = 0x00;
	}
		
	/* Current */
	if (ps305d.General_parameters.i_pid_update_flag == true && ps305d.work_mode == CC)
	{
		cur_com_time++;
		if(cur_com_time >= 500)
		{
			cur_com_time = 0x00;
			ps305d.General_parameters.i_pid_update_flag = false;
			last_set_current_data = 0x00;
			ps305d.system_parameters.compensation_current_data = check_compensation(ps305d.system_parameters.set_current_data, ps305d.system_parameters.actual_current_data);
		}
	}
	else
	{
		ps305d.system_parameters.compensation_current_data = 0x00;
	}
	
	
	dac_output_control(ps305d.system_parameters.set_voltage_data, ps305d.system_parameters.set_current_data,
					   ps305d.system_parameters.compensation_voltage_data, ps305d.system_parameters.compensation_current_data);

#endif
}

int check_compensation(uint16_t set_v, uint16_t act_v)
{
	int com_value = 0;

	com_value = set_v - act_v;

	return com_value;
}

void dac_output_control(uint16_t base_voltage, uint16_t base_current, int com_vol, int com_cur)
{
	static float dac_output_crt_data = 0x00;
	static float dac_output_vtg_data = 0x00;
	static uint16_t dac_output_value = 0x00;
	static uint16_t dac_output_value1 = 0x00;
	
	static int sum_voltage_data = 0;
	static int sum_current_data = 0;
	
	/* current output control */
	if (last_set_current_data != ps305d.system_parameters.set_current_data)
	{
		last_set_current_data = ps305d.system_parameters.set_current_data;
		
		//自动切换dac输出缓存
		dac_buffer_auto_switch(DAC2_SELECT,base_current,I_DAC_THRESHOLD_VALUE);
		
		sum_current_data = base_current + com_cur;
		
		if(sum_current_data <= 0)
			sum_voltage_data = 0;

		dac_output_crt_data = sum_current_data * 0.819;
		
		dac_output_value1 = ROUND_TO_INT(dac_output_crt_data);
		
		if (dac_output_value1 >= 4095)
		{
			dac_output_value1 = 4095;
		}
		if (dac_output_value1 <= 0)
		{
			dac_output_value1 = 0;
		}
		
		dac_2_data_set(DAC2_12BIT_RIGHT, dac_output_value1);
	}

	/* voltage output control */
	if (last_set_voltage_data != ps305d.system_parameters.set_voltage_data)
	{
		last_set_voltage_data = ps305d.system_parameters.set_voltage_data;
		
		//自动切换dac输出缓存
		dac_buffer_auto_switch(DAC1_SELECT,base_voltage,V_DAC_THRESHOLD_VALUE);

		sum_voltage_data = base_voltage + com_vol;
		
		if(sum_voltage_data <= 1)
			sum_voltage_data = 1;
		
		
#if 0
		/* 软线+硬件补偿 */
		if(base_voltage < 100)
		{
			dac_output_vtg_data = sum_voltage_data * 1.365;
		}
		else if(base_voltage <= 500)
			dac_output_vtg_data = (sum_voltage_data * 1.358) + 25;
		else if(base_voltage <= 1000)
			dac_output_vtg_data = (sum_voltage_data * 1.358) + 25;
		else if(base_voltage <= 1500)
			dac_output_vtg_data = (sum_voltage_data * 1.357) + 25;
		else if(base_voltage <= 2000)
			dac_output_vtg_data = (sum_voltage_data * 1.359) + 20;
		else if(base_voltage <= 2500)
			dac_output_vtg_data = (sum_voltage_data * 1.357) + 24.5;
		else
			dac_output_vtg_data = sum_voltage_data * 1.365;
#endif

#if 1
		/* 纯硬件线性 */

		dac_output_vtg_data = sum_voltage_data * 1.365;
//		dac_output_vtg_data = base_voltage * 1.365;
#endif

		//四舍五入
		dac_output_value = ROUND_TO_INT(dac_output_vtg_data);

		if (dac_output_value >= 4095)
		{
			dac_output_value = 4095;
		}
		if (dac_output_value <= 1)
		{
			dac_output_value = 1;
		}
	
		dac_1_data_set(DAC1_12BIT_RIGHT, dac_output_value);
	}
}



/**
 * @brief  根据设置值切换DAC输出缓冲器状态
 * @param  dac_channel: DAC通道选择 (DAC1_SELECT 或 DAC2_SELECT)
 * @param  target_data: 设置输出值 
 * @param  threshold_value: 临界值
 * @retval 无
 * @note   当目标电压 < 0.1V 时关闭缓冲器，否则开启缓冲器
 * @note   当目标电压 < 0.1A 时关闭缓冲器，否则开启缓冲器
 */
void dac_buffer_auto_switch(uint8_t dac_channel, uint16_t target_data, uint16_t threshold_value)
{
    // 1. 电压阈值判断 (0.1V为临界值)
    if (target_data < threshold_value)
    {
        // 关闭DAC输出缓冲器
        dac_output_buffer_enable(dac_channel, FALSE);

    }
    else
    {
        // 开启DAC输出缓冲器
        dac_output_buffer_enable(dac_channel, TRUE);

    }
}
