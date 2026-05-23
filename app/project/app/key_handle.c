#include "stdbool.h"
#include "key_handle.h"
#include "beep_handle.h"
#include "PS305D_handle.h"
#include "output_handle.h"

KEY_EVENT key_event[KEY_NUMBER];
static KEY keys[];
static KEY_VALUE get_mod1();
static KEY_VALUE get_mod2();
static KEY_VALUE get_mod3();
static KEY_VALUE get_mod4();
static KEY_VALUE get_a_limin();
static KEY_VALUE get_v_limin();
static KEY_VALUE get_reach_5v();
static KEY_VALUE get_cc_cv();
static KEY_VALUE get_cont_ocp();
static KEY_VALUE get_once_ocp();
static KEY_VALUE get_output();

static void get_key(void)
{
    static bool set_done = false;
    static uint16_t last_set_vol_value = 0x00;
    static int set_time = 150;

    if (get_mod1() == K_RELEASE &&
        get_mod2() == K_RELEASE &&
        get_mod3() == K_RELEASE &&
        get_mod4() == K_RELEASE &&
        get_a_limin() == K_RELEASE &&
        get_v_limin() == K_RELEASE &&
        get_reach_5v() == K_RELEASE &&
        get_cc_cv() == K_RELEASE &&
        get_cont_ocp() == K_RELEASE &&
        get_once_ocp() == K_RELEASE &&
        get_output() == K_RELEASE)
    {
        set_done = false;
        if (ps305d.General_parameters.set_vol_flag == true || ps305d.General_parameters.set_cur_flag == true)
        {
            set_time--;
            if (set_time <= 0)
            {
                set_time = 150;
                if (ps305d.General_parameters.set_vol_flag == true)
                    ps305d.General_parameters.set_vol_flag = false;
                if (ps305d.General_parameters.set_cur_flag == true)
                    ps305d.General_parameters.set_cur_flag = false;
            }
        }
    }

    /* scan key begin */
    for (uint8_t i = 0; i < KEY_NUMBER; i++)
    {
        key_event[i] = key_event_check(&keys[i], LONG_PRESS_TIME);
    }

    /* scan key end */
    if (set_done == false && ps305d.lock_gate == UNLOCK)
    {
        /* get mod */
        if (key_event[0] == KE_PRESS)
        {
            ps305d.system_parameters.set_voltage_data = ps305d.system_parameters.mod1_set_voltage_data;
            ps305d.system_parameters.set_current_data = ps305d.system_parameters.mod1_set_current_data;
            ps305d.ocp_mode = CONT_OCP_MODE;
            ps305d.General_parameters.set_cur_flag = true;
            ps305d.General_parameters.set_vol_flag = true;
            ps305d.General_parameters.v_pid_update_flag = true;
			ps305d.General_parameters.i_pid_update_flag = true;
            ps305d.General_parameters.pid_update_times = 0x00;
            sbeep.cmd = BEEP_SHORT;
            set_done = TRUE;
        }
        else if (key_event[1] == KE_PRESS)
        {
            ps305d.system_parameters.set_voltage_data = ps305d.system_parameters.mod2_set_voltage_data;
            ps305d.system_parameters.set_current_data = ps305d.system_parameters.mod2_set_current_data;
            ps305d.ocp_mode = CONT_OCP_MODE;
            ps305d.General_parameters.set_cur_flag = true;
            ps305d.General_parameters.set_vol_flag = true;
            ps305d.General_parameters.v_pid_update_flag = true;
			ps305d.General_parameters.i_pid_update_flag = true;
            ps305d.General_parameters.pid_update_times = 0x00;
            sbeep.cmd = BEEP_SHORT;
            set_done = TRUE;
        }
        else if (key_event[2] == KE_PRESS)
        {
            ps305d.system_parameters.set_voltage_data = ps305d.system_parameters.mod3_set_voltage_data;
            ps305d.system_parameters.set_current_data = ps305d.system_parameters.mod3_set_current_data;
            ps305d.ocp_mode = CONT_OCP_MODE;
            ps305d.General_parameters.set_cur_flag = true;
            ps305d.General_parameters.set_vol_flag = true;
            ps305d.General_parameters.v_pid_update_flag = true;
			ps305d.General_parameters.i_pid_update_flag = true;
            ps305d.General_parameters.pid_update_times = 0x00;
            sbeep.cmd = BEEP_SHORT;
            set_done = TRUE;
        }
        else if (key_event[3] == KE_PRESS)
        {
            ps305d.system_parameters.set_voltage_data = ps305d.system_parameters.mod4_set_voltage_data;
            ps305d.system_parameters.set_current_data = ps305d.system_parameters.mod4_set_current_data;
            ps305d.ocp_mode = CONT_OCP_MODE;
            ps305d.General_parameters.set_cur_flag = true;
            ps305d.General_parameters.set_vol_flag = true;
            ps305d.General_parameters.v_pid_update_flag = true;
			ps305d.General_parameters.i_pid_update_flag = true;
            ps305d.General_parameters.pid_update_times = 0x00;
            sbeep.cmd = BEEP_SHORT;
            set_done = TRUE;
        }

        /* save mod */
        else if (key_event[0] == KE_LONG_PRESS)
        {
            ps305d.system_parameters.mod1_set_voltage_data = ps305d.system_parameters.set_voltage_data;
            ps305d.system_parameters.mod1_set_current_data = ps305d.system_parameters.set_current_data;
            ps305d.mod1_ocp_mode = ps305d.ocp_mode;
            sbeep.cmd = BEEP_LONG;
            set_done = TRUE;
        }
        else if (key_event[1] == KE_LONG_PRESS)
        {
            ps305d.system_parameters.mod2_set_voltage_data = ps305d.system_parameters.set_voltage_data;
            ps305d.system_parameters.mod2_set_current_data = ps305d.system_parameters.set_current_data;
            ps305d.mod2_ocp_mode = ps305d.ocp_mode;

            sbeep.cmd = BEEP_LONG;
            set_done = TRUE;
        }
        else if (key_event[2] == KE_LONG_PRESS)
        {
            ps305d.system_parameters.mod3_set_voltage_data = ps305d.system_parameters.set_voltage_data;
            ps305d.system_parameters.mod3_set_current_data = ps305d.system_parameters.set_current_data;
            ps305d.mod3_ocp_mode = ps305d.ocp_mode;
            sbeep.cmd = BEEP_LONG;
            set_done = TRUE;
        }
        else if (key_event[3] == KE_LONG_PRESS)
        {
            ps305d.system_parameters.mod4_set_voltage_data = ps305d.system_parameters.set_voltage_data;
            ps305d.system_parameters.mod4_set_current_data = ps305d.system_parameters.set_current_data;
            ps305d.mod4_ocp_mode = ps305d.ocp_mode;
            sbeep.cmd = BEEP_LONG;
            set_done = TRUE;
        }

        /* other key KE_PRESS */
        else if (key_event[4] == KE_PRESS)
        {
            /* 切换电流表量程 */

            if (ps305d.General_parameters.current_limit_flag == false)
            {
                /* 电表500MA量程 */
                ps305d.General_parameters.current_limit_flag = true;

                gpio_bits_set(GPIOB, GPIO_PINS_9);
            }
            else if (ps305d.General_parameters.current_limit_flag == true)
            {
                /* 电表5A量程 */
                ps305d.General_parameters.current_limit_flag = false;

                gpio_bits_reset(GPIOB, GPIO_PINS_9);
            }
            sbeep.cmd = BEEP_SHORT;
            set_done = TRUE;
        }
        else if (key_event[5] == KE_PRESS)
        {
            if (ps305d.General_parameters.voltage_limit_flag == false)
            {
                ps305d.General_parameters.voltage_limit_flag = true;
                last_set_vol_value = ps305d.system_parameters.set_voltage_data;
                if (ps305d.system_parameters.set_voltage_data > 840)
                {
                    ps305d.system_parameters.set_voltage_data = 840;
                }
            }
            else if (ps305d.General_parameters.voltage_limit_flag == true)
            {
                ps305d.General_parameters.voltage_limit_flag = false;
                ps305d.system_parameters.set_voltage_data = last_set_vol_value;
            }

            ps305d.General_parameters.set_vol_flag = true;
            ps305d.General_parameters.v_pid_update_flag = true;
            ps305d.General_parameters.pid_update_times = 0x00;
            sbeep.cmd = BEEP_SHORT;
            set_done = TRUE;
        }
        else if (key_event[6] == KE_PRESS)
        {
            ps305d.system_parameters.set_voltage_data = 500;
            ps305d.General_parameters.set_vol_flag = true;
            ps305d.General_parameters.v_pid_update_flag = true;
            ps305d.General_parameters.pid_update_times = 0x00;
            sbeep.cmd = BEEP_SHORT;
            set_done = TRUE;
        }
        else if (key_event[7] == KE_PRESS)
        {
            if (ps305d.ocp_mode != NO_OCP_MODE)
            {
				if (ps305d.General_parameters.ocp_triggered_flag == true)
                {
                    ps305d.General_parameters.ocp_triggered_flag = false;
					ps305d.work_mode = CV;
                }
                ps305d.ocp_mode = NO_OCP_MODE;
            }
            sbeep.cmd = BEEP_SHORT;
            set_done = TRUE;
        }
        else if (key_event[8] == KE_PRESS)
        {
            if (ps305d.ocp_mode == NO_OCP_MODE)
            {
                ps305d.ocp_mode = CONT_OCP_MODE;
            }
            else if (ps305d.ocp_mode == CONT_OCP_MODE)
            {
                if (ps305d.General_parameters.ocp_triggered_flag == true)
                {
                    ps305d.General_parameters.ocp_triggered_flag = false;
					ps305d.work_mode = CV;
                }
                ps305d.ocp_mode = NO_OCP_MODE;
            }
            else if (ps305d.ocp_mode == ONCE_OCP_MODE)
            {
                if (ps305d.General_parameters.ocp_triggered_flag == true)
                {
                    ps305d.General_parameters.ocp_triggered_flag = false;
					ps305d.work_mode = CV;
                }
                ps305d.ocp_mode = CONT_OCP_MODE;
            }
            sbeep.cmd = BEEP_SHORT;
            set_done = TRUE;
        }
        else if (key_event[9] == KE_PRESS)
        {
            if (ps305d.ocp_mode == NO_OCP_MODE)
            {
                ps305d.ocp_mode = ONCE_OCP_MODE;
            }
            else if (ps305d.ocp_mode == CONT_OCP_MODE)
            {
                if (ps305d.General_parameters.ocp_triggered_flag == true)
                {
                    ps305d.General_parameters.ocp_triggered_flag = false;
					ps305d.work_mode = CV;
                }
                ps305d.ocp_mode = ONCE_OCP_MODE;
            }
            else if (ps305d.ocp_mode == ONCE_OCP_MODE)
            {
                if (ps305d.General_parameters.ocp_triggered_flag == true)
                {
                    ps305d.General_parameters.ocp_triggered_flag = false;
                    ps305d.ocp_mode = ONCE_OCP_MODE;
					ps305d.work_mode = CV;
                }
                else
				{
                    ps305d.ocp_mode = NO_OCP_MODE;
				}
            }

            sbeep.cmd = BEEP_SHORT;
            set_done = TRUE;
        }
        else if (key_event[10] == KE_PRESS)
        {
            if (ps305d.output_state == OUTPUT)
                ps305d.output_state = NO_OUTPUT;
            else if (ps305d.output_state == NO_OUTPUT)
            {
                if (ps305d.ocp_mode == ONCE_OCP_MODE)
                {
                    if (ps305d.General_parameters.ocp_triggered_flag == false)
                    {
                        ps305d.output_state = OUTPUT;
                        ps305d.General_parameters.v_pid_update_flag = true;
						ps305d.General_parameters.i_pid_update_flag = true;
                        ps305d.General_parameters.pid_update_times = 0x00;
                    }
                    else
                        ps305d.output_state = NO_OUTPUT;
                }
                else
                {
                    ps305d.output_state = OUTPUT;
                    ps305d.General_parameters.v_pid_update_flag = true;
					ps305d.General_parameters.i_pid_update_flag = true;
                    ps305d.General_parameters.pid_update_times = 0x00;
                }
				g_last_set_voltage_data = 0xffff;
            }
			ps305d.system_parameters.actual_display_times1 = 0x00;
            sbeep.cmd = BEEP_SHORT;
            set_done = TRUE;
        }
		
		if (key_event[10] == KE_LONG_PRESS)
		{
			sbeep.cmd = BEEP_LONG;
			ps305d.system_parameters.set_voltage_data      = 450;
			ps305d.system_parameters.set_current_data      = 5000;
			ps305d.system_parameters.mod1_set_voltage_data = 450;
			ps305d.system_parameters.mod1_set_current_data = 5000;
			ps305d.mod1_ocp_mode                           = CONT_OCP_MODE;
			ps305d.system_parameters.mod2_set_voltage_data = 450;
			ps305d.system_parameters.mod2_set_current_data = 5000;
			ps305d.mod2_ocp_mode                           = CONT_OCP_MODE;
			ps305d.system_parameters.mod3_set_voltage_data = 450;
			ps305d.system_parameters.mod3_set_current_data = 5000;
			ps305d.mod3_ocp_mode                           = CONT_OCP_MODE;
			ps305d.system_parameters.mod4_set_voltage_data = 450;
			ps305d.system_parameters.mod4_set_current_data = 5000;
			ps305d.mod4_ocp_mode                           = CONT_OCP_MODE;
			ps305d.ocp_mode                                = NO_OCP_MODE;
			ps305d.General_parameters.voltage_limit_flag   = false;
			ps305d.General_parameters.current_limit_flag   = false;
			ps305d.lock_gate                               = UNLOCK;
			ps305d.speak_gate                              = SPEAK_OPEN;
			ps305d.output_state = NO_OUTPUT;
			
		}
    }
}

void key_handle(void)
{
    get_key();
}

static KEY_VALUE get_mod1()
{
    if (READ_MOD1 == 0)
    {
        return K_PRESS;
    }
    else
    {
        return K_RELEASE;
    }
}

static KEY_VALUE get_mod2()
{
    if (READ_MOD2 == 0)
    {
        return K_PRESS;
    }
    else
    {
        return K_RELEASE;
    }
}

static KEY_VALUE get_mod3()
{
    if (READ_MOD3 == 0)
    {
        return K_PRESS;
    }
    else
    {
        return K_RELEASE;
    }
}

static KEY_VALUE get_mod4()
{
    if (READ_MOD4 == 0)
    {
        return K_PRESS;
    }
    else
    {
        return K_RELEASE;
    }
}

static KEY_VALUE get_a_limin()
{
    if (READ_A_LIMIN == 0)
    {
        return K_PRESS;
    }
    else
    {
        return K_RELEASE;
    }
}

static KEY_VALUE get_v_limin()
{
    if (READ_V_LIMIN == 0)
    {
        return K_PRESS;
    }
    else
    {
        return K_RELEASE;
    }
}

static KEY_VALUE get_reach_5v()
{
    if (READ_REACH_5V == 0)
    {
        return K_PRESS;
    }
    else
    {
        return K_RELEASE;
    }
}

static KEY_VALUE get_cc_cv()
{
    if (READ_CC_CV == 0)
    {
        return K_PRESS;
    }
    else
    {
        return K_RELEASE;
    }
}

static KEY_VALUE get_cont_ocp()
{
    if (READ_CONT_OCP == 0)
    {
        return K_PRESS;
    }
    else
    {
        return K_RELEASE;
    }
}

static KEY_VALUE get_once_ocp()
{
    if (READ_ONCE_OCP == 0)
    {
        return K_PRESS;
    }
    else
    {
        return K_RELEASE;
    }
}

static KEY_VALUE get_output()
{
    if (READ_OUTPUT == 0)
    {
        return K_PRESS;
    }
    else
    {
        return K_RELEASE;
    }
}

static KEY keys[] =
    {
        {KS_RELEASE, 0, KEY_CYCLE_TIME, get_mod1},
        {KS_RELEASE, 0, KEY_CYCLE_TIME, get_mod2},
        {KS_RELEASE, 0, KEY_CYCLE_TIME, get_mod3},
        {KS_RELEASE, 0, KEY_CYCLE_TIME, get_mod4},
        {KS_RELEASE, 0, KEY_CYCLE_TIME, get_a_limin},
        {KS_RELEASE, 0, KEY_CYCLE_TIME, get_v_limin},
        {KS_RELEASE, 0, KEY_CYCLE_TIME, get_reach_5v},
        {KS_RELEASE, 0, KEY_CYCLE_TIME, get_cc_cv},
        {KS_RELEASE, 0, KEY_CYCLE_TIME, get_cont_ocp},
        {KS_RELEASE, 0, KEY_CYCLE_TIME, get_once_ocp},
        {KS_RELEASE, 0, KEY_CYCLE_TIME, get_output},
};

KEY_EVENT key_event[KEY_NUMBER] =
    {
        KE_PRESS,
        KE_RELEASE,
        KE_LONG_PRESS,
        KE_LONG_RELEASE,
        KE_NONE,
};
