#ifndef __BE02_HANDLE_H
#define __BE02_HANDLE_H

#include <stdbool.h>
#include <string.h>
#include "at32f403a_407_wk_config.h"
#include "at32f403a_407_gpio.h"



#define MAX_VOLTAGE_DATA 3000
#define MIN_VOLTAGE_DATA 1

#define MAX_CURRENT_DATA 5000
#define MIN_CURRENT_DATA 1

#define V_DAC_THRESHOLD_VALUE 10
#define I_DAC_THRESHOLD_VALUE 100

#define MAX_500MA_CURRENT_DATA 500

#define MAX_8400MV_VOLTAGE_DATA 840

#define OFFSET_VOLTAGE 1

#define SET_SHOW_TIMES 3000

#define ACTUAL_SHOW_TIMES 200

#define RESET_DATA 9999

#define	RESET_VALUE 255

#define FAN_CH1 250
#define FAN_CH2 500
#define FAN_CH3 750
#define FAN_CH4 999



#define HIGH_ERROR_THRESHOLD 50 //0.5V
#define LOW_ERROR_THRESHOLD 100 //1V

#define CURRENT_COMP_TIMES 150 // 1.5s

#define SoftwareVISION_BAI 1
#define SoftwareVISION_SHI 0
#define SoftwareVISION_GE  4
#define HardwareVISION_BAI 4
#define HardwareVISION_SHI 0
#define HardwareVISION_GE  1
typedef enum
{
	NO_AMMETER = 0,
	HAVE_AMMETER,
} ammeter_state_e;

typedef enum
{
	CHECK_GPIO_STATE = 0,
	CHECK_OUPUT,
} check_ammeter_mode_e;


typedef enum
{
	NO_OCP_MODE = 0,
	ONCE_OCP_MODE,
	CONT_OCP_MODE,
} ocp_mode_e;

typedef enum
{
	NO_OUTPUT = 0,
	OUTPUT,
} output_state_e;

typedef enum
{
	CV = 0,
	CC,
	OCP,

} work_mode_e;

typedef enum
{
	UNLOCK = 0,
	LOCK,
} lock_mode_e;

typedef enum
{
	SPEAK_OPEN = 0,
	SPEAK_CLOSE,
} speak_mode_e;


typedef enum
{
	JUST_USB = 0,
	JUST_TYPEC,
	BOTH_USE,
} Quick_charge_e;

typedef enum
{
	VOLTAGE_NORMAL = 0,    // µçÑ¹Õý³£
    VOLTAGE_HIGH_ERROR,    // ¸ßÑ¹´íÎó
    VOLTAGE_LOW_ERROR      // µÍÑ¹´íÎó
} error_state_e;



typedef struct
{
	uint16_t dac_voltage_data;
	uint16_t last_dac_voltage_data;
	
	uint16_t dac_current_data;
	uint16_t last_dac_current_data;
	
	uint16_t actual_voltage_data;
	uint16_t last_actual_voltage_data;
	
	uint16_t actual_current_data;
	uint16_t last_actual_current_data;
	
	float usb_voltage_data;
	float last_usb_voltage_data;
	
	float usb_current_data;
	float last_usb_current_data;
	
	float typec_voltage_data;
	float last_typec_voltage_data;
	
	float typec_current_data;
	float last_typec_current_data;
	
	uint16_t set_voltage_data;
	uint16_t last_set_voltage_data;
	
	uint16_t set_current_data;
	uint16_t last_set_current_data;
	
	uint16_t mod1_set_voltage_data;
	uint16_t last_mod1_set_voltage_data;
	
	uint16_t mod1_set_current_data;
	uint16_t last_mod1_set_current_data;
	
	uint16_t mod2_set_voltage_data;
	uint16_t last_mod2_set_voltage_data;
	
	uint16_t mod2_set_current_data;
	uint16_t last_mod2_set_current_data;
	
	uint16_t mod3_set_voltage_data;
	uint16_t last_mod3_set_voltage_data;
	
	uint16_t mod3_set_current_data;
	uint16_t last_mod3_set_current_data;
	
	uint16_t mod4_set_voltage_data;
	uint16_t last_mod4_set_voltage_data;
	
	uint16_t mod4_set_current_data;
	uint16_t last_mod4_set_current_data;
	
	uint16_t ntc_value;
	
	int compensation_voltage_data;
	int compensation_current_data;
	
	int actual_display_times1;
	
} System_parameters_t;

typedef struct
{
	bool voltage_limit_flag;
	bool current_limit_flag;
	bool output_state;
	uint8_t vol_display_position;
	uint8_t cur_display_position;
	uint8_t last_vol_display_position;
	uint8_t last_cur_display_position;
	bool vol_flicker_display_flag;
	bool cur_flicker_display_flag;
	bool set_vol_flag;
	bool set_cur_flag;
	bool last_vol_flicker_display_flag;
	bool last_cur_flicker_display_flag;
	
	bool ocp_flicker_display_flag;
	bool ocp_triggered_flag;
	
	bool v_pid_update_flag;
	bool i_pid_update_flag;
	uint16_t pid_update_times;
	
	bool led_error_flicker_display_flag;
	bool last_led_error_flicker_display_flag;
	
	int check_cur_times;
} General_parameters_t;

typedef struct
{
	bool volt_high_error ;
	bool volt_low_error ;
	
} error_flag_e;


typedef struct
{
	System_parameters_t system_parameters;
	General_parameters_t General_parameters;
	error_flag_e error_flag;

	ocp_mode_e ocp_mode;
	ocp_mode_e last_ocp_mode;
	ocp_mode_e mod1_ocp_mode;
	ocp_mode_e mod2_ocp_mode;
	ocp_mode_e mod3_ocp_mode;
	ocp_mode_e mod4_ocp_mode;
	
	lock_mode_e lock_gate;
	speak_mode_e speak_gate;
	output_state_e output_state;
	output_state_e last_output_state;
	work_mode_e work_mode;
	work_mode_e last_work_mode;
	bool init_transformer_control;
	
	ammeter_state_e ammeter_state;
	ammeter_state_e last_ammeter_state;
	
	check_ammeter_mode_e check_ammeter_mode;
	
	Quick_charge_e Quick_charge_mode;
	Quick_charge_e Last_Quick_charge_mode;
	
	error_state_e error_state;
	
	bool check_vision_flag;
	

} PS305D_t;

extern PS305D_t ps305d;

void PS305D_init(PS305D_t *fwps305d);

#endif
