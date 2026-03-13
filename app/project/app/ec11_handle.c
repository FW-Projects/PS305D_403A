#include "ec11_handle.h"
#include "beep_handle.h"
#include "PS305D_handle.h"
#include "lcd_handle.h"

// 显示位置最大值（0~4）
#define DISPLAY_POS_MAX               4
// 调节步长定义
#define ADJUST_STEP_ONE               1
#define ADJUST_STEP_TEN               10
#define ADJUST_STEP_HUNDRED           100
#define ADJUST_STEP_THOUSAND          1000
// 计时阈值
#define SET_SHOW_TIMES_VAL            SET_SHOW_TIMES  // 显示超时计时基准值
#define CUR_SHOW_TIMEOUT_THRESHOLD    1300            // 电流显示超时阈值
// 事件处理结束标记
#define VOL_EC11_EVENT_END            VOL_EC11_END_EVENT
#define CUR_EC11_EVENT_END            CUR_EC11_END_EVENT


void vol_ec11_event_handle(void);
void cur_ec11_event_handle(void);
void voltage_ec11_get_event(EC11_AnalyzeResult state);
void current_ec11_get_event(EC11_AnalyzeResult state_current);

static vol_ec11_event_e g_vol_ec11_event = VOL_EC11_END_EVENT;
static cur_ec11_event_e g_cur_ec11_event = CUR_EC11_END_EVENT;
static bool g_vol_long_press_handled = false;

EC11_AnalyzeResult g_vol_ec11_analyze_result;
EC11_AnalyzeResult g_cur_ec11_analyze_result;




/**
 * @brief 电压值调节通用函数（提取重复的电压加减逻辑）
 * @param step: 调节步长（1/10/100/1000）
 * @param is_add: 是否为增加（true=加，false=减）
 */
static void adjust_voltage(int step, bool is_add)
{
    uint16_t max_volt = (ps305d.General_parameters.voltage_limit_flag == true) 
                        ? MAX_8400MV_VOLTAGE_DATA : MAX_VOLTAGE_DATA;
    uint16_t min_volt = MIN_VOLTAGE_DATA;
    uint32_t new_volt = ps305d.system_parameters.set_voltage_data;

    if (is_add)
    {
        new_volt += step;
        if (new_volt > max_volt)
        {
            new_volt = max_volt;
            ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
        }
    }
    else
    {
        if (new_volt <= min_volt + step) // 防止下溢
        {
            new_volt = min_volt;
            ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
        }
        else
        {
            new_volt -= step;
        }
    }

    ps305d.system_parameters.set_voltage_data = new_volt;
}

/**
 * @brief 电流值调节通用函数（提取重复的电流加减逻辑）
 * @param step: 调节步长（1/10/100/1000）
 * @param is_add: 是否为增加（true=加，false=减）
 */
static void adjust_current(int step, bool is_add)
{
    uint16_t max_cur = MAX_CURRENT_DATA;
    uint16_t min_cur = MIN_CURRENT_DATA;
    uint32_t new_cur = ps305d.system_parameters.set_current_data;

    if (is_add)
    {
        new_cur += step;
        if (new_cur > max_cur)
        {
            new_cur = max_cur;
            ps305d.system_parameters.last_set_current_data = RESET_DATA;
        }
    }
    else
    {
        if (new_cur <= min_cur + step) // 防止下溢
        {
            new_cur = min_cur;
            ps305d.system_parameters.last_set_current_data = RESET_DATA;
        }
        else
        {
            new_cur -= step;
        }
    }

    ps305d.system_parameters.set_current_data = new_cur;
}


/************************** EC11总处理 **************************/
/**
 * @brief EC11编码器总处理函数
 * @note  依次处理电压EC11、电流EC11的扫描→解析→事件处理
 */
void ec11_handle(void)
{
	 // 电压EC11处理：扫描→解析→事件分发

	EC11_ScanResult vol_scan_result = EC11_Scan(&voltage_ec11);
	g_vol_ec11_analyze_result  = EC11_Analyze(&voltage_ec11, vol_scan_result);
	voltage_ec11_get_event(g_vol_ec11_analyze_result);
	vol_ec11_event_handle();

	// 电流EC11处理：扫描→解析→事件分发
	EC11_ScanResult cur_scan_result = EC11_Scan(&current_ec11);
	g_cur_ec11_analyze_result  = EC11_Analyze(&current_ec11, cur_scan_result);
	current_ec11_get_event(g_cur_ec11_analyze_result);
	cur_ec11_event_handle();
}

/************************** 电流EC11事件解析 **************************/
/**
 * @brief 电流EC11事件解析（根据编码器动作设置调节事件）
 * @param state: EC11解析结果（正转/反转/短按/长按等）
 */
void current_ec11_get_event(EC11_AnalyzeResult state)
{
	static int set_time2 = SET_SHOW_TIMES_VAL; // 电流显示超时计时
	
	switch (state)
	{
	case EC11_ANALYZE_CW:
		if (ps305d.lock_gate == UNLOCK)
		{
			// 根据显示位置设置调节步长
			switch (ps305d.General_parameters.cur_display_position)
			{
				case 0:  g_cur_ec11_event = CURRENT_HUNDRED_ADD;   break;
				case 1:  g_cur_ec11_event = CURRENT_THOUSAND_ADD;  break;
				case 2:  g_cur_ec11_event = CURRENT_HUNDRED_ADD;   break;
				case 3:  g_cur_ec11_event = CURRENT_TEN_ADD;       break;
				case 4:  g_cur_ec11_event = CURRENT_ONE_ADD;       break;
				default: break;
			}
			ps305d.General_parameters.i_pid_update_flag = true;
			ps305d.General_parameters.last_cur_flicker_display_flag = true;
			ps305d.General_parameters.set_cur_flag = true;
			sbeep.cmd = BEEP_SHORT;
		}
		break;

	case EC11_ANALYZE_CCW:
		if (ps305d.lock_gate == UNLOCK)
		{
			switch (ps305d.General_parameters.cur_display_position)
			{
				case 0:  g_cur_ec11_event = CURRENT_HUNDRED_REDUCE; break;
				case 1:  g_cur_ec11_event = CURRENT_THOUSAND_REDUCE;break;
				case 2:  g_cur_ec11_event = CURRENT_HUNDRED_REDUCE; break;
				case 3:  g_cur_ec11_event = CURRENT_TEN_REDUCE;     break;
				case 4:  g_cur_ec11_event = CURRENT_ONE_REDUCE;     break;
				default: break;
			}
			ps305d.General_parameters.i_pid_update_flag = true;
			ps305d.General_parameters.last_cur_flicker_display_flag = true;
			ps305d.General_parameters.set_cur_flag = true;
			sbeep.cmd = BEEP_SHORT;
		}
		break;
	case EC11_ANALYZE_SHORT_CLICK:
		if (ps305d.lock_gate == UNLOCK)
		{
			g_cur_ec11_event = CURRENT_POSITION;
			ps305d.General_parameters.last_cur_flicker_display_flag = false;
			ps305d.General_parameters.set_cur_flag = true;
			sbeep.cmd = BEEP_SHORT;
		}
		break;
	case EC11_ANALYZE_DOUBLE_CLICK:
		break;
	case EC11_ANALYZE_LONG_PRESS:
		if (ps305d.lock_gate == LOCK)
		{
			// 切换蜂鸣器状态
			ps305d.speak_gate = (ps305d.speak_gate == SPEAK_OPEN) ? SPEAK_CLOSE : SPEAK_OPEN;
		}
		sbeep.cmd = BEEP_LONG;
		break;
	case EC11_ANALYZE_LONG_RELEASE:break;
	case EC11_ANALYZE_KEY_CW:break;
	case EC11_ANALYZE_KEY_CCW:break;
	case EC11_ANALYZE_FAST_CW:break;
	case EC11_ANALYZE_FAST_CCW:break;
	default:break;
	}

	// 电流显示超时处理
	if (ps305d.General_parameters.cur_display_position != 0)
	{
		if (state == EC11_ANALYZE_NO_ACTION)
		{
			ps305d.General_parameters.last_cur_flicker_display_flag = false;
			set_time2--;
//			if (set_time2 <= CUR_SHOW_TIMEOUT_THRESHOLD)
//			{
//				ps305d.General_parameters.last_cur_flicker_display_flag = false;
//			}
			if (set_time2 <= 0)
			{
				set_time2 = SET_SHOW_TIMES;
				ps305d.General_parameters.cur_display_position = 0;
				ps305d.General_parameters.set_cur_flag = false;
				actual_display_times = 0x00;
				ps305d.system_parameters.last_actual_current_data = 0xffff;
			}
		}
		else 
		{
			set_time2 = SET_SHOW_TIMES;
		}
		
	}
	
}


/************************** 电压EC11事件解析 **************************/
/**
 * @brief 电压EC11事件解析（根据编码器动作设置调节事件）
 * @param state: EC11解析结果（正转/反转/短按/长按等）
 */
void voltage_ec11_get_event(EC11_AnalyzeResult state)
{
	static int set_time1 = SET_SHOW_TIMES_VAL; // 电压显示超时计时

	switch (state)
	{
	case EC11_ANALYZE_CW:
		if (ps305d.lock_gate == UNLOCK)
		{
			switch (ps305d.General_parameters.vol_display_position)
			{
				case 0:  g_vol_ec11_event = VOLTAGE_HUNDRED_ADD;   break;
				case 1:  g_vol_ec11_event = VOLTAGE_THOUSAND_ADD;  break;
				case 2:  g_vol_ec11_event = VOLTAGE_HUNDRED_ADD;   break;
				case 3:  g_vol_ec11_event = VOLTAGE_TEN_ADD;       break;
				case 4:  g_vol_ec11_event = VOLTAGE_ONE_ADD;       break;
				default: break;
			}
			 // 设置PID更新标记+显示标记+蜂鸣
			ps305d.General_parameters.v_pid_update_flag = true;
			ps305d.General_parameters.pid_update_times = 0x00;
			ps305d.General_parameters.last_vol_flicker_display_flag = true;
			ps305d.General_parameters.set_vol_flag = true;
			sbeep.cmd = BEEP_SHORT;
		}
		break;
	case EC11_ANALYZE_CCW:
		if (ps305d.lock_gate == UNLOCK)
		{
			switch (ps305d.General_parameters.vol_display_position)
			{
				case 0:  g_vol_ec11_event = VOLTAGE_HUNDRED_REDUCE; break;
				case 1:  g_vol_ec11_event = VOLTAGE_THOUSAND_REDUCE;break;
				case 2:  g_vol_ec11_event = VOLTAGE_HUNDRED_REDUCE; break;
				case 3:  g_vol_ec11_event = VOLTAGE_TEN_REDUCE;     break;
				case 4:  g_vol_ec11_event = VOLTAGE_ONE_REDUCE;     break;
				default: break;
			}
			ps305d.General_parameters.v_pid_update_flag = true;
			ps305d.General_parameters.pid_update_times = 0x00;
			ps305d.General_parameters.last_vol_flicker_display_flag = true;
			ps305d.General_parameters.set_vol_flag = true;
			sbeep.cmd = BEEP_SHORT;
		}
		break;
	case EC11_ANALYZE_FAST_CW:
		break;
	case EC11_ANALYZE_FAST_CCW:
		break;
	case EC11_ANALYZE_SHORT_CLICK:
		if (ps305d.lock_gate == UNLOCK)
		{
			g_vol_ec11_event  = VOLTAGE_POSITION;
			ps305d.General_parameters.last_vol_flicker_display_flag = false;
			ps305d.General_parameters.set_vol_flag = true;
			sbeep.cmd = BEEP_SHORT;
		}
		break;
	case EC11_ANALYZE_DOUBLE_CLICK:
		break;
	case EC11_ANALYZE_LONG_PRESS:
		if (!g_vol_long_press_handled)
		{
			ps305d.lock_gate = (ps305d.lock_gate == UNLOCK) ? LOCK : UNLOCK;
			sbeep.cmd = BEEP_LONG;
			g_vol_long_press_handled = true;
		}
		break;
	case EC11_ANALYZE_LONG_RELEASE:
		g_vol_long_press_handled = false;
		break;
	case EC11_ANALYZE_KEY_CW:break;
	case EC11_ANALYZE_KEY_CCW:break;
	default:break;
	}

	// 电压显示超时处理
	if (ps305d.General_parameters.vol_display_position != 0)
	{
		if (state == EC11_ANALYZE_NO_ACTION)
		{
			ps305d.General_parameters.last_vol_flicker_display_flag = false;
			set_time1--;
			if (set_time1 <= 0)
			{
				set_time1 = SET_SHOW_TIMES;
				ps305d.General_parameters.vol_display_position = 0;
				ps305d.General_parameters.set_vol_flag = false;
				ps305d.system_parameters.actual_display_times1 = 0x00;
				ps305d.system_parameters.last_actual_voltage_data = 0xffff;
			}
		}
		else 
		{
			set_time1 = SET_SHOW_TIMES;
		}
	}
	
}

/************************** 电压EC11事件处理（调节电压值） **************************/
void vol_ec11_event_handle(void)
{
    switch (g_vol_ec11_event)
    {
        case VOLTAGE_ONE_ADD:      adjust_voltage(ADJUST_STEP_ONE, true);        break;
        case VOLTAGE_TEN_ADD:      adjust_voltage(ADJUST_STEP_TEN, true);        break;
        case VOLTAGE_HUNDRED_ADD:  adjust_voltage(ADJUST_STEP_HUNDRED, true);    break;
        case VOLTAGE_THOUSAND_ADD: adjust_voltage(ADJUST_STEP_THOUSAND, true);   break;
        case VOLTAGE_ONE_REDUCE:   adjust_voltage(ADJUST_STEP_ONE, false);       break;
        case VOLTAGE_TEN_REDUCE:   adjust_voltage(ADJUST_STEP_TEN, false);       break;
        case VOLTAGE_HUNDRED_REDUCE: adjust_voltage(ADJUST_STEP_HUNDRED, false); break;
        case VOLTAGE_THOUSAND_REDUCE:adjust_voltage(ADJUST_STEP_THOUSAND, false);break;

        case VOLTAGE_POSITION: // 切换电压显示位置
            ps305d.General_parameters.vol_display_position++;
            ps305d.General_parameters.cur_display_position = 0;
            // 显示位置超限，复位为0
            if (ps305d.General_parameters.vol_display_position > DISPLAY_POS_MAX)
            {
                ps305d.General_parameters.vol_display_position = 0;
            }
            ps305d.General_parameters.last_vol_flicker_display_flag = false;
            break;

        case VOL_EC11_EVENT_END: // 无事件
        default:
            break;
    }

    // 事件处理完成，复位事件标记
    g_vol_ec11_event = VOL_EC11_EVENT_END;
}

/************************** 电流EC11事件处理（调节电流值） **************************/
void cur_ec11_event_handle(void)
{
    switch (g_cur_ec11_event)
    {
        case CURRENT_ONE_ADD:      adjust_current(ADJUST_STEP_ONE, true);        break;
        case CURRENT_TEN_ADD:      adjust_current(ADJUST_STEP_TEN, true);        break;
        case CURRENT_HUNDRED_ADD:  adjust_current(ADJUST_STEP_HUNDRED, true);    break;
        case CURRENT_THOUSAND_ADD: adjust_current(ADJUST_STEP_THOUSAND, true);   break;
        case CURRENT_ONE_REDUCE:   adjust_current(ADJUST_STEP_ONE, false);       break;
        case CURRENT_TEN_REDUCE:   adjust_current(ADJUST_STEP_TEN, false);       break;
        case CURRENT_HUNDRED_REDUCE: adjust_current(ADJUST_STEP_HUNDRED, false); break;
        case CURRENT_THOUSAND_REDUCE:adjust_current(ADJUST_STEP_THOUSAND, false);break;

        case CURRENT_POSITION: // 切换电流显示位置
            ps305d.General_parameters.vol_display_position = 0;
            ps305d.General_parameters.cur_display_position++;
            // 显示位置超限，复位为0
            if (ps305d.General_parameters.cur_display_position > DISPLAY_POS_MAX)
            {
                ps305d.General_parameters.cur_display_position = 0;
            }
            ps305d.General_parameters.last_cur_flicker_display_flag = false;
            break;

        case CUR_EC11_EVENT_END: // 无事件
        default:
            break;
    }

    // 事件处理完成，复位事件标记
    g_cur_ec11_event = CUR_EC11_EVENT_END;
}





//void vol_ec11_event_handle(void)
//{
//	switch (vol_ec11_handle_event)
//	{
//	case VOLTAGE_ONE_ADD:
//		if (ps305d.General_parameters.voltage_limit_flag == true)
//		{
//			if (ps305d.system_parameters.set_voltage_data + 1 > MAX_8400MV_VOLTAGE_DATA)
//			{
//				ps305d.system_parameters.set_voltage_data = MAX_8400MV_VOLTAGE_DATA;
//				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//			}
//			else
//			{
//				ps305d.system_parameters.set_voltage_data++;
//			}
//		}
//		else
//		{
//			if (ps305d.system_parameters.set_voltage_data + 1 > MAX_VOLTAGE_DATA)
//			{
//				ps305d.system_parameters.set_voltage_data = MAX_VOLTAGE_DATA;
//				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//			}
//			else
//			{
//				ps305d.system_parameters.set_voltage_data++;
//			}
//		}

//		vol_ec11_handle_event = VOL_EC11_END_EVENT;
//		break;
//	case VOLTAGE_TEN_ADD:
//		if (ps305d.General_parameters.voltage_limit_flag == true)
//		{
//			if (ps305d.system_parameters.set_voltage_data + 10 > MAX_8400MV_VOLTAGE_DATA)
//			{
//				ps305d.system_parameters.set_voltage_data = MAX_8400MV_VOLTAGE_DATA;
//				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//			}
//			else
//			{
//				ps305d.system_parameters.set_voltage_data += 10;
//			}
//		}
//		else
//		{
//			if (ps305d.system_parameters.set_voltage_data + 10 > MAX_VOLTAGE_DATA)
//			{
//				ps305d.system_parameters.set_voltage_data = MAX_VOLTAGE_DATA;
//				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//			}
//			else
//			{
//				ps305d.system_parameters.set_voltage_data += 10;
//			}
//		}
//		vol_ec11_handle_event = VOL_EC11_END_EVENT;
//		break;
//	case VOLTAGE_HUNDRED_ADD:
//		if (ps305d.General_parameters.voltage_limit_flag == true)
//		{
//			if (ps305d.system_parameters.set_voltage_data + 100 > MAX_8400MV_VOLTAGE_DATA)
//			{
//				ps305d.system_parameters.set_voltage_data = MAX_8400MV_VOLTAGE_DATA;
//				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//			}
//			else
//			{
//				ps305d.system_parameters.set_voltage_data += 100;
//			}
//		}
//		else
//		{
//			if (ps305d.system_parameters.set_voltage_data + 100 > MAX_VOLTAGE_DATA)
//			{
//				ps305d.system_parameters.set_voltage_data = MAX_VOLTAGE_DATA;
//				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//			}
//			else
//			{
//				ps305d.system_parameters.set_voltage_data += 100;
//			}
//		}
//		vol_ec11_handle_event = VOL_EC11_END_EVENT;
//		break;
//	case VOLTAGE_THOUSAND_ADD:
//		if (ps305d.General_parameters.voltage_limit_flag == true)
//		{
//			if (ps305d.system_parameters.set_voltage_data + 1000 > MAX_8400MV_VOLTAGE_DATA)
//			{
//				ps305d.system_parameters.set_voltage_data = MAX_8400MV_VOLTAGE_DATA;
//				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//			}
//			else
//			{
//				ps305d.system_parameters.set_voltage_data += 1000;
//			}
//		}
//		else
//		{
//			if (ps305d.system_parameters.set_voltage_data + 1000 > MAX_VOLTAGE_DATA)
//			{
//				ps305d.system_parameters.set_voltage_data = MAX_VOLTAGE_DATA;
//				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//			}
//			else
//			{
//				ps305d.system_parameters.set_voltage_data += 1000;
//			}
//		}
//		vol_ec11_handle_event = VOL_EC11_END_EVENT;
//		break;
//	case VOLTAGE_ONE_REDUCE:
//		if (ps305d.system_parameters.set_voltage_data - 1 <= MIN_VOLTAGE_DATA)
//		{
//			ps305d.system_parameters.set_voltage_data = MIN_VOLTAGE_DATA;
//			ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_voltage_data--;
//		}
//		vol_ec11_handle_event = VOL_EC11_END_EVENT;
//		break;
//	case VOLTAGE_TEN_REDUCE:
//		if (ps305d.system_parameters.set_voltage_data - 10 <= MIN_VOLTAGE_DATA)
//		{
//			ps305d.system_parameters.set_voltage_data = MIN_VOLTAGE_DATA;
//			ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_voltage_data -= 10;
//		}
//		vol_ec11_handle_event = VOL_EC11_END_EVENT;
//		break;
//	case VOLTAGE_HUNDRED_REDUCE:
//		if (ps305d.system_parameters.set_voltage_data - 100 <= MIN_VOLTAGE_DATA)
//		{
//			ps305d.system_parameters.set_voltage_data = MIN_VOLTAGE_DATA;
//			ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_voltage_data -= 100;
//		}
//		vol_ec11_handle_event = VOL_EC11_END_EVENT;
//		break;
//	case VOLTAGE_THOUSAND_REDUCE:
//		if (ps305d.system_parameters.set_voltage_data - 1000 <= MIN_VOLTAGE_DATA)
//		{
//			ps305d.system_parameters.set_voltage_data = MIN_VOLTAGE_DATA;
//			ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_voltage_data -= 1000;
//		}
//		vol_ec11_handle_event = VOL_EC11_END_EVENT;
//		break;
//	case VOLTAGE_POSITION:
//		ps305d.General_parameters.vol_display_position++;
//		ps305d.General_parameters.cur_display_position = 0;
//		if (ps305d.General_parameters.vol_display_position > 4)
//		{
//			ps305d.General_parameters.vol_display_position = 0;
//		}
//		ps305d.General_parameters.last_vol_flicker_display_flag = false;
//		vol_ec11_handle_event = VOL_EC11_END_EVENT;
//		break;
//	case VOL_EC11_END_EVENT:
//		break;
//	}
//}

//void cur_ec11_event_handle(void)
//{
//	switch (cur_ec11_handle_event)
//	{
//	case CURRENT_ONE_ADD:
//		if (ps305d.system_parameters.set_current_data + 1 > MAX_CURRENT_DATA)
//		{
//			ps305d.system_parameters.set_current_data = MAX_CURRENT_DATA;
//			ps305d.system_parameters.last_set_current_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_current_data++;
//		}
//		cur_ec11_handle_event = CUR_EC11_END_EVENT;
//		break;
//	case CURRENT_TEN_ADD:
//		if (ps305d.system_parameters.set_current_data + 10 > MAX_CURRENT_DATA)
//		{
//			ps305d.system_parameters.set_current_data = MAX_CURRENT_DATA;
//			ps305d.system_parameters.last_set_current_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_current_data += 10;
//		}

//		cur_ec11_handle_event = CUR_EC11_END_EVENT;
//		break;

//	case CURRENT_HUNDRED_ADD:
//		if (ps305d.system_parameters.set_current_data + 100 > MAX_CURRENT_DATA)
//		{
//			ps305d.system_parameters.set_current_data = MAX_CURRENT_DATA;
//			ps305d.system_parameters.last_set_current_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_current_data += 100;
//		}
//		cur_ec11_handle_event = CUR_EC11_END_EVENT;
//		break;

//	case CURRENT_THOUSAND_ADD:
//		if (ps305d.system_parameters.set_current_data + 1000 > MAX_CURRENT_DATA)
//		{
//			ps305d.system_parameters.set_current_data = MAX_CURRENT_DATA;
//			ps305d.system_parameters.last_set_current_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_current_data += 1000;
//		}

//		cur_ec11_handle_event = CUR_EC11_END_EVENT;
//		break;
//	case CURRENT_ONE_REDUCE:
//		if (ps305d.system_parameters.set_current_data - 1 <= MIN_CURRENT_DATA)
//		{
//			ps305d.system_parameters.set_current_data = MIN_CURRENT_DATA;
//			ps305d.system_parameters.last_set_current_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_current_data--;
//		}

//		cur_ec11_handle_event = CUR_EC11_END_EVENT;
//		break;
//	case CURRENT_TEN_REDUCE:
//		if (ps305d.system_parameters.set_current_data - 10 <= MIN_CURRENT_DATA)
//		{
//			ps305d.system_parameters.set_current_data = MIN_CURRENT_DATA;
//			ps305d.system_parameters.last_set_current_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_current_data -= 10;
//		}
//		cur_ec11_handle_event = CUR_EC11_END_EVENT;
//		break;

//	case CURRENT_HUNDRED_REDUCE:
//		if (ps305d.system_parameters.set_current_data - 100 <= MIN_CURRENT_DATA)
//		{
//			ps305d.system_parameters.set_current_data = MIN_CURRENT_DATA;
//			ps305d.system_parameters.last_set_current_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_current_data -= 100;
//		}
//		cur_ec11_handle_event = CUR_EC11_END_EVENT;
//		break;

//	case CURRENT_THOUSAND_REDUCE:
//		if (ps305d.system_parameters.set_current_data - 1000 <= MIN_CURRENT_DATA)
//		{
//			ps305d.system_parameters.set_current_data = MIN_CURRENT_DATA;
//			ps305d.system_parameters.last_set_current_data = RESET_DATA;
//		}
//		else
//		{
//			ps305d.system_parameters.set_current_data -= 1000;
//		}
//		cur_ec11_handle_event = CUR_EC11_END_EVENT;
//		break;
//	case CURRENT_POSITION:
//		ps305d.General_parameters.vol_display_position = 0;
//		ps305d.General_parameters.cur_display_position++;
//		if (ps305d.General_parameters.cur_display_position > 4)
//		{
//			ps305d.General_parameters.cur_display_position = 0;
//		}
//		ps305d.General_parameters.last_cur_flicker_display_flag = false;
//		cur_ec11_handle_event = CUR_EC11_END_EVENT;
//		break;
//	case CUR_EC11_END_EVENT:
//		break;
//	}
//}