#include "lcd_handle.h"
#include "PS305D_handle.h"

/* TM1680 */

static uint8_t display_run = 0;
int actual_display_times = 0x00;

uint8_t table[] = {CHAR_0, CHAR_1, CHAR_2, CHAR_3, CHAR_4, CHAR_5, CHAR_6, CHAR_7, CHAR_8, CHAR_9, CHAR_r, LCDSEG_G, CHAR_C, CHAR_d, CHAR_E, CHAR_F, CHAR_H, CHAR_n};

void Actual_Typec_Current_display(uint16_t typec_current_data);
void Actual_Typec_Voltage_display(uint16_t typec_voltage_data);
void Actual_Usb_Current_display(uint16_t usb_current_data);
void Actual_Usb_Voltage_display(uint16_t usb_voltage_data);
void Set_Voltage_display(uint16_t set_voltage_data);
void Actual_Voltage_display(uint16_t act_voltage_data);
void Set_Current_display(uint16_t set_current_data);
void Actual_Current_display(uint16_t act_current_data);
void usb_display_handle(float current_dat, float voltage_dat);
void typec_display_handle(float current_data, float voltage_data);
void voltage_display_handle(void);
void current_display_handle(void);
void led_display_handle(void);
void data_display_state(void);
void Quick_charge_display_handle(void);
void Disp_SoftwareVision(void);
void Disp_HardwareVision(void);

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

		if (ps305d.check_vision_flag == true)
		{
			Disp_SoftwareVision();

			Disp_HardwareVision();
		}
		else
		{
			current_display_handle();
			voltage_display_handle();
		}
		display_run++;
		break;
	case 1:
		Quick_charge_display_handle();
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
	if (last_output != ps305d.output_state)
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

	if (ps305d.error_state == VOLTAGE_NORMAL)
	{
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
			//			if (ps305d.General_parameters.ocp_triggered_flag != true)
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
	else
	{

		if (ps305d.General_parameters.led_error_flicker_display_flag == true ||
			ps305d.General_parameters.last_led_error_flicker_display_flag == true)
		{
			ps305d.General_parameters.last_led_error_flicker_display_flag = false;
			TM1680Write4bit(40, 0xff);
			TM1680Write4bit(41, 0xff);
		}
		else if (ps305d.General_parameters.vol_flicker_display_flag == false &&
				 ps305d.General_parameters.last_led_error_flicker_display_flag == false)
		{
			ps305d.General_parameters.last_led_error_flicker_display_flag = true;
			TM1680Write4bit(40, 0x00);
			TM1680Write4bit(41, 0x00);
		}
	}
}

void current_display_handle(void)
{
	static int set_display_times = SET_SHOW_TIMES;
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
				//				if (ps305d.system_parameters.last_actual_current_data != ps305d.system_parameters.actual_current_data)
				//				{
				//					ps305d.system_parameters.last_actual_current_data = ps305d.system_parameters.actual_current_data;
				if (ps305d.output_state == OUTPUT)
					Actual_Current_display(ps305d.system_parameters.actual_current_data);
				else
					Actual_Current_display(0);
				//				}
			}
		}
		else if (ps305d.work_mode == CC)
		{
			actual_display_times = 0;
			//			if (ps305d.system_parameters.last_set_current_data != ps305d.system_parameters.set_current_data)
			//			{
			//				//ps305d.system_parameters.last_set_current_data = ps305d.system_parameters.set_current_data;y
			////				Set_Current_display(ps305d.system_parameters.set_current_data);
			//			}
			Set_Current_display(ps305d.system_parameters.set_current_data);
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
		if (ps305d.work_mode == CV && ps305d.error_state == VOLTAGE_NORMAL)
		{
			ps305d.system_parameters.actual_display_times1 = 0;
			if (ps305d.system_parameters.last_set_voltage_data != ps305d.system_parameters.set_voltage_data)
			{
				//				ps305d.system_parameters.last_set_voltage_data = ps305d.system_parameters.set_voltage_data;
				Set_Voltage_display(ps305d.system_parameters.set_voltage_data);
			}
		}
		else if (ps305d.work_mode == CC || ps305d.error_state != VOLTAGE_NORMAL)
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

void Check_QC_PD(void)
{

	static uint8_t just_usb_number = 0x00;
	static uint8_t just_typec_number = 0x00;
	static uint8_t both_number = 0x00;

	if (ps305d.system_parameters.typec_voltage_data < 300 && ps305d.system_parameters.usb_voltage_data != 0x00)
	{
		/* just usb */
		just_usb_number++;
		just_typec_number = 0x00;
		both_number = 0x00;
		if (just_usb_number > 5)
		{
			ps305d.Quick_charge_mode = JUST_USB;
			just_usb_number = 0x00;
		}
	}
	else if (ps305d.system_parameters.typec_voltage_data != 0x00 && ps305d.system_parameters.usb_voltage_data < 500)
	{
		/* just typec */
		just_typec_number++;
		just_usb_number = 0x00;
		both_number = 0x00;
		if (just_typec_number > 5)
		{
			just_typec_number = 0x00;
			ps305d.Quick_charge_mode = JUST_TYPEC;
		}
	}

	else if (ps305d.system_parameters.typec_voltage_data >= 400 && ps305d.system_parameters.usb_voltage_data >= 400)
	{
		/* usb & typec */
		both_number++;
		just_usb_number = 0x00;
		just_typec_number = 0x00;
		if (both_number > 5)
		{
			both_number = 0x00;
			ps305d.Quick_charge_mode = BOTH_USE;
		}
	}
}

void Quick_charge_display_handle(void)
{
	static char quick_charge_runtime = 0x01;
	static float typec_disp_crt = 0x00, typec_disp_vlt = 0x00;
	static float usb_disp_crt = 0x00, usb_disp_vlt = 0x00;

	quick_charge_runtime--;

	if (quick_charge_runtime <= 0x00)
	{
		quick_charge_runtime = 100;

		//		Check_QC_PD();
		//
		//		if(ps305d.Quick_charge_mode == JUST_USB)
		//		{
		//			typec_disp_crt = 0x00;
		//			typec_disp_vlt = 500;
		//			usb_disp_crt = ps305d.system_parameters.usb_current_data - 1;
		//			usb_disp_vlt = ps305d.system_parameters.usb_voltage_data;
		//
		//		}
		//		else if(ps305d.Quick_charge_mode == JUST_TYPEC)
		//		{
		//			typec_disp_crt = ps305d.system_parameters.typec_current_data;
		//			typec_disp_vlt = ps305d.system_parameters.typec_voltage_data;
		//			usb_disp_crt = 0x00;
		//			usb_disp_vlt = 500;
		//		}
		//		else
		//		{
		//			typec_disp_vlt = ps305d.system_parameters.typec_voltage_data;
		//			usb_disp_vlt = ps305d.system_parameters.usb_voltage_data;
		//
		//			typec_disp_crt = ps305d.system_parameters.typec_current_data;
		////			usb_disp_crt = ps305d.system_parameters.usb_current_data - ps305d.system_parameters.typec_current_data - 1;
		//			usb_disp_crt = ps305d.system_parameters.usb_current_data;
		//		}

		typec_disp_vlt = ps305d.system_parameters.typec_voltage_data;
		usb_disp_vlt = ps305d.system_parameters.usb_voltage_data;
		typec_disp_crt = ps305d.system_parameters.typec_current_data;
		usb_disp_crt = ps305d.system_parameters.usb_current_data;

		if (usb_disp_crt < 0x00)
			usb_disp_crt = 0x00;
		if (usb_disp_vlt < 0x00)
			usb_disp_vlt = 0x00;
		if (typec_disp_crt < 0x00)
			typec_disp_crt = 0x00;
		if (typec_disp_vlt < 0x00)
			typec_disp_vlt = 0x00;

		usb_display_handle(usb_disp_crt, usb_disp_vlt);
		typec_display_handle(typec_disp_crt, typec_disp_vlt);
	}
}

void typec_display_handle(float current_data, float voltage_data)
{
	static float last_current_data = 0xffff, last_voltage_data = 0xffff;

	if (last_current_data != current_data)
	{
		last_current_data = current_data;
		Actual_Typec_Current_display(current_data);
	}

	if (last_voltage_data != voltage_data)
	{
		last_voltage_data = voltage_data;
		Actual_Typec_Voltage_display(voltage_data);
	}
}

void usb_display_handle(float current_dat, float voltage_dat)
{
	static float last_current_dat = 0xffff, last_voltage_dat = 0xffff;

	if (last_current_dat != current_dat)
	{
		last_current_dat = current_dat;
		Actual_Usb_Current_display(current_dat);
	}

	if (last_voltage_dat != voltage_dat)
	{
		last_voltage_dat = voltage_dat;
		Actual_Usb_Voltage_display(voltage_dat);
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

void Disp_SoftwareVision(void)
{
	Disp_one_person(0, 0x00);
	Disp_one_person(2, table[SoftwareVISION_BAI] | 0x02);
	Disp_one_person(4, table[SoftwareVISION_SHI] | 0x02);
	Disp_one_person(6, table[SoftwareVISION_GE]);
}
void Disp_HardwareVision(void)
{
	Disp_one_person(8, 0x00); 
	Disp_one_person(10, table[HardwareVISION_BAI] | 0x02);
	Disp_one_person(12, table[HardwareVISION_SHI] | 0x02);
	Disp_one_person(14, table[HardwareVISION_GE]);
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
	uint8_t bai = 0, shi = 0, ge = 0;

	bai = typec_current_data / 100;
	shi = typec_current_data % 100 / 10;
	ge = typec_current_data % 10;

	Disp_one_person(34, table[bai] | 0x02);
	Disp_one_person(36, table[shi]);
	Disp_one_person(38, table[ge]);
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
	uint8_t bai = 0, shi = 0, ge = 0;

	bai = usb_current_data / 100;
	shi = usb_current_data % 100 / 10;
	ge = usb_current_data % 10;

	Disp_one_person(22, table[bai] | 0x02);
	Disp_one_person(24, table[shi]);
	Disp_one_person(26, table[ge]);
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
