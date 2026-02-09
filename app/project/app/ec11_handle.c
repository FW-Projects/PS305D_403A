#include "ec11_handle.h"
#include "beep_handle.h"
#include "PS305D_handle.h"

static uint8_t ec11_run = 0;
void vol_ec11_event_handle(void);
void cur_ec11_event_handle(void);
void voltage_ec11_get_event(EC11_AnalyzeResult state);
void current_ec11_get_event(EC11_AnalyzeResult state_current);
static vol_ec11_event_e vol_ec11_handle_event = VOL_EC11_END_EVENT;
static cur_ec11_event_e cur_ec11_handle_event = CUR_EC11_END_EVENT;
EC11_AnalyzeResult temp_analyze_result;
EC11_AnalyzeResult air_analyze_result;

void ec11_handle(void)
{
	EC11_ScanResult voltage_scan_result;
	EC11_ScanResult current_scan_result;

	voltage_scan_result = EC11_Scan(&voltage_ec11);
	temp_analyze_result = EC11_Analyze(&voltage_ec11, voltage_scan_result);
	voltage_ec11_get_event(temp_analyze_result);
	vol_ec11_event_handle();

	current_scan_result = EC11_Scan(&current_ec11);
	air_analyze_result = EC11_Analyze(&current_ec11, current_scan_result);
	current_ec11_get_event(air_analyze_result);
	cur_ec11_event_handle();
}

void current_ec11_get_event(EC11_AnalyzeResult state_current)
{
	static bool first_in = false;
	static int set_time2 = SET_SHOW_TIMES;
	switch (state_current)
	{
	case EC11_ANALYZE_CW:
		if (ps305d.lock_gate == UNLOCK)
		{
			switch (ps305d.General_parameters.cur_display_position)
			{
			case 0:
				cur_ec11_handle_event = CURRENT_HUNDRED_ADD;
				break;
			case 1:
				cur_ec11_handle_event = CURRENT_THOUSAND_ADD;
				break;
			case 2:
				cur_ec11_handle_event = CURRENT_HUNDRED_ADD;
				break;
			case 3:
				cur_ec11_handle_event = CURRENT_TEN_ADD;
				break;
			case 4:
				cur_ec11_handle_event = CURRENT_ONE_ADD;
				break;
			default:
				break;
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
			case 0:
				cur_ec11_handle_event = CURRENT_HUNDRED_REDUCE;
				break;
			case 1:
				cur_ec11_handle_event = CURRENT_THOUSAND_REDUCE;
				break;
			case 2:
				cur_ec11_handle_event = CURRENT_HUNDRED_REDUCE;
				break;
			case 3:
				cur_ec11_handle_event = CURRENT_TEN_REDUCE;
				break;
			case 4:
				cur_ec11_handle_event = CURRENT_ONE_REDUCE;
				break;
			default:
				break;
			}
			ps305d.General_parameters.i_pid_update_flag = true;
			ps305d.General_parameters.last_cur_flicker_display_flag = true;
			ps305d.General_parameters.set_cur_flag = true;
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
			cur_ec11_handle_event = CURRENT_POSITION;
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
			if (ps305d.speak_gate == SPEAK_OPEN)
				ps305d.speak_gate = SPEAK_CLOSE;
			else if (ps305d.speak_gate == SPEAK_CLOSE)
				ps305d.speak_gate = SPEAK_OPEN;
		}
		sbeep.cmd = BEEP_LONG;
		break;
	case EC11_ANALYZE_LONG_RELEASE:
		break;
	case EC11_ANALYZE_KEY_CW:
		// 按下并顺时针旋转
		break;
	case EC11_ANALYZE_KEY_CCW:
		// 按下并逆时针旋转
		break;
	default:
		// 无动作或未处理的动作
		break;
	}

	if (ps305d.General_parameters.cur_display_position != 0 && state_current == EC11_ANALYZE_NO_ACTION)
	{
		set_time2--;
		if (set_time2 <= 1300)
		{
			ps305d.General_parameters.last_cur_flicker_display_flag = false;
		}
		if (set_time2 <= 0)
		{
			set_time2 = SET_SHOW_TIMES;
			ps305d.General_parameters.cur_display_position = 0;
			ps305d.General_parameters.set_cur_flag = false;
		}
	}
	else if (ps305d.General_parameters.cur_display_position != 0 && state_current != EC11_ANALYZE_NO_ACTION)
		set_time2 = SET_SHOW_TIMES;
}

void voltage_ec11_get_event(EC11_AnalyzeResult state_voltage)
{
	static bool first_in = false;
	static int set_time1 = SET_SHOW_TIMES;
	switch (state_voltage)
	{
	case EC11_ANALYZE_CW:
		if (ps305d.lock_gate == UNLOCK)
		{
			switch (ps305d.General_parameters.vol_display_position)
			{
			case 0:
				//					vol_ec11_handle_event = VOLTAGE_TEN_ADD;
				vol_ec11_handle_event = VOLTAGE_HUNDRED_ADD;
				break;
			case 1:
				vol_ec11_handle_event = VOLTAGE_THOUSAND_ADD;
				break;
			case 2:
				vol_ec11_handle_event = VOLTAGE_HUNDRED_ADD;
				break;
			case 3:
				vol_ec11_handle_event = VOLTAGE_TEN_ADD;
				break;
			case 4:
				vol_ec11_handle_event = VOLTAGE_ONE_ADD;
				break;
			default:
				break;
			}
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
			case 0:
				//					vol_ec11_handle_event = VOLTAGE_TEN_REDUCE;
				vol_ec11_handle_event = VOLTAGE_HUNDRED_REDUCE;
				break;
			case 1:
				vol_ec11_handle_event = VOLTAGE_THOUSAND_REDUCE;
				break;
			case 2:
				vol_ec11_handle_event = VOLTAGE_HUNDRED_REDUCE;
				break;
			case 3:
				vol_ec11_handle_event = VOLTAGE_TEN_REDUCE;
				break;
			case 4:
				vol_ec11_handle_event = VOLTAGE_ONE_REDUCE;
				break;
			default:
				break;
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
			vol_ec11_handle_event = VOLTAGE_POSITION;
			ps305d.General_parameters.last_vol_flicker_display_flag = false;
			ps305d.General_parameters.set_vol_flag = true;
			sbeep.cmd = BEEP_SHORT;
		}
		break;
	case EC11_ANALYZE_DOUBLE_CLICK:
		break;
	case EC11_ANALYZE_LONG_PRESS:
		if (ps305d.lock_gate == UNLOCK)
		{
			sbeep.cmd = BEEP_LONG;
			ps305d.lock_gate = LOCK;
		}
		else if (ps305d.lock_gate == LOCK)
		{
			sbeep.cmd = BEEP_LONG;
			ps305d.lock_gate = UNLOCK;
		}
		break;
	case EC11_ANALYZE_LONG_RELEASE:
		break;
	case EC11_ANALYZE_KEY_CW:
		// 按下并顺时针旋转
		break;
	case EC11_ANALYZE_KEY_CCW:
		// 按下并逆时针旋转
		break;
	default:

		// 无动作或未处理的动作
		break;
	}

	if (ps305d.General_parameters.vol_display_position != 0 && state_voltage == EC11_ANALYZE_NO_ACTION)
	{
		ps305d.General_parameters.last_vol_flicker_display_flag = false;
		set_time1--;
		if (set_time1 <= 0)
		{
			set_time1 = SET_SHOW_TIMES;
			ps305d.General_parameters.vol_display_position = 0;
			ps305d.General_parameters.set_vol_flag = false;
		}
	}
	else if (ps305d.General_parameters.vol_display_position != 0 && state_voltage != EC11_ANALYZE_NO_ACTION)
		set_time1 = SET_SHOW_TIMES;
}

void vol_ec11_event_handle(void)
{
	switch (vol_ec11_handle_event)
	{
	case VOLTAGE_ONE_ADD:
		if (ps305d.General_parameters.voltage_limit_flag == true)
		{
			if (ps305d.system_parameters.set_voltage_data + 1 > MAX_8400MV_VOLTAGE_DATA)
			{
				ps305d.system_parameters.set_voltage_data = MAX_8400MV_VOLTAGE_DATA;
				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
			}
			else
			{
				ps305d.system_parameters.set_voltage_data++;
			}
		}
		else
		{
			if (ps305d.system_parameters.set_voltage_data + 1 > MAX_VOLTAGE_DATA)
			{
				ps305d.system_parameters.set_voltage_data = MAX_VOLTAGE_DATA;
				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
			}
			else
			{
				ps305d.system_parameters.set_voltage_data++;
			}
		}

		vol_ec11_handle_event = VOL_EC11_END_EVENT;
		break;
	case VOLTAGE_TEN_ADD:
		if (ps305d.General_parameters.voltage_limit_flag == true)
		{
			if (ps305d.system_parameters.set_voltage_data + 10 > MAX_8400MV_VOLTAGE_DATA)
			{
				ps305d.system_parameters.set_voltage_data = MAX_8400MV_VOLTAGE_DATA;
				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
			}
			else
			{
				ps305d.system_parameters.set_voltage_data += 10;
			}
		}
		else
		{
			if (ps305d.system_parameters.set_voltage_data + 10 > MAX_VOLTAGE_DATA)
			{
				ps305d.system_parameters.set_voltage_data = MAX_VOLTAGE_DATA;
				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
			}
			else
			{
				ps305d.system_parameters.set_voltage_data += 10;
			}
		}
		vol_ec11_handle_event = VOL_EC11_END_EVENT;
		break;
	case VOLTAGE_HUNDRED_ADD:
		if (ps305d.General_parameters.voltage_limit_flag == true)
		{
			if (ps305d.system_parameters.set_voltage_data + 100 > MAX_8400MV_VOLTAGE_DATA)
			{
				ps305d.system_parameters.set_voltage_data = MAX_8400MV_VOLTAGE_DATA;
				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
			}
			else
			{
				ps305d.system_parameters.set_voltage_data += 100;
			}
		}
		else
		{
			if (ps305d.system_parameters.set_voltage_data + 100 > MAX_VOLTAGE_DATA)
			{
				ps305d.system_parameters.set_voltage_data = MAX_VOLTAGE_DATA;
				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
			}
			else
			{
				ps305d.system_parameters.set_voltage_data += 100;
			}
		}
		vol_ec11_handle_event = VOL_EC11_END_EVENT;
		break;
	case VOLTAGE_THOUSAND_ADD:
		if (ps305d.General_parameters.voltage_limit_flag == true)
		{
			if (ps305d.system_parameters.set_voltage_data + 1000 > MAX_8400MV_VOLTAGE_DATA)
			{
				ps305d.system_parameters.set_voltage_data = MAX_8400MV_VOLTAGE_DATA;
				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
			}
			else
			{
				ps305d.system_parameters.set_voltage_data += 1000;
			}
		}
		else
		{
			if (ps305d.system_parameters.set_voltage_data + 1000 > MAX_VOLTAGE_DATA)
			{
				ps305d.system_parameters.set_voltage_data = MAX_VOLTAGE_DATA;
				ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
			}
			else
			{
				ps305d.system_parameters.set_voltage_data += 1000;
			}
		}
		vol_ec11_handle_event = VOL_EC11_END_EVENT;
		break;
	case VOLTAGE_ONE_REDUCE:
		if (ps305d.system_parameters.set_voltage_data - 1 <= MIN_VOLTAGE_DATA)
		{
			ps305d.system_parameters.set_voltage_data = MIN_VOLTAGE_DATA;
			ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_voltage_data--;
		}
		vol_ec11_handle_event = VOL_EC11_END_EVENT;
		break;
	case VOLTAGE_TEN_REDUCE:
		if (ps305d.system_parameters.set_voltage_data - 10 <= MIN_VOLTAGE_DATA)
		{
			ps305d.system_parameters.set_voltage_data = MIN_VOLTAGE_DATA;
			ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_voltage_data -= 10;
		}
		vol_ec11_handle_event = VOL_EC11_END_EVENT;
		break;
	case VOLTAGE_HUNDRED_REDUCE:
		if (ps305d.system_parameters.set_voltage_data - 100 <= MIN_VOLTAGE_DATA)
		{
			ps305d.system_parameters.set_voltage_data = MIN_VOLTAGE_DATA;
			ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_voltage_data -= 100;
		}
		vol_ec11_handle_event = VOL_EC11_END_EVENT;
		break;
	case VOLTAGE_THOUSAND_REDUCE:
		if (ps305d.system_parameters.set_voltage_data - 1000 <= MIN_VOLTAGE_DATA)
		{
			ps305d.system_parameters.set_voltage_data = MIN_VOLTAGE_DATA;
			ps305d.system_parameters.last_set_voltage_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_voltage_data -= 1000;
		}
		vol_ec11_handle_event = VOL_EC11_END_EVENT;
		break;
	case VOLTAGE_POSITION:
		ps305d.General_parameters.vol_display_position++;
		ps305d.General_parameters.cur_display_position = 0;
		if (ps305d.General_parameters.vol_display_position > 4)
		{
			ps305d.General_parameters.vol_display_position = 0;
		}
		ps305d.General_parameters.last_vol_flicker_display_flag = false;
		vol_ec11_handle_event = VOL_EC11_END_EVENT;
		break;
	case VOL_EC11_END_EVENT:
		break;
	}
}

void cur_ec11_event_handle(void)
{
	switch (cur_ec11_handle_event)
	{
	case CURRENT_ONE_ADD:
		if (ps305d.system_parameters.set_current_data + 1 > MAX_CURRENT_DATA)
		{
			ps305d.system_parameters.set_current_data = MAX_CURRENT_DATA;
			ps305d.system_parameters.last_set_current_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_current_data++;
		}
		cur_ec11_handle_event = CUR_EC11_END_EVENT;
		break;
	case CURRENT_TEN_ADD:
		if (ps305d.system_parameters.set_current_data + 10 > MAX_CURRENT_DATA)
		{
			ps305d.system_parameters.set_current_data = MAX_CURRENT_DATA;
			ps305d.system_parameters.last_set_current_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_current_data += 10;
		}

		cur_ec11_handle_event = CUR_EC11_END_EVENT;
		break;

	case CURRENT_HUNDRED_ADD:
		if (ps305d.system_parameters.set_current_data + 100 > MAX_CURRENT_DATA)
		{
			ps305d.system_parameters.set_current_data = MAX_CURRENT_DATA;
			ps305d.system_parameters.last_set_current_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_current_data += 100;
		}
		cur_ec11_handle_event = CUR_EC11_END_EVENT;
		break;

	case CURRENT_THOUSAND_ADD:
		if (ps305d.system_parameters.set_current_data + 1000 > MAX_CURRENT_DATA)
		{
			ps305d.system_parameters.set_current_data = MAX_CURRENT_DATA;
			ps305d.system_parameters.last_set_current_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_current_data += 1000;
		}

		cur_ec11_handle_event = CUR_EC11_END_EVENT;
		break;
	case CURRENT_ONE_REDUCE:
		if (ps305d.system_parameters.set_current_data - 1 <= MIN_CURRENT_DATA)
		{
			ps305d.system_parameters.set_current_data = MIN_CURRENT_DATA;
			ps305d.system_parameters.last_set_current_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_current_data--;
		}

		cur_ec11_handle_event = CUR_EC11_END_EVENT;
		break;
	case CURRENT_TEN_REDUCE:
		if (ps305d.system_parameters.set_current_data - 10 <= MIN_CURRENT_DATA)
		{
			ps305d.system_parameters.set_current_data = MIN_CURRENT_DATA;
			ps305d.system_parameters.last_set_current_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_current_data -= 10;
		}
		cur_ec11_handle_event = CUR_EC11_END_EVENT;
		break;

	case CURRENT_HUNDRED_REDUCE:
		if (ps305d.system_parameters.set_current_data - 100 <= MIN_CURRENT_DATA)
		{
			ps305d.system_parameters.set_current_data = MIN_CURRENT_DATA;
			ps305d.system_parameters.last_set_current_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_current_data -= 100;
		}
		cur_ec11_handle_event = CUR_EC11_END_EVENT;
		break;

	case CURRENT_THOUSAND_REDUCE:
		if (ps305d.system_parameters.set_current_data - 1000 <= MIN_CURRENT_DATA)
		{
			ps305d.system_parameters.set_current_data = MIN_CURRENT_DATA;
			ps305d.system_parameters.last_set_current_data = RESET_DATA;
		}
		else
		{
			ps305d.system_parameters.set_current_data -= 1000;
		}
		cur_ec11_handle_event = CUR_EC11_END_EVENT;
		break;
	case CURRENT_POSITION:
		ps305d.General_parameters.vol_display_position = 0;
		ps305d.General_parameters.cur_display_position++;
		if (ps305d.General_parameters.cur_display_position > 4)
		{
			ps305d.General_parameters.cur_display_position = 0;
		}
		ps305d.General_parameters.last_cur_flicker_display_flag = false;
		cur_ec11_handle_event = CUR_EC11_END_EVENT;
		break;
	case CUR_EC11_END_EVENT:
		break;
	}
}