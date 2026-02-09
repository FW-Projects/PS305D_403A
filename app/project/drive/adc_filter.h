#ifndef __ADC_FILTER_H
#define __ADC_FILTER_H

#include "at32f403a_407.h"

#define ADC_USBA_CRT  8
#define ADC_USBA_VTG  9

#define ADC_TYPEC_CRT 14
#define ADC_TYPEC_VTG 15
 
#define ADC_NTC 10

#define WINDOW_SIZE 15

typedef struct
{
    float input_data[WINDOW_SIZE];
    uint16_t sum;
	uint16_t cur_count;
	adc_channel_select_type adc_channel;
}
filter_t;



//extern filter_t ADC_USBA_CRT;
//extern filter_t ADC_TYPEC_VTG;
//extern filter_t ADC_TYPEC_CRT;
//extern filter_t ADC_USBA_VTG;
extern filter_t ADC_VTG;
extern filter_t ADC_CRT;
uint16_t get_adcval(adc_channel_select_type adc_channel);
uint32_t move_average_filter(filter_t *filter);
uint8_t filter_init(filter_t * filter,adc_channel_select_type adc_channel);
uint16_t get_adcval_average(adc_channel_select_type adc_channel, uint8_t times);
#endif