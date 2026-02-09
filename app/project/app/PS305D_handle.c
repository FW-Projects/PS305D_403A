 #include "PS305D_HANDLE.h"

	PS305D_t ps305d;

void PS305D_init(PS305D_t *fwps305d)
{
	fwps305d->General_parameters.vol_display_position = 0;
	fwps305d->General_parameters.cur_display_position = 0;
	fwps305d->General_parameters.last_cur_display_position = 0;
	fwps305d->General_parameters.last_vol_display_position = 0;

	fwps305d->General_parameters.vol_flicker_display_flag = false;
	fwps305d->General_parameters.cur_flicker_display_flag = false;

	fwps305d->General_parameters.set_cur_flag = false;
	fwps305d->General_parameters.set_vol_flag = false;
	fwps305d->General_parameters.ocp_triggered_flag = false;
	fwps305d->General_parameters.v_pid_update_flag = false;
	fwps305d->General_parameters.i_pid_update_flag = false;
	fwps305d->General_parameters.pid_update_times = 0x00;
	fwps305d->General_parameters.check_cur_times = 0x00;

	fwps305d->system_parameters.mod1_set_current_data = 1000;
	fwps305d->system_parameters.mod2_set_current_data = 1500;
	fwps305d->system_parameters.mod3_set_current_data = 2000;
	fwps305d->system_parameters.mod4_set_current_data = 2500;
	fwps305d->system_parameters.mod1_set_voltage_data = 400;
	fwps305d->system_parameters.mod2_set_voltage_data = 500;
	fwps305d->system_parameters.mod3_set_voltage_data = 600;
	fwps305d->system_parameters.mod4_set_voltage_data = 700;
	fwps305d->mod1_ocp_mode = CONT_OCP_MODE;
	fwps305d->mod2_ocp_mode = CONT_OCP_MODE;
	fwps305d->mod3_ocp_mode = CONT_OCP_MODE;
	fwps305d->mod4_ocp_mode = CONT_OCP_MODE;
	fwps305d->system_parameters.set_voltage_data = 500;
	fwps305d->system_parameters.last_set_voltage_data = RESET_DATA;
	fwps305d->system_parameters.set_current_data = 1000;
	fwps305d->system_parameters.last_set_current_data = RESET_DATA;

	fwps305d->system_parameters.actual_current_data = 0x00;
	fwps305d->system_parameters.actual_voltage_data = 0x00;
	fwps305d->system_parameters.last_actual_current_data = RESET_DATA;
	fwps305d->system_parameters.last_actual_voltage_data = RESET_DATA;

	fwps305d->system_parameters.usb_current_data = 0x00;
	fwps305d->system_parameters.usb_voltage_data = 0x00;
	fwps305d->system_parameters.typec_current_data = 0x00;
	fwps305d->system_parameters.typec_voltage_data = 0x00;

	fwps305d->system_parameters.last_usb_current_data = RESET_DATA;
	fwps305d->system_parameters.last_usb_voltage_data = RESET_DATA;
	fwps305d->system_parameters.last_typec_current_data = RESET_DATA;
	fwps305d->system_parameters.last_typec_voltage_data = RESET_DATA;

	fwps305d->system_parameters.ntc_value = 0x00;

	fwps305d->system_parameters.compensation_current_data = 0x00;
	fwps305d->system_parameters.compensation_voltage_data = 0x00;
	
	fwps305d->system_parameters.actual_display_times1 = 0x00;
	fwps305d->lock_gate = UNLOCK;
	fwps305d->speak_gate = SPEAK_OPEN;
	fwps305d->output_state = NO_OUTPUT;
	fwps305d->work_mode = CV;
	fwps305d->last_work_mode = RESET_VALUE;
	fwps305d->ocp_mode = NO_OCP_MODE;
	fwps305d->last_ocp_mode = RESET_VALUE;
	fwps305d->init_transformer_control = false;
	fwps305d->ammeter_state = NO_AMMETER;
	fwps305d->check_ammeter_mode = CHECK_GPIO_STATE;

}