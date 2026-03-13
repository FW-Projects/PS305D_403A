#ifndef _WORK_HANDLE_H
#define _WORK_HANDLE_H
#include "key.h"
#define WORK_TASK_TIME          4

// 过流保护相关
#define CONT_OCP_PROTECT_TIME_VAL    500    // 持续过流保护恢复时间
#define OCP_TRIGGER_NUMBER_VAL       5       // 过流触发次数阈值
#define CV_TRIGGER_NUMBER_VAL        5       // 恒压恢复触发次数阈值
// 风扇控制相关
#define FAN_CHECK_INTERVAL           3000    // 风扇温度检测间隔
#define FAN_PWM_BASE_VAL             5599    // 风扇PWM基准值
#define FAN_PWM_SCALE_40             0.4     // 风扇40%转速（低温）
#define FAN_PWM_SCALE_60             0.6     // 风扇60%转速（中温）
#define FAN_PWM_SCALE_80             0.8     // 风扇80%转速（高温）
#define FAN_PWM_SCALE_100            1.0     // 风扇100%转速（超高温）
#define NTC_TEMP_THRESHOLD_1         0x250   // NTC温度阈值1（低温）
#define NTC_TEMP_THRESHOLD_2         0x300   // NTC温度阈值2（中温）
#define NTC_TEMP_THRESHOLD_3         0x350   // NTC温度阈值3（高温）
// 电流表检测相关
#define AMMETER_CHECK_TIMES          5       // 电流表状态检测次数阈值
#define AMMETER_RELAY_DELAY_NO       1000    // 无表时继电器操作延迟(ms)
#define AMMETER_RELAY_DELAY_HAVE     500     // 有表时继电器操作延迟(ms)
#define VOLTAGE_CHECK_TIMES          20      // 输出电压检测次数阈值
// 变压器电压档位阈值（直流）
#define VOLT_GEAR_5V                 500     // 0~5V档位阈值
#define VOLT_GEAR_14V                1400    // 5.01~14V档位阈值
#define VOLT_GEAR_21V                2100    // 14.01~21V档位阈值

#define JK_1_HIGH gpio_bits_set(GPIOC,GPIO_PINS_2)
#define JK_1_LOW  gpio_bits_reset(GPIOC,GPIO_PINS_2)

#define JK_2_HIGH gpio_bits_set(GPIOC,GPIO_PINS_1)
#define JK_2_LOW  gpio_bits_reset(GPIOC,GPIO_PINS_1)

#define OUTPUT_LED_OPEN  gpio_bits_reset(GPIOB,GPIO_PINS_12)
#define OUTPUT_LED_CLOSE gpio_bits_set(GPIOB,GPIO_PINS_12)

#define OUTPUT_OPEN  gpio_bits_reset(GPIOA,GPIO_PINS_0)
#define OUTPUT_CLOSE gpio_bits_set(GPIOA,GPIO_PINS_0)

void work_handle(void);
#endif