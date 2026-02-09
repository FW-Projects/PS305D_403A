#ifndef _PID_HANDLE_H
#define _PID_HANDLE_H

#include "PS305D_handle.h"


#define PID_TASK_TIME 5


#define VOLTAGE_TABLE_SIZE 3000  // 0-30V按0.01V精度存储
#define CURRENT_TABLE_SIZE 5000   // 0-5A按0.001A精度存储

#define EEPROM_VOLTAGE_START_ADDR  (0x08000000 + 1024 * 500) // 电压表占用6000字节
#define EEPROM_CURRENT_START_ADDR  (0x08000000 + 1024 * 505) // 电流表占用10000字节
//#define EEPROM_METADATA_ADDR       0x6000  // 元数据存储地址

typedef struct {
    unsigned long voltage_table_valid;  // 电压表有效性标志
    unsigned long current_table_valid;  // 电流表有效性标志
    unsigned long last_update_time;     // 最后更新时间
    unsigned long record_count;         // 记录数量
    unsigned char reserved[16];          // 预留空间
} EEPROM_Metadata;

typedef struct {
    float Kp;          // 比例系数（核心补偿参数）
    float Ki;          // 积分系数（消除静态误差）
    float Kd;          // 微分系数（抑制超调）
    float setpoint;    // 目标电压（如5V）
    float feedback;    // 实际输出电压（如4.95V）
    float error;       // 偏差值（setpoint - feedback）
    float integral;    // 积分累积（消除静态误差）
    float derivative;  // 微分变化（预测趋势）
    float last_error;  // 上一次偏差
    float base_dac;    // DAC基础输出值（未补偿前的值）
    unsigned int compensation;// PID计算出的补偿量（如0.05V对应的DAC值）
	unsigned int stored_compensation;   // 存储的补偿值
	float outputmax;   // 输出项的最大值
	float Integralmax; // 积分项的最大值
	unsigned int target_index;       // 目标值索引（用于查表）
    unsigned int compensation_valid;  // 补偿值有效性标志
} PID_Controller;

typedef struct {
    unsigned int voltage_index;      // 电压索引（0-2999对应0.01-30.00V）
    unsigned int voltage_compensation;      // 电压补偿值
    unsigned int voltage_valid;     // 电压补偿有效性
    
    unsigned int current_index;      // 电流索引（0-4999对应0.001-5.000A）  
    unsigned int current_compensation;      // 电流补偿值
    unsigned int current_valid;     // 电流补偿有效性
    
    unsigned long timestamp;         // 存储时间戳
} Compensation_Record;

// 补偿表声明
extern uint16_t voltage_compensation_table[VOLTAGE_TABLE_SIZE];
extern uint16_t current_compensation_table[CURRENT_TABLE_SIZE];




extern PID_Controller voltage_pid;  // 电压补偿器
extern PID_Controller current_pid;  // 电流补偿器
extern unsigned int compensation_voltage_data;
extern unsigned int compensation_current_data;

void Compensator_Init(PID_Controller *comp, float Kp, float Ki, float Kd, float Limit_value);
uint16_t Voltage_PID_Compensation(uint16_t target_voltage, uint16_t actual_voltage);
uint16_t Current_PID_Compensation(uint16_t target_current, uint16_t actual_current);
uint16_t Fast_Call_Voltage_Compensation(uint16_t target_voltage);
uint16_t Fast_Call_Current_Compensation(uint16_t target_current);

#endif