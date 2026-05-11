#ifndef _OUTPUT_HANDLE_H
#define _OUTPUT_HANDLE_H


#define OUTPUT_TASK_TIME          100

void Power_Output_Control_Loop(void);

extern unsigned int g_last_set_voltage_data;  // 上一次设置的电压值
extern unsigned int g_last_set_current_data;  // 上一次设置的电流值
#endif