/* pid计算电压电流输出补偿值
 * 只做一次计算后给到内存、
 * 后续只需要调取内存反馈到DAC输出即可
 */
#include "PS305D_handle.h"
#include "PID_handle.h"
#include <math.h>
#include "perf_counter.h"
#include "flash.h"

// 补偿表声明
uint16_t voltage_compensation_table[VOLTAGE_TABLE_SIZE] = {0};
uint16_t current_compensation_table[CURRENT_TABLE_SIZE] = {0};

Compensation_Record com_record;

PID_Controller voltage_pid; // 电压补偿器
PID_Controller current_pid; // 电流补偿器

void Voltage_Compensation_Loop(void);
void Current_Compensation_Loop(void);

/**
 * @brief 存储电压补偿值到EEPROM
 * @param voltage_target 目标电压值 0-3000
 * @param compensation 计算得到的补偿值
 */
void Store_Voltage_Compensation(uint16_t voltage_target, uint16_t compensation)
{
    uint16_t index = voltage_target; // 0.01V精度索引

    if (index >= VOLTAGE_TABLE_SIZE)
        return; // 超出范围

    // 更新内存表
    voltage_compensation_table[index] = compensation;
    com_record.voltage_valid = 1;
    com_record.timestamp = get_system_ms();

    // 写入EEPROM（需要AT32F403A的EEPROM操作函数）
    flash_write_halfword(voltage_compensation_table[index], EEPROM_VOLTAGE_START_ADDR + index * 2);
}

/**
 * @brief 从EEPROM读取补偿值
 * @param voltage_target 目标电压值
 * @return 读取到的补偿值
 */
float Read_Voltage_Compensation(uint16_t voltage_target)
{
    uint16_t index = voltage_target;

    if (index >= VOLTAGE_TABLE_SIZE)
        return 0;

    // 从EEPROM读取到内存表
    voltage_compensation_table[index] = flash_read_halfword(EEPROM_VOLTAGE_START_ADDR + index * 2);

    if (com_record.voltage_valid)
    {
        return voltage_compensation_table[index];
    }

    return 0; // 无效补偿值
}

/**
 * @brief 存储电流补偿值到EEPROM
 * @param current_target 目标电压值 0-3000
 * @param compensation 计算得到的补偿值
 */
void Store_Current_Compensation(uint16_t current_target, uint16_t compensation)
{
    uint16_t index = current_target; // 0.01V精度索引

    if (index >= CURRENT_TABLE_SIZE)
        return; // 超出范围

    // 更新内存表
    current_compensation_table[index] = compensation;
    com_record.current_valid = 1;
    com_record.timestamp = get_system_ms();

    // 写入EEPROM（需要AT32F403A的EEPROM操作函数）
    flash_write_halfword(current_compensation_table[index], EEPROM_CURRENT_START_ADDR + index * 2);
}

/**
 * @brief 从EEPROM读取补偿值
 * @param voltage_target 目标电压值
 * @return 读取到的补偿值
 */
float Read_Current_Compensation(uint16_t current_target)
{
    uint16_t index = current_target;

    if (index >= CURRENT_TABLE_SIZE)
        return 0;

    // 从EEPROM读取到内存表
    current_compensation_table[index] = flash_read_halfword(EEPROM_CURRENT_START_ADDR + index * 2);

    if (com_record.current_valid)
    {
        return current_compensation_table[index];
    }

    return 0; // 无效补偿值
}

/**
 * @brief 快速调用电压补偿值（跳过PID计算）
 * @param target_voltage 目标电压值
 * @return 预存储的补偿值
 */
uint16_t Fast_Call_Voltage_Compensation(uint16_t target_voltage)
{
    uint16_t index = target_voltage;

    if (index >= VOLTAGE_TABLE_SIZE)
        return 0.0;

    // 优先使用内存缓存，否则从EEPROM读取
    if (com_record.voltage_valid)
    {
        return voltage_compensation_table[index];
    }
    else
    {
        
            // 从EEPROM读取并缓存到内存
            uint16_t compensation = Read_Voltage_Compensation(target_voltage);
        if (compensation != 0)
        {
            voltage_compensation_table[index] = compensation;
            com_record.voltage_valid = 1;
        }
        return compensation;
    }
}

/**
 * @brief 快速调用电流补偿值
 * @param target_current 目标电流值
 * @return 预存储的补偿值
 */
uint16_t Fast_Call_Current_Compensation(uint16_t target_current)
{
    uint16_t index = target_current;

    if (index >= CURRENT_TABLE_SIZE)
        return 0;

    if (com_record.current_valid)
    {
        return current_compensation_table[index];
    }
    else
    {
        uint16_t compensation = Read_Current_Compensation(target_current);
        if (compensation != 0)
        {
            current_compensation_table[index] = compensation;
            com_record.current_valid = 1;
        }
        return compensation;
    }
}

/* PID */

/**
 * @brief 电压环PID补偿计算与存储
 * @param target_voltage 目标电压
 * @param actual_voltage 实际输出电压
 * @return 补偿后的DAC输出值
 */
uint16_t Voltage_PID_Compensation(uint16_t target_voltage, uint16_t actual_voltage)
{
    // 1. 计算偏差
    voltage_pid.error = target_voltage - actual_voltage;

    // 2. 积分项（防积分饱和）
    if (fabs(voltage_pid.error) < 100)
    {                                              // 偏差<0.1V时积分
        voltage_pid.integral += voltage_pid.error; // 1ms采样周期
    }

    // 3. 微分项
    voltage_pid.derivative = (voltage_pid.error - voltage_pid.last_error);

    // 4. PID输出计算
    voltage_pid.compensation = voltage_pid.Kp * voltage_pid.error +
                               voltage_pid.Ki * voltage_pid.integral +
                               voltage_pid.Kd * voltage_pid.derivative;

    // 5. 补偿量限幅
    if (voltage_pid.compensation > 70)
        voltage_pid.compensation = 70; // 最大补偿0.5V = 68.25
    if (voltage_pid.compensation < -70)
        voltage_pid.compensation = -70; // 最小补偿-0.5V = -68.25

    // 6. 更新历史值
    voltage_pid.last_error = voltage_pid.error;

    // 7. 系统稳定后存储补偿值
    if (fabs(current_pid.error) < 2 &&
        fabs(current_pid.derivative) < 2)
    {
        Store_Current_Compensation(target_voltage, voltage_pid.compensation);
    }

    // 8. 返回补偿量
    return voltage_pid.compensation;
}

/**
 * @brief 电流环PID补偿计算与存储
 * @param target_current 目标电压
 * @param actual_current 实际输出电压
 * @return 补偿后的DAC输出值
 */
uint16_t Current_PID_Compensation(uint16_t target_current, uint16_t actual_current)
{
    // 1. 计算偏差
    current_pid.error = target_current - actual_current;

    // 2. 积分项（防积分饱和）
    if (fabs(current_pid.error) < 100)
    {                                              // 偏差<0.1A时积分
        current_pid.integral += current_pid.error; // 1ms采样周期
    }

    // 3. 微分项
    current_pid.derivative = (current_pid.error - current_pid.last_error);

    // 4. PID输出计算
    current_pid.compensation = current_pid.Kp * current_pid.error +
                               current_pid.Ki * current_pid.integral +
                               current_pid.Kd * current_pid.derivative;

    // 5. 补偿量限幅
    if (current_pid.compensation > 82)
        current_pid.compensation = 82; // 最大补偿0.1A = 81.9
    if (current_pid.compensation < -82)
        current_pid.compensation = -82; // 最小补偿-0.1A = -81.9

    // 6. 更新历史值
    current_pid.last_error = current_pid.error;

    // 7. 系统稳定后存储补偿值
    if (fabs(current_pid.error) < 5 &&
        fabs(current_pid.derivative) < 5)
    {
        Store_Current_Compensation(target_current, current_pid.compensation);
    }

    // 8. 返回补偿量
    return current_pid.compensation;
}

void Compensator_Init(PID_Controller *PID, float Kp, float Ki, float Kd, float Limit_value)
{
    // 针对0.05V小偏差的优化参数（实测验证值）
    PID->Kp = Kp; // 比例系数：直接响应偏差
    PID->Ki = Ki; // 积分系数：缓慢消除静态误差
    PID->Kd = Kd; // 微分系数：抑制补偿过冲
    PID->integral = 0;
    PID->last_error = 0;
    PID->Integralmax = Limit_value;
    PID->outputmax = Limit_value;
}
