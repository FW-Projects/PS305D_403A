#include "adc_filter.h"

#include <stdio.h>
#include <string.h>

filter_t ADC_VTG;
filter_t ADC_CRT;

uint16_t get_adcval(adc_channel_select_type adc_channel);
uint32_t move_average_filter(filter_t *filter);


uint8_t filter_init(filter_t * filter,adc_channel_select_type adc_channel)
{
	if(filter == NULL)
	{
	    return 1;
	}
	
	uint8_t i = 0;
	
	for(i = 0;i<WINDOW_SIZE; i++)
	{
	    filter->input_data[i] = 0;
	}

	filter->adc_channel = adc_channel;
    filter->cur_count = 0;
	filter->sum = 0;
	
  return 0;
}


uint16_t get_adcval(adc_channel_select_type adc_channel)
{
    adc_ordinary_channel_set(ADC1, adc_channel, 1,
                             ADC_SAMPLETIME_239_5);	 
    adc_ordinary_software_trigger_enable(ADC1, TRUE);						 

    while (!adc_flag_get(ADC1, ADC_CCE_FLAG));								 

    return adc_ordinary_conversion_data_get(ADC1);							 
}

/*
*获取adc平均值
*adc_channel：要获取的通道
*times：总次数
*去掉第一和最后一个数据再取平均值。
*/

uint16_t get_adcval_average(adc_channel_select_type adc_channel, uint8_t times)
{
    uint16_t adc_val[times] ;
    uint32_t adc_valsum = 0;
    uint8_t cut;

    for (cut = 0; cut < times; cut++)							//多次获取adc的值
    {
        adc_val[cut] = get_adcval(adc_channel);
    }

    for (cut = 1; cut < times - 1; cut++)						//去掉第一次和最后一次的值
    {
        adc_valsum += adc_val[cut];
    }

    return adc_valsum / (times - 2);						//返回平均值
}



uint32_t move_average_filter(filter_t *filter)
{
    if (filter->cur_count < WINDOW_SIZE)
    {
        filter->input_data[filter->cur_count] = get_adcval(filter->adc_channel);
        filter->sum += filter->input_data[filter->cur_count];
        filter->cur_count++;
        return filter->sum / filter->cur_count;
    }
    else
    {
        filter->sum -= filter->sum / WINDOW_SIZE;
        filter->sum += get_adcval(filter->adc_channel);
        return filter->sum / WINDOW_SIZE;
    }
}
