#include "adc.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/*
*获得ADC值
*adc_channel：要获取的通道
*
*/





uint16_t get_adcval(adc_channel_select_type adc_channel)
{
	adc_ordinary_channel_set(ADC1, adc_channel, 1,ADC_SAMPLETIME_239_5);                     	//ADC1,ADC通道,序列号1，采样时间为239.5周期
	adc_ordinary_software_trigger_enable(ADC1, TRUE);						//使能指定的ADC1的软件转换启动功能
	adc_ordinary_channel_set(ADC1, adc_channel, 1, ADC_SAMPLETIME_1_5);
	while (!adc_flag_get(ADC1, ADC_CCE_FLAG));								//等待转换结束

	return adc_ordinary_conversion_data_get(ADC1);							//返回最近一次ADC1普通通道的转换结果
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



#define N 26
uint16_t middleValueFilter(void)
{
    uint16_t value_buf[N];
    uint16_t i, j, k, temp;

    for (i = 0; i < N; ++i)
    {
        value_buf[i] = get_adcval(ADC_CHANNEL_10);
    }

    for (j = 0 ; j < N - 1; ++j)
    {
        for (k = 0; k < N - j - 1; ++k)
        {
            //从小到大排序，冒泡法排序
            if (value_buf[k] > value_buf[k + 1])
            {
                temp = value_buf[k];
                value_buf[k] = value_buf[k + 1];
                value_buf[k + 1] = temp;
            }
        }
    }

    return value_buf[(N - 1) / 2];
}



#define N_1 26
uint16_t value_buf[N_1];
uint16_t sum = 0;
uint16_t curNum = 0;
uint16_t moveAverageFilter(adc_channel_select_type adc_channel)
{
    if (curNum < N_1)
    {
        value_buf[curNum] = get_adcval(adc_channel);
        sum += value_buf[curNum];
        curNum++;
        return sum / curNum;
    }
    else
    {
        sum -= sum / N_1;
        sum += get_adcval(adc_channel);
        return sum / N_1;
    }
}



int32_t AD_BAT = 0;
uint16_t Most_data[4096] = {0}; //放在全局变量防止堆栈段溢出
uint16_t k = 0;			//出现次数最多的数
uint16_t ii = 0;
uint16_t max = 0;

uint16_t modeFilter(void)
{
    for (ii = 0; ii < 50; ii++)
    {
        //AD_BAT = get_adcval_average(ADC_CHANNEL,80);
        AD_BAT = get_adcval(ADC_CHANNEL_10);
        Most_data[AD_BAT]++;              //读到的数据，其数组中对应的值+1
    }

    max = Most_data[0];						//查找数组中的最大数，并且最大数为出现的次数

    for (ii = 0; ii < 4096; ii++)
    {
        if (max < Most_data[ii])
        {
            k = ii;
        }
    }

    memset(Most_data, 0, 4096);
    return k;
}



uint16_t Search(uint16_t a[],uint16_t len)
{
	uint16_t max=0;//保持到目前为此出现次数最多的那个数
	uint16_t count=1;
	uint16_t maxnum=count;//保存最大计数次
	uint16_t maxd;
	uint16_t i;
  for( i=0;i<len-1;i++)
  {
	   
	  max=a[i];
	  if(a[i+1]==max)
		  count++;
	  else
	  	  count=1;
	  if(count>maxnum)
	  {
        maxnum=count;
		maxd=max;
	  }
  }
  return maxd;
}

//uint16_t max_count_filter(void)
//{
//    static uint16_t max_num;
//	static uint16_t adc_value;
//	static uint16_t adc_data[20] = {0};
//	static uint16_t i = 0;
//	for (i = 0; i < 20; i++)
//    {
//		//adc_value = get_adcval(ADC_CHANNEL_10);
//		adc_value = (uint16_t)(get_adcval_average(ADC_CHANNEL,10));
//        adc_data[i] = adc_value;//读到的数据，其数组中对应的值+1
//    }

//	max_num = Search(adc_data,sizeof(adc_data)/sizeof(adc_data[0]));
//	return max_num;
//}



//去掉一个最大值，去掉一个最小值，求平均值

uint16_t average(uint8_t n)
{
	volatile uint16_t adc_buff[10] = {0};
	volatile uint16_t max=adc_buff[0];
	volatile uint16_t min=adc_buff[0];
	volatile uint16_t sum=0;
    volatile uint16_t adc_value;
	uint8_t i,j;
	
	for ( j=0;j<n;j++)
    {
        adc_value = get_adcval(ADC_CHANNEL_1);
        adc_buff[j] = adc_value;              //读到的数据，其数组中对应的值+1
    }
	max=adc_buff[0];
    min=adc_buff[0];
	for (i=0;i<n;i++)
	{
		sum+=adc_buff[i];
        
		if(max<=adc_buff[i])
		{
		   max = adc_buff[i];
		}
		
		if(min>=adc_buff[i])
		{
		   min = adc_buff[i];
		}
	}
 
	return (sum-max-min)/(n-2);
}














