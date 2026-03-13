#include "stdbool.h"
#include "PS305D_handle.h"
#include "adc_filter.h"
#include "round_data.h"
#include "collect_data_handle.h"
#define COLLECT_DATA_TIMES 50

void collect_typec_usb_data(void);
uint16_t collect_voltage(void);
uint16_t collect_current(void);

/* Collect typec & usb voltage and current */

void collect_data_handle(void)
{
	ps305d.system_parameters.actual_voltage_data = collect_voltage();

	ps305d.system_parameters.actual_current_data = collect_current();

	collect_typec_usb_data();
}


uint16_t collect_voltage(void)
{
	float adc_data = 0;
	uint16_t output_data = 0;
	/* 4095 = 3000份 一份1MV */
	adc_data = (move_average_filter(&ADC_VTG) / 1.365);

	// 四舍五入
	output_data = ROUND_TO_INT(adc_data); 
	if (output_data > 3000)
		output_data = 3000;
	if(output_data <= 0)
		output_data = 0;
	return output_data;
}
uint16_t collect_current(void)
{
	float adc_dat = 0;
	uint16_t out_data = 0;

	adc_dat = (move_average_filter(&ADC_CRT) / 0.819) - OFFSET_VOLTAGE;
	
	if(adc_dat <= 0)
		adc_dat = 0;
	if (adc_dat > 5000)
		adc_dat = 5000;
	// 四舍五入
	out_data = ROUND_TO_INT(adc_dat) ;

	
	
	
	return out_data;
}
                                
void collect_typec_usb_data(void)
{
	static int collect_data_times = 0x00;
	static float t_v = 0, t_a = 0, u_a = 0, u_v = 0;

	collect_data_times--;
	if (collect_data_times <= 0x00)
	{
		collect_data_times = COLLECT_DATA_TIMES;

		t_v = get_adcval_average(ADC_TYPEC_VTG, 10);
		ps305d.system_parameters.typec_voltage_data = ROUND_TO_INT(t_v);

		t_a = move_average_filter(&ADC_TYPEC_CRT) - 7;
		ps305d.system_parameters.typec_current_data = ROUND_TO_INT(t_a);

		u_a = move_average_filter(&ADC_USB_CRT) - 1;
		ps305d.system_parameters.usb_current_data = ROUND_TO_INT(u_a);

		u_v = get_adcval_average(ADC_USBA_VTG, 10) * 0.955;
		ps305d.system_parameters.usb_voltage_data = ROUND_TO_INT(u_v);
	}
}
