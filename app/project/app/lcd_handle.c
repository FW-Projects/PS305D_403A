#include "lcd_handle.h"
#include "PS305D_handle.h"

/* TM1680 */

static uint8_t display_run = 0;

uint8_t table[] = {CHAR_0, CHAR_1, CHAR_2, CHAR_3, CHAR_4, CHAR_5, CHAR_6, CHAR_7, CHAR_8, CHAR_9, CHAR_r, LCDSEG_G, CHAR_C, CHAR_d, CHAR_E, CHAR_F, CHAR_H, CHAR_n};

void Actual_Typec_Current_display(uint16_t typec_current_data);
void Actual_Typec_Voltage_display(uint16_t typec_voltage_data);
void Actual_Usb_Current_display(uint16_t usb_current_data);
void Actual_Usb_Voltage_display(uint16_t usb_voltage_data);
void Set_Voltage_display(uint16_t set_voltage_data);
void Actual_Voltage_display(uint16_t act_voltage_data);
void Set_Current_display(uint16_t set_current_data);
void Actual_Current_display(uint16_t act_current_data);
void usb_display_handle(void);
void typec_display_handle(void);
void voltage_display_handle(void);
void current_display_handle(void);
void led_display_handle(void);
void data_display_state(void);

uint8_t get_digit(uint16_t num, uint8_t index)
{
	const uint16_t divisors[4] = {1000, 100, 10, 1};

	if (index >= 4)
		return 0;

	return (num / divisors[index]) % 10;
}

void lcd_handle(void)
{

	switch (display_run)
	{
	case 0:
		data_display_state();
		current_display_handle();
		voltage_display_handle();
		display_run++;
		break;
	case 1:
		usb_display_handle();
		typec_display_handle();
		led_display_handle();
		display_run = 0;
		break;
	default:
		break;
	}
}

void data_display_state(void)
{
	static output_state_e last_output = 0;
	if(last_output != ps305d.output_state)
	{
		last_output = ps305d.output_state;
		ps305d.system_parameters.actual_display_times1 = 0x00;
		ps305d.system_parameters.last_actual_current_data = 0x00;
		ps305d.system_parameters.last_actual_voltage_data = 0x00;
	}
}



void led_display_handle(void)
{
	static bool last_cur_limin_flag, last_vol_limin_flag, last_lock_state;
	led_postion1_e led1_postion_state = CV_NOOCP;
	led_postion2_e led2_postion_state = NOTHING;

	if (ps305d.last_work_mode != ps305d.work_mode ||
		ps305d.last_ocp_mode != ps305d.ocp_mode ||
		ps305d.General_parameters.ocp_triggered_flag == true)
	{
		ps305d.last_work_mode = ps305d.work_mode;
		ps305d.last_ocp_mode = ps305d.ocp_mode;
		if (ps305d.work_mode == CV || ps305d.ocp_mode != NO_OCP_MODE)
		{
			if (ps305d.ocp_mode == NO_OCP_MODE)
			{
				led1_postion_state = CV_NOOCP;
			}
			else if (ps305d.ocp_mode == CONT_OCP_MODE)
			{
				led1_postion_state = CV_CONTOCP;
			}
			else if (ps305d.ocp_mode == ONCE_OCP_MODE)
			{
				if (ps305d.General_parameters.ocp_triggered_flag == true)
				{
					if (ps305d.General_parameters.ocp_flicker_display_flag == false)
						TM1680Write4bit(40, CV_ONCEOCP);
					else
						TM1680Write4bit(40, CV_NOOCP);
				}
				else
					led1_postion_state = CV_ONCEOCP;
			}
		}
		else if (ps305d.work_mode == CC)
		{
			led1_postion_state = CC_NOOCP;
		}
		if (ps305d.General_parameters.ocp_triggered_flag != true)
			TM1680Write4bit(40, led1_postion_state);
	}

	if (last_cur_limin_flag != ps305d.General_parameters.current_limit_flag ||
		last_vol_limin_flag != ps305d.General_parameters.voltage_limit_flag ||
		last_lock_state != ps305d.lock_gate)
	{
		last_cur_limin_flag = ps305d.General_parameters.current_limit_flag;
		last_vol_limin_flag = ps305d.General_parameters.voltage_limit_flag;
		last_lock_state = ps305d.lock_gate;
		if (ps305d.lock_gate == LOCK)
		{
			if (ps305d.General_parameters.voltage_limit_flag == true && ps305d.General_parameters.current_limit_flag == true)
				led2_postion_state = VLimin_ALimin_Lock;
			else if (ps305d.General_parameters.voltage_limit_flag == true && ps305d.General_parameters.current_limit_flag == false)
				led2_postion_state = VLimin_Lock;
			else if (ps305d.General_parameters.voltage_limit_flag == false && ps305d.General_parameters.current_limit_flag == true)
				led2_postion_state = ALimin_Lock;
			else if (ps305d.General_parameters.voltage_limit_flag == false && ps305d.General_parameters.current_limit_flag == false)
				led2_postion_state = Lock;
		}
		else
		{
			if (ps305d.General_parameters.voltage_limit_flag == true && ps305d.General_parameters.current_limit_flag == true)
				led2_postion_state = VLimin_ALimin;
			else if (ps305d.General_parameters.voltage_limit_flag == true && ps305d.General_parameters.current_limit_flag == false)
				led2_postion_state = VLimin;
			else if (ps305d.General_parameters.voltage_limit_flag == false && ps305d.General_parameters.current_limit_flag == true)
				led2_postion_state = ALimin;
			else if (ps305d.General_parameters.voltage_limit_flag == false && ps305d.General_parameters.current_limit_flag == false)
				led2_postion_state = NOTHING;
		}
		TM1680Write4bit(41, led2_postion_state);
	}
}

void current_display_handle(void)
{
	static int set_display_times = SET_SHOW_TIMES;
	static int actual_display_times = 0x00;
	if (ps305d.General_parameters.set_cur_flag == true)
	{
		if (ps305d.system_parameters.last_set_current_data != ps305d.system_parameters.set_current_data)
		{
			//			ps305d.system_parameters.last_set_current_data = ps305d.system_parameters.set_current_data;
			Set_Current_display(ps305d.system_parameters.set_current_data);
		}
		actual_display_times = 0;
		set_display_times--;
		if (set_display_times <= 0 && ps305d.General_parameters.cur_display_position == 0)
		{
			set_display_times = SET_SHOW_TIMES;
			ps305d.General_parameters.set_cur_flag = false;
			ps305d.system_parameters.last_set_current_data = 0xffff;
		}
	}
	else
	{
		if (ps305d.work_mode == CV)
		{
			actual_display_times--;
			if (actual_display_times <= 0)
			{
				actual_display_times = ACTUAL_SHOW_TIMES;
				if (ps305d.system_parameters.last_actual_current_data != ps305d.system_parameters.actual_current_data)
				{
					ps305d.system_parameters.last_actual_current_data = ps305d.system_parameters.actual_current_data;
					if (ps305d.output_state == OUTPUT)
						Actual_Current_display(ps305d.system_parameters.actual_current_data);
					else
						Actual_Current_display(0);
				}
			}
		}
		else if (ps305d.work_mode == CC)
		{
			actual_display_times = 0;
			if (ps305d.system_parameters.last_set_current_data != ps305d.system_parameters.set_current_data)
			{
				//ps305d.system_parameters.last_set_current_data = ps305d.system_parameters.set_current_data;y
				Set_Current_display(ps305d.system_parameters.set_current_data);
			}		
		}
	}
}

void voltage_display_handle(void)
{
	static int set_display_times1 = SET_SHOW_TIMES;
	if (ps305d.General_parameters.set_vol_flag == true)
	{
		if (ps305d.system_parameters.last_set_voltage_data != ps305d.system_parameters.set_voltage_data)
		{
			//			ps305d.system_parameters.last_set_voltage_data = ps305d.system_parameters.set_voltage_data;
			Set_Voltage_display(ps305d.system_parameters.set_voltage_data);
		}
		ps305d.system_parameters.actual_display_times1 = 0;
		set_display_times1--;
		if (set_display_times1 <= 0 && ps305d.General_parameters.vol_display_position == 0)
		{
			set_display_times1 = SET_SHOW_TIMES;
			ps305d.General_parameters.set_vol_flag = false;
			ps305d.system_parameters.last_set_voltage_data = 0xffff;
		}
	}
	else
	{
#if 1
		if (ps305d.work_mode == CV)
		{
			ps305d.system_parameters.actual_display_times1 = 0;
			if (ps305d.system_parameters.last_set_voltage_data != ps305d.system_parameters.set_voltage_data)
			{
				//				ps305d.system_parameters.last_set_voltage_data = ps305d.system_parameters.set_voltage_data;
				Set_Voltage_display(ps305d.system_parameters.set_voltage_data);
			}
		}
		else if (ps305d.work_mode == CC)
		{
			ps305d.system_parameters.actual_display_times1--;

			if (ps305d.system_parameters.actual_display_times1 <= 0)
			{
				ps305d.system_parameters.actual_display_times1 = ACTUAL_SHOW_TIMES;
				if (ps305d.system_parameters.last_actual_voltage_data != ps305d.system_parameters.actual_voltage_data)
				{
					ps305d.system_parameters.last_actual_voltage_data = ps305d.system_parameters.actual_voltage_data;
					Actual_Voltage_display(ps305d.system_parameters.actual_voltage_data);
				}
			}
		}
#endif

#if 0
 
		if(ps305d.work_mode == CV)
		{
			actual_display_times1--;
			
			if(actual_display_times1 <= 0)
			{
				actual_display_times1 = ACTUAL_SHOW_TIMES;
				if(ps305d.system_parameters.last_actual_voltage_data != ps305d.system_parameters.actual_voltage_data)
				{
					ps305d.system_parameters.last_actual_voltage_data = ps305d.system_parameters.actual_voltage_data;
					Actual_Voltage_display(ps305d.system_parameters.actual_voltage_data);
				}
			}
		}
#endif
	}
}

void typec_display_handle(void)
{
	if (ps305d.system_parameters.last_typec_current_data != ps305d.system_parameters.typec_current_data)
	{
		ps305d.system_parameters.last_typec_current_data = ps305d.system_parameters.typec_current_data;
		Actual_Typec_Current_display(ps305d.system_parameters.typec_current_data);
	}

	if (ps305d.system_parameters.last_typec_voltage_data != ps305d.system_parameters.typec_voltage_data)
	{
		ps305d.system_parameters.last_typec_voltage_data = ps305d.system_parameters.typec_voltage_data;
		Actual_Typec_Voltage_display(ps305d.system_parameters.typec_voltage_data);
	}
}

void usb_display_handle(void)
{
	if (ps305d.system_parameters.last_usb_current_data != ps305d.system_parameters.usb_current_data)
	{
		ps305d.system_parameters.last_usb_current_data = ps305d.system_parameters.usb_current_data;
		Actual_Usb_Current_display(ps305d.system_parameters.usb_current_data);
	}

	if (ps305d.system_parameters.last_usb_voltage_data != ps305d.system_parameters.usb_voltage_data)
	{
		ps305d.system_parameters.last_usb_voltage_data = ps305d.system_parameters.usb_voltage_data;
		Actual_Usb_Voltage_display(ps305d.system_parameters.usb_voltage_data);
	}
}

void Set_Voltage_display(uint16_t set_voltage_data)
{
	static uint8_t first_in = false;
	uint8_t t, y, temp;
	uint8_t enshow = 0;

	if (ps305d.General_parameters.vol_display_position != 0)
	{
		switch (ps305d.General_parameters.vol_display_position)
		{
		case 1:
			if (ps305d.General_parameters.vol_flicker_display_flag == true ||
				ps305d.General_parameters.last_vol_flicker_display_flag == true)
			{
				for (y = 0; y < 4; y++)
				{
					temp = get_digit(set_voltage_data, y);
					temp = table[temp];
					if (y == 1)
						temp |= 0x02;
					Disp_one_person(y * 2, temp);
				}
			}
			if (ps305d.General_parameters.vol_flicker_display_flag == false &&
				ps305d.General_parameters.last_vol_flicker_display_flag == false)
			{
				temp = 0;

				Disp_one_person(0, temp);
			}
			break;
		case 2:
			if (ps305d.General_parameters.vol_flicker_display_flag == true ||
				ps305d.General_parameters.last_vol_flicker_display_flag == true)
			{
				for (y = 0; y < 4; y++)
				{
					temp = get_digit(set_voltage_data, y);
					temp = table[temp];
					if (y == 1)
						temp |= 0x02;
					Disp_one_person(y * 2, temp);
				}
			}
			else if (ps305d.General_parameters.vol_flicker_display_flag == false &&
					 ps305d.General_parameters.last_vol_flicker_display_flag == false)
			{
				temp = 0;

				Disp_one_person(2, temp | 0x02);
			}
			break;
		case 3:
			if (ps305d.General_parameters.vol_flicker_display_flag == true ||
				ps305d.General_parameters.last_vol_flicker_display_flag == true)
			{
				for (y = 0; y < 4; y++)
				{
					temp = get_digit(set_voltage_data, y);
					temp = table[temp];
					if (y == 1)
						temp |= 0x02;
					Disp_one_person(y * 2, temp);
				}
			}
			else if (ps305d.General_parameters.vol_flicker_display_flag == false &&
					 ps305d.General_parameters.last_vol_flicker_display_flag == false)
			{
				temp = 0;

				Disp_one_person(4, temp);
			}
			break;
		case 4:
			if (ps305d.General_parameters.vol_flicker_display_flag == true ||
				ps305d.General_parameters.last_vol_flicker_display_flag == true)
			{
				for (y = 0; y < 4; y++)
				{
					temp = get_digit(set_voltage_data, y);
					temp = table[temp];
					if (y == 1)
						temp |= 0x02;
					Disp_one_person(y * 2, temp);
				}
			}
			else if (ps305d.General_parameters.vol_flicker_display_flag == false &&
					 ps305d.General_parameters.last_vol_flicker_display_flag == false)
			{
				temp = 0;

				Disp_one_person(6, temp);
			}
			break;
		default:
			break;
		}
	}
	else
	{
		first_in = false;
		for (t = 0; t < 4; t++)
		{
			temp = get_digit(set_voltage_data, t);

			temp = table[temp];
			if (t == 1)
			{
				temp |= 0x02;
			}
			Disp_one_person(t * 2, temp);
		}
	}
}

void Actual_Voltage_display(uint16_t act_voltage_data)
{
	uint8_t t, temp;
	for (t = 0; t < 4; t++)
	{
		temp = get_digit(act_voltage_data, t);
		temp = table[temp];

		if (t == 1)
		{
			temp |= 0x02;
		}
		Disp_one_person(t * 2, temp);
	}
}

void Set_Current_display(uint16_t set_current_data)
{
	static uint8_t first_in = false;
	uint8_t t, temp;
	uint8_t enshow = 0;

	if (ps305d.General_parameters.cur_display_position != 0)
	{
		switch (ps305d.General_parameters.cur_display_position)
		{
		case 1:
			if (ps305d.General_parameters.cur_flicker_display_flag == true ||
				ps305d.General_parameters.last_cur_flicker_display_flag == true)
			{
				for (t = 0; t < 4; t++)
				{
					temp = get_digit(set_current_data, t);
					temp = table[temp];
					if (t == 0)
						temp |= 0x02;
					Disp_one_person((t * 2) + 8, temp);
				}
			}
			else if (ps305d.General_parameters.cur_flicker_display_flag == false &&
					 ps305d.General_parameters.last_cur_flicker_display_flag == false)
			{
				temp = 0;

				Disp_one_person(8, temp | 0x02);
			}
			break;
		case 2:
			if (ps305d.General_parameters.cur_flicker_display_flag == true ||
				ps305d.General_parameters.last_cur_flicker_display_flag == true)
			{
				for (t = 0; t < 4; t++)
				{
					temp = get_digit(set_current_data, t);
					temp = table[temp];
					if (t == 0)
						temp |= 0x02;
					Disp_one_person((t * 2) + 8, temp);
				}
			}
			else if (ps305d.General_parameters.cur_flicker_display_flag == false &&
					 ps305d.General_parameters.last_cur_flicker_display_flag == false)
			{
				temp = 0;

				Disp_one_person(10, temp);
			}
			break;
		case 3:
			if (ps305d.General_parameters.cur_flicker_display_flag == true ||
				ps305d.General_parameters.last_cur_flicker_display_flag == true)
			{
				for (t = 0; t < 4; t++)
				{
					temp = get_digit(set_current_data, t);
					temp = table[temp];
					if (t == 0)
						temp |= 0x02;
					Disp_one_person((t * 2) + 8, temp);
				}
			}
			else if (ps305d.General_parameters.cur_flicker_display_flag == false &&
					 ps305d.General_parameters.last_cur_flicker_display_flag == false)
			{
				temp = 0;

				Disp_one_person(12, temp);
			}
			break;
		case 4:
			if (ps305d.General_parameters.cur_flicker_display_flag == true ||
				ps305d.General_parameters.last_cur_flicker_display_flag == true)
			{
				for (t = 0; t < 4; t++)
				{
					temp = get_digit(set_current_data, t);
					temp = table[temp];
					if (t == 0)
						temp |= 0x02;
					Disp_one_person((t * 2) + 8, temp);
				}
			}
			else if (ps305d.General_parameters.cur_flicker_display_flag == false &&
					 ps305d.General_parameters.last_cur_flicker_display_flag == false)
			{
				temp = 0;

				Disp_one_person(14, temp);
			}
			break;
		default:
			break;
		}
	}
	else
	{
		first_in = false;
		for (t = 0; t < 4; t++)
		{
			temp = get_digit(set_current_data, t);
			temp = table[temp];

			if (t == 0)
				temp |= 0x02;
			Disp_one_person((t * 2) + 8, temp);
		}
	}
}

void Actual_Current_display(uint16_t act_current_data)
{
	uint8_t t, temp;

	for (t = 0; t < 4; t++)
	{
		temp = get_digit(act_current_data, t);
		temp = table[temp];

		if (t == 0)
			temp |= 0x02;
		Disp_one_person((t * 2) + 8, temp);
	}
}

void Actual_Typec_Current_display(uint16_t typec_current_data)
{
	uint8_t t, dat;

	for (t = 0; t < 3; t++)
	{
		dat = get_digit(typec_current_data, t);
		dat = table[dat];

		if (t == 0)
		{
			if (typec_current_data < 100)
				dat |= 0x02;
		}
		else if (t == 1)
		{
			if (typec_current_data >= 100)
				dat |= 0x02;
		}

		Disp_one_person((t * 2) + 34, dat);
	}
}

void Actual_Typec_Voltage_display(uint16_t typec_voltage_data)
{
	uint8_t t, dat;

	for (t = 0; t < 3; t++)
	{
		dat = get_digit(typec_voltage_data, t);
		dat = table[dat];

		if (t == 0)
		{
			if (typec_voltage_data < 100)
				dat |= 0x02;
		}
		else if (t == 1)
		{
			if (typec_voltage_data >= 100)
				dat |= 0x02;
		}

		Disp_one_person((t * 2) + 28, dat);
	}
}

void Actual_Usb_Current_display(uint16_t usb_current_data)
{
	uint8_t t, dat;

	for (t = 0; t < 3; t++)
	{
		dat = get_digit(usb_current_data, t);
		dat = table[dat];

		if (t == 0)
		{
			if (usb_current_data < 100)
				dat |= 0x02;
		}
		else if (t == 1)
		{
			if (usb_current_data >= 100)
				dat |= 0x02;
		}

		Disp_one_person((t * 2) + 22, dat);
	}
}

void Actual_Usb_Voltage_display(uint16_t usb_voltage_data)
{
	uint8_t t, dat;

	for (t = 0; t < 3; t++)
	{
		dat = get_digit(usb_voltage_data, t);
		dat = table[dat];

		if (t == 0)
		{
			if (usb_voltage_data < 100)
				dat |= 0x02;
		}
		else if (t == 1)
		{
			if (usb_voltage_data >= 100)
				dat |= 0x02;
		}

		Disp_one_person((t * 2) + 16, dat);
	}
}
