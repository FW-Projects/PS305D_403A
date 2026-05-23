#include <stdbool.h>
#include "flash_handle.h"
 #include "PS305D_HANDLE.h"

static void get_data_from_b(void);
static void get_data_from_a(void);
static void get_reset_data(void);
static uint16_t data_check_len(uint32_t address, uint8_t len);
static uint16_t data_check(uint32_t address);
static void check_data_all(void);

void flash_handle(void)
{
#if 1
    static flash_handle_t sflash;
    static bool first_start_flag = FALSE;
    /* last handle data */
    static float last_v_setdata = 0;
    static float last_i_setdata = 0;                                                                                                                                                                                                                                                                                                                       
	static float last_mod1_v_setdata = 0;
	static float last_mod1_i_setdata = 0;
	static ocp_mode_e last_mod1_ocp_mode = NO_OCP_MODE;
	static float last_mod2_v_setdata = 0;
	static float last_mod2_i_setdata = 0;
	static ocp_mode_e last_mod2_ocp_mode = NO_OCP_MODE;
	static float last_mod3_v_setdata = 0;
	static float last_mod3_i_setdata = 0;
	static ocp_mode_e last_mod3_ocp_mode = NO_OCP_MODE;
	static float last_mod4_v_setdata = 0;
	static float last_mod4_i_setdata = 0;
	static ocp_mode_e last_mod4_ocp_mode = NO_OCP_MODE;
	static ocp_mode_e last_ocp_mode = NO_OCP_MODE;
	static bool last_v_limin_state = false;
	static bool last_i_limin_state = false;
	static lock_mode_e last_lock_state = UNLOCK;
	static speak_mode_e last_speak_state = SPEAK_OPEN;
    static uint16_t flash_version = 0;
    static uint8_t flash_count = 0;
    static uint16_t a_ver;
    static uint16_t b_ver;

    switch (sflash.state)
    {
    case FLASH_START:
        if (FALSE == first_start_flag)
        {
			#if 1
            /* check area b and area b data  */
            if (data_check_len(A_VOLTAGE_SET_DATA_ADDRESS, FLASH_MENBER) != 0xFFFF
                    && data_check_len(B_VOLTAGE_SET_DATA_ADDRESS, FLASH_MENBER) != 0xFFFF)
            {
                /* compare a and b */
                a_ver = flash_read_halfword(A_FLASH_VERSION_ADDRESS);
                b_ver = flash_read_halfword(B_FLASH_VERSION_ADDRESS);

                /* get data */
                if (a_ver > b_ver)
                {
                    get_data_from_a();
                }
                else
                {
                    get_data_from_b();
                }
            }
            /* check area a data  */
            else if (data_check_len(A_VOLTAGE_SET_DATA_ADDRESS, FLASH_MENBER) != 0xFFFF)
            {
                get_data_from_a();
            }
            /* check area b data  */
            else if (data_check_len(B_VOLTAGE_SET_DATA_ADDRESS, FLASH_MENBER) != 0xFFFF)
            {
                get_data_from_b();
            }
            else
            {
                get_reset_data();
            }

            check_data_all();
			
			last_v_setdata      = ps305d.system_parameters.set_voltage_data;
			last_i_setdata      = ps305d.system_parameters.set_current_data;
			last_mod1_v_setdata = ps305d.system_parameters.mod1_set_voltage_data;
			last_mod1_i_setdata = ps305d.system_parameters.mod1_set_current_data;
			last_mod1_ocp_mode  = ps305d.mod1_ocp_mode;
			last_mod2_v_setdata = ps305d.system_parameters.mod2_set_voltage_data;
			last_mod2_i_setdata = ps305d.system_parameters.mod2_set_current_data;
			last_mod2_ocp_mode  = ps305d.mod2_ocp_mode;
			last_mod3_v_setdata = ps305d.system_parameters.mod3_set_voltage_data;
			last_mod3_i_setdata = ps305d.system_parameters.mod3_set_current_data;
			last_mod3_ocp_mode  = ps305d.mod3_ocp_mode;
			last_mod4_v_setdata = ps305d.system_parameters.mod4_set_voltage_data;
			last_mod4_i_setdata = ps305d.system_parameters.mod4_set_current_data;
			last_mod4_ocp_mode  = ps305d.mod4_ocp_mode;
			last_ocp_mode       = ps305d.ocp_mode;
			last_v_limin_state  = ps305d.General_parameters.voltage_limit_flag;
			last_i_limin_state  = ps305d.General_parameters.current_limit_flag;
			last_lock_state     = ps305d.lock_gate;
			last_speak_state    = ps305d.speak_gate;
			
#endif
            
            first_start_flag  = TRUE;
            /* system run */
        }
        else
        {
            sflash.state = FLASH_HANDLE_DATA;
            break;
        }

        break;

    case FLASH_HANDLE_DATA:
		if (last_v_setdata      != ps305d.system_parameters.set_voltage_data      ||   \
			last_i_setdata      != ps305d.system_parameters.set_current_data     ||   \
			last_mod1_v_setdata != ps305d.system_parameters.mod1_set_voltage_data ||   \
			last_mod1_i_setdata != ps305d.system_parameters.mod1_set_current_data ||   \
			last_mod1_ocp_mode  != ps305d.mod1_ocp_mode                           ||   \
			last_mod2_v_setdata != ps305d.system_parameters.mod2_set_voltage_data ||   \
			last_mod2_i_setdata != ps305d.system_parameters.mod2_set_current_data ||   \
			last_mod2_ocp_mode  != ps305d.mod2_ocp_mode                           ||   \
			last_mod3_v_setdata != ps305d.system_parameters.mod3_set_voltage_data ||   \
			last_mod3_i_setdata != ps305d.system_parameters.mod3_set_current_data ||   \
			last_mod3_ocp_mode  != ps305d.mod3_ocp_mode                           ||   \
			last_mod4_v_setdata != ps305d.system_parameters.mod4_set_voltage_data ||   \
			last_mod4_i_setdata != ps305d.system_parameters.mod4_set_current_data ||   \
			last_mod4_ocp_mode  != ps305d.mod4_ocp_mode                           ||   \
			last_ocp_mode       != ps305d.ocp_mode                                ||   \
			last_v_limin_state  != ps305d.General_parameters.voltage_limit_flag   ||   \
			last_i_limin_state  != ps305d.General_parameters.current_limit_flag   ||   \
			last_lock_state     != ps305d.lock_gate                               ||   \
			last_speak_state    != ps305d.speak_gate)                                                                   
        {
            flash_unlock();

            if (flash_count % 2 != FALSE)
            {
				flash_sector_erase(A_VOLTAGE_SET_DATA_ADDRESS);
			
				flash_halfword_program(A_VOLTAGE_SET_DATA_ADDRESS,ps305d.system_parameters.set_voltage_data);
				flash_halfword_program(A_CURRENT_SET_DATA_ADDRESS,ps305d.system_parameters.set_current_data);
				flash_halfword_program(A_MOD1_VOLTAGE_SET_DATA_ADDRESS,ps305d.system_parameters.mod1_set_voltage_data);
				flash_halfword_program(A_MOD1_CURRENT_SET_DATA_ADDRESS,ps305d.system_parameters.mod1_set_current_data);
				flash_halfword_program(A_MOD1_OCP_MODE_ADDRESS,ps305d.mod1_ocp_mode);
				flash_halfword_program(A_MOD2_VOLTAGE_SET_DATA_ADDRESS,ps305d.system_parameters.mod2_set_voltage_data);
				flash_halfword_program(A_MOD2_CURRENT_SET_DATA_ADDRESS,ps305d.system_parameters.mod2_set_current_data);
				flash_halfword_program(A_MOD2_OCP_MODE_ADDRESS,ps305d.mod2_ocp_mode);
				flash_halfword_program(A_MOD3_VOLTAGE_SET_DATA_ADDRESS,ps305d.system_parameters.mod3_set_voltage_data);
			    flash_halfword_program(A_MOD3_CURRENT_SET_DATA_ADDRESS,ps305d.system_parameters.mod3_set_current_data);
				flash_halfword_program(A_MOD3_OCP_MODE_ADDRESS,ps305d.mod3_ocp_mode);
				flash_halfword_program(A_MOD4_VOLTAGE_SET_DATA_ADDRESS,ps305d.system_parameters.mod4_set_voltage_data);
				flash_halfword_program(A_MOD4_CURRENT_SET_DATA_ADDRESS,ps305d.system_parameters.mod4_set_current_data);
				flash_halfword_program(A_MOD4_OCP_MODE_ADDRESS,ps305d.mod4_ocp_mode);
            }
            else
            {
				flash_sector_erase(B_VOLTAGE_SET_DATA_ADDRESS);
			
				flash_halfword_program(B_VOLTAGE_SET_DATA_ADDRESS,ps305d.system_parameters.set_voltage_data);
				flash_halfword_program(B_CURRENT_SET_DATA_ADDRESS,ps305d.system_parameters.set_current_data);
				flash_halfword_program(B_MOD1_VOLTAGE_SET_DATA_ADDRESS,ps305d.system_parameters.mod1_set_voltage_data);
				flash_halfword_program(B_MOD1_CURRENT_SET_DATA_ADDRESS,ps305d.system_parameters.mod1_set_current_data);
				flash_halfword_program(B_MOD1_OCP_MODE_ADDRESS,ps305d.mod1_ocp_mode);
				flash_halfword_program(B_MOD2_VOLTAGE_SET_DATA_ADDRESS,ps305d.system_parameters.mod2_set_voltage_data);
				flash_halfword_program(B_MOD2_CURRENT_SET_DATA_ADDRESS,ps305d.system_parameters.mod2_set_current_data);
				flash_halfword_program(B_MOD2_OCP_MODE_ADDRESS,ps305d.mod2_ocp_mode);
				flash_halfword_program(B_MOD3_VOLTAGE_SET_DATA_ADDRESS,ps305d.system_parameters.mod3_set_voltage_data);
			    flash_halfword_program(B_MOD3_CURRENT_SET_DATA_ADDRESS,ps305d.system_parameters.mod3_set_current_data);
				flash_halfword_program(B_MOD3_OCP_MODE_ADDRESS,ps305d.mod3_ocp_mode);
				flash_halfword_program(B_MOD4_VOLTAGE_SET_DATA_ADDRESS,ps305d.system_parameters.mod4_set_voltage_data);
				flash_halfword_program(B_MOD4_CURRENT_SET_DATA_ADDRESS,ps305d.system_parameters.mod4_set_current_data);
				flash_halfword_program(B_MOD4_OCP_MODE_ADDRESS,ps305d.mod4_ocp_mode);
            }

			last_v_setdata      = ps305d.system_parameters.set_voltage_data;
			last_i_setdata      = ps305d.system_parameters.set_current_data;
			last_mod1_v_setdata = ps305d.system_parameters.mod1_set_voltage_data;
			last_mod1_i_setdata = ps305d.system_parameters.mod1_set_current_data;
			last_mod1_ocp_mode  = ps305d.mod1_ocp_mode;
			last_mod2_v_setdata = ps305d.system_parameters.mod2_set_voltage_data;
			last_mod2_i_setdata = ps305d.system_parameters.mod2_set_current_data;
			last_mod2_ocp_mode  = ps305d.mod2_ocp_mode;
			last_mod3_v_setdata = ps305d.system_parameters.mod3_set_voltage_data;
			last_mod3_i_setdata = ps305d.system_parameters.mod3_set_current_data;
			last_mod3_ocp_mode  = ps305d.mod3_ocp_mode;
			last_mod4_v_setdata = ps305d.system_parameters.mod4_set_voltage_data;
			last_mod4_i_setdata = ps305d.system_parameters.mod4_set_current_data;
			last_mod4_ocp_mode  = ps305d.mod4_ocp_mode;
			
            sflash.state ++;
            break;
        }

        break;

    case FLASH_GENERAL_DATA:
        if (flash_count % 2 != FALSE)
        {
			flash_halfword_program(A_OCP_MODE_ADDRESS,ps305d.ocp_mode);
			flash_halfword_program(A_VOLTAGE_LIMIN_STATE_ADDRESS,ps305d.General_parameters.voltage_limit_flag);
			flash_halfword_program(A_CURRENT_LIMIN_STATE_ADDRESS,ps305d.General_parameters.current_limit_flag);
			flash_halfword_program(A_LOCK_STATE_ADDRESS,ps305d.lock_gate);
			flash_halfword_program(A_SPEAK_STATE_ADDRESS,ps305d.speak_gate);
        }
        else
        {
			flash_halfword_program(B_OCP_MODE_ADDRESS,ps305d.ocp_mode);
			flash_halfword_program(B_VOLTAGE_LIMIN_STATE_ADDRESS,ps305d.General_parameters.voltage_limit_flag);
			flash_halfword_program(B_CURRENT_LIMIN_STATE_ADDRESS,ps305d.General_parameters.current_limit_flag);
			flash_halfword_program(B_LOCK_STATE_ADDRESS,ps305d.lock_gate);
			flash_halfword_program(B_SPEAK_STATE_ADDRESS,ps305d.speak_gate);
        }

		last_ocp_mode       = ps305d.ocp_mode;
		last_v_limin_state  = ps305d.General_parameters.voltage_limit_flag;
		last_i_limin_state  = ps305d.General_parameters.current_limit_flag;
		last_lock_state     = ps305d.lock_gate;
		last_speak_state    = ps305d.speak_gate;
        sflash.state++;
        break;

    case FLASH_FINSH:

        /* flash version */
        if (flash_count % 2 != FALSE)
        {
            flash_halfword_program(A_FLASH_VERSION_ADDRESS, flash_version);
        }
        else
        {
            flash_halfword_program(B_FLASH_VERSION_ADDRESS, flash_version);
        }

        flash_version++;
        flash_count++;
        flash_lock();
        sflash.state = FLASH_HANDLE_DATA;
        break;
    }

#endif
}

static void get_data_from_a(void)
{
	ps305d.system_parameters.set_voltage_data      = flash_read_halfword(A_VOLTAGE_SET_DATA_ADDRESS);
	ps305d.system_parameters.set_current_data      = flash_read_halfword(A_CURRENT_SET_DATA_ADDRESS);
	ps305d.system_parameters.mod1_set_voltage_data = flash_read_halfword(A_MOD1_VOLTAGE_SET_DATA_ADDRESS);
	ps305d.system_parameters.mod1_set_current_data = flash_read_halfword(A_MOD1_CURRENT_SET_DATA_ADDRESS);
	ps305d.mod1_ocp_mode                           = flash_read_halfword(A_MOD1_OCP_MODE_ADDRESS);
	ps305d.system_parameters.mod2_set_voltage_data = flash_read_halfword(A_MOD2_VOLTAGE_SET_DATA_ADDRESS);
	ps305d.system_parameters.mod2_set_current_data = flash_read_halfword(A_MOD2_CURRENT_SET_DATA_ADDRESS);
	ps305d.mod2_ocp_mode                           = flash_read_halfword(A_MOD2_OCP_MODE_ADDRESS);
	ps305d.system_parameters.mod3_set_voltage_data = flash_read_halfword(A_MOD3_VOLTAGE_SET_DATA_ADDRESS);
	ps305d.system_parameters.mod3_set_current_data = flash_read_halfword(A_MOD3_CURRENT_SET_DATA_ADDRESS);
	ps305d.mod3_ocp_mode                           = flash_read_halfword(A_MOD3_OCP_MODE_ADDRESS);
	ps305d.system_parameters.mod4_set_voltage_data = flash_read_halfword(A_MOD4_VOLTAGE_SET_DATA_ADDRESS);
	ps305d.system_parameters.mod4_set_current_data = flash_read_halfword(A_MOD4_CURRENT_SET_DATA_ADDRESS);
	ps305d.mod4_ocp_mode                           = flash_read_halfword(A_MOD4_OCP_MODE_ADDRESS);
	ps305d.ocp_mode                                = flash_read_halfword(A_OCP_MODE_ADDRESS);
	ps305d.General_parameters.voltage_limit_flag   = flash_read_halfword(A_VOLTAGE_LIMIN_STATE_ADDRESS);
	ps305d.General_parameters.current_limit_flag   = flash_read_halfword(A_CURRENT_LIMIN_STATE_ADDRESS);
	ps305d.lock_gate                               = flash_read_halfword(A_LOCK_STATE_ADDRESS);
	ps305d.speak_gate                              = flash_read_halfword(A_SPEAK_STATE_ADDRESS);
}


static void get_data_from_b(void)
{
    ps305d.system_parameters.set_voltage_data      = flash_read_halfword(B_VOLTAGE_SET_DATA_ADDRESS);
	ps305d.system_parameters.set_current_data      = flash_read_halfword(B_CURRENT_SET_DATA_ADDRESS);
	ps305d.system_parameters.mod1_set_voltage_data = flash_read_halfword(B_MOD1_VOLTAGE_SET_DATA_ADDRESS);
	ps305d.system_parameters.mod1_set_current_data = flash_read_halfword(B_MOD1_CURRENT_SET_DATA_ADDRESS);
	ps305d.mod1_ocp_mode                           = flash_read_halfword(B_MOD1_OCP_MODE_ADDRESS);
	ps305d.system_parameters.mod2_set_voltage_data = flash_read_halfword(B_MOD2_VOLTAGE_SET_DATA_ADDRESS);
	ps305d.system_parameters.mod2_set_current_data = flash_read_halfword(B_MOD2_CURRENT_SET_DATA_ADDRESS);
	ps305d.mod2_ocp_mode                           = flash_read_halfword(B_MOD2_OCP_MODE_ADDRESS);
	ps305d.system_parameters.mod3_set_voltage_data = flash_read_halfword(B_MOD3_VOLTAGE_SET_DATA_ADDRESS);
	ps305d.system_parameters.mod3_set_current_data = flash_read_halfword(B_MOD3_CURRENT_SET_DATA_ADDRESS);
	ps305d.mod3_ocp_mode                           = flash_read_halfword(B_MOD3_OCP_MODE_ADDRESS);
	ps305d.system_parameters.mod4_set_voltage_data = flash_read_halfword(B_MOD4_VOLTAGE_SET_DATA_ADDRESS);
	ps305d.system_parameters.mod4_set_current_data = flash_read_halfword(B_MOD4_CURRENT_SET_DATA_ADDRESS);
	ps305d.mod4_ocp_mode                           = flash_read_halfword(B_MOD4_OCP_MODE_ADDRESS);
	ps305d.ocp_mode                                = flash_read_halfword(B_OCP_MODE_ADDRESS);
	ps305d.General_parameters.voltage_limit_flag   = flash_read_halfword(B_VOLTAGE_LIMIN_STATE_ADDRESS);
	ps305d.General_parameters.current_limit_flag   = flash_read_halfword(B_CURRENT_LIMIN_STATE_ADDRESS);
	ps305d.lock_gate                               = flash_read_halfword(B_LOCK_STATE_ADDRESS);
	ps305d.speak_gate                              = flash_read_halfword(B_SPEAK_STATE_ADDRESS);
}


static void get_reset_data(void)
{
	ps305d.system_parameters.set_voltage_data      = 500;
    ps305d.system_parameters.set_current_data      = 1000;
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
}

static uint16_t data_check_len(uint32_t address, uint8_t len)
{
    volatile uint16_t data;
    uint8_t i;

    for (i = 0; i < len; i += DATA_SIZE)
    {
        data = flash_read_halfword(address + i);

        if (data == 0xFFFF)
        {
            break;
        }
    }

    return data;
}

static void check_data_all(void)
{
    /* check s1 handle data */
    if (ps305d.system_parameters.set_voltage_data > MAX_VOLTAGE_DATA || ps305d.system_parameters.set_voltage_data < MIN_VOLTAGE_DATA)
    {
        ps305d.system_parameters.set_voltage_data = 500;
    }

    if (ps305d.system_parameters.set_current_data > MAX_CURRENT_DATA || ps305d.system_parameters.set_current_data < MIN_CURRENT_DATA)
    {
        ps305d.system_parameters.set_current_data = 1000;
    }
	
	if (ps305d.system_parameters.mod1_set_voltage_data > MAX_VOLTAGE_DATA || ps305d.system_parameters.mod1_set_voltage_data < MIN_VOLTAGE_DATA)
    {
        ps305d.system_parameters.mod1_set_voltage_data = 450;
    }

    if (ps305d.system_parameters.mod1_set_current_data > MAX_CURRENT_DATA || ps305d.system_parameters.mod1_set_current_data < MIN_CURRENT_DATA)
    {
        ps305d.system_parameters.mod1_set_current_data = 5000;
    }
	
	if(ps305d.mod1_ocp_mode != NO_OCP_MODE && ps305d.mod1_ocp_mode != CONT_OCP_MODE && ps305d.mod1_ocp_mode != ONCE_OCP_MODE)
	{
		ps305d.mod1_ocp_mode = CONT_OCP_MODE;
	}
	
	if (ps305d.system_parameters.mod2_set_voltage_data > MAX_VOLTAGE_DATA || ps305d.system_parameters.mod2_set_voltage_data < MIN_VOLTAGE_DATA)
    {
        ps305d.system_parameters.mod2_set_voltage_data = 450;
    }

    if (ps305d.system_parameters.mod2_set_current_data > MAX_CURRENT_DATA || ps305d.system_parameters.mod2_set_current_data < MIN_CURRENT_DATA)
    {
        ps305d.system_parameters.mod2_set_current_data = 5000;
    }
	
	if(ps305d.mod2_ocp_mode != NO_OCP_MODE && ps305d.mod2_ocp_mode != CONT_OCP_MODE && ps305d.mod2_ocp_mode != ONCE_OCP_MODE)
	{
		ps305d.mod2_ocp_mode = CONT_OCP_MODE;
	}
	
	if (ps305d.system_parameters.mod3_set_voltage_data > MAX_VOLTAGE_DATA || ps305d.system_parameters.mod3_set_voltage_data < MIN_VOLTAGE_DATA)
    {
        ps305d.system_parameters.mod3_set_voltage_data = 450;
    }

    if (ps305d.system_parameters.mod3_set_current_data > MAX_CURRENT_DATA || ps305d.system_parameters.mod3_set_current_data < MIN_CURRENT_DATA)
    {
        ps305d.system_parameters.mod3_set_current_data = 5000;
    }
	
	if(ps305d.mod3_ocp_mode != NO_OCP_MODE && ps305d.mod3_ocp_mode != CONT_OCP_MODE && ps305d.mod3_ocp_mode != ONCE_OCP_MODE)
	{
		ps305d.mod3_ocp_mode = CONT_OCP_MODE;
	}
	
	if (ps305d.system_parameters.mod4_set_voltage_data > MAX_VOLTAGE_DATA || ps305d.system_parameters.mod4_set_voltage_data < MIN_VOLTAGE_DATA)
    {
        ps305d.system_parameters.mod4_set_voltage_data = 450;
    }

    if (ps305d.system_parameters.mod4_set_current_data > MAX_CURRENT_DATA || ps305d.system_parameters.mod4_set_current_data < MIN_CURRENT_DATA)
    {
        ps305d.system_parameters.mod4_set_current_data = 5000;
    }
	
	if(ps305d.mod4_ocp_mode != NO_OCP_MODE && ps305d.mod4_ocp_mode != CONT_OCP_MODE && ps305d.mod4_ocp_mode != ONCE_OCP_MODE)
	{
		ps305d.mod4_ocp_mode = CONT_OCP_MODE;
	}
	
	if(ps305d.ocp_mode != NO_OCP_MODE && ps305d.ocp_mode != CONT_OCP_MODE && ps305d.ocp_mode != ONCE_OCP_MODE)
	{
		ps305d.ocp_mode = NO_OCP_MODE;
	}
	
	if(ps305d.General_parameters.voltage_limit_flag != false && ps305d.General_parameters.voltage_limit_flag != true)
	{
		ps305d.General_parameters.voltage_limit_flag = false;
	}
	
	if(ps305d.General_parameters.current_limit_flag != false && ps305d.General_parameters.current_limit_flag != true)
	{
		ps305d.General_parameters.current_limit_flag = false;
	}
	
	if(ps305d.lock_gate != UNLOCK && ps305d.lock_gate != LOCK)
	{
		ps305d.lock_gate = UNLOCK;
	}
	
	if(ps305d.speak_gate != SPEAK_CLOSE && ps305d.speak_gate != SPEAK_OPEN)
	{
		ps305d.speak_gate = SPEAK_OPEN;		
	}
	
}


