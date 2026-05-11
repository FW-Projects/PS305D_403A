#ifndef __ADC_FILTER_H
#define __ADC_FILTER_H

#include "at32f403a_407.h"

#define ADC_USBA_VTG  9

#define ADC_TYPEC_VTG 15
 
#define ADC_NTC 10

// ===================== 13位 高精度配置 =====================
#define OVER_SAMPLE_TIMES     16      // 4倍过采样 = 精准13位
#define OVERSAMPLE_RIGHT_SHIFT   3   
#define WINDOW_SIZE           15
#define MAX_CURRENT_MA       6000
#define LOOKUP_SIZE          21

extern uint16_t adc_dma_buf[OVER_SAMPLE_TIMES];

typedef struct
{
    float input_data[WINDOW_SIZE];
    uint32_t sum;
	uint32_t cur_count;
	uint16_t index;  
	adc_channel_select_type adc_channel;
}
filter_t;



//extern filter_t ADC_USBA_CRT;
//extern filter_t ADC_TYPEC_VTG;
//extern filter_t ADC_TYPEC_CRT;
//extern filter_t ADC_USBA_VTG;
extern filter_t ADC_TYPEC_CRT;
extern filter_t ADC_USB_CRT;
extern filter_t ADC_VTG;
extern filter_t ADC_CRT;
uint16_t get_average(void);
uint16_t get_adcval(adc_channel_select_type adc_channel);
uint32_t move_average_filter(filter_t *filter);
uint32_t moving_average(filter_t *f, uint32_t new_val);
uint8_t filter_init(filter_t * filter,adc_channel_select_type adc_channel);
uint16_t get_adcval_average(adc_channel_select_type adc_channel, uint8_t times);
#endif