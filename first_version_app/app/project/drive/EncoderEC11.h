#ifndef __ENCODER_EC11_H
#define __ENCODER_EC11_H
#include <stdint.h>
#include <stdbool.h>

#include "at32f403a_407.h"

#define VOL_EC11_A_GPIO_PORT    GPIOB
#define VOL_EC11_A_GPIO_PIN     GPIO_PINS_11

#define VOL_EC11_B_GPIO_PORT    GPIOB
#define VOL_EC11_B_GPIO_PIN     GPIO_PINS_10

#define VOL_EC11_KEY_GPIO_PORT  GPIOC
#define VOL_EC11_KEY_GPIO_PIN   GPIO_PINS_6

#define CUR_EC11_A_GPIO_PORT    GPIOA
#define CUR_EC11_A_GPIO_PIN     GPIO_PINS_3

#define CUR_EC11_B_GPIO_PORT    GPIOA
#define CUR_EC11_B_GPIO_PIN     GPIO_PINS_2

#define CUR_EC11_KEY_GPIO_PORT  GPIOA
#define CUR_EC11_KEY_GPIO_PIN   GPIO_PINS_1


// 编码器类型定义
typedef enum
{
    EC11_TYPE_SINGLE_PULSE = 0, // 一定位对应一脉冲
    EC11_TYPE_DOUBLE_PULSE = 1  // 两定位对应一脉冲
} EC11_Type;

// 旋转速度定义
typedef enum
{
    EC11_SPEED_NORMAL = 0,      // 正常速度
    EC11_SPEED_FAST = 1         // 快速旋转
} EC11_RotationSpeed;

// 编码器扫描结果
typedef enum
{
    EC11_NO_ACTION = 0,         // 无动作
    EC11_CLOCKWISE = 1,         // 正转
    EC11_COUNTERCLOCKWISE = -1, // 反转
    EC11_KEY_PRESSED = 2,       // 只按下按键
    EC11_KEY_CW = 3,            // 按着按键正转
    EC11_KEY_CCW = -3           // 按着按键反转
} EC11_ScanResult;

// 编码器分析结果
typedef enum
{
    EC11_ANALYZE_NO_ACTION = 0,    // 无动作
    EC11_ANALYZE_CW = 1,           // 正转
    EC11_ANALYZE_CCW = 2,          // 反转
    EC11_ANALYZE_KEY_CW = 3,       // 按下并正转
    EC11_ANALYZE_KEY_CCW = 4,      // 按下并反转
    EC11_ANALYZE_SHORT_CLICK = 5,  // 短按
    EC11_ANALYZE_DOUBLE_CLICK = 6, // 双击
    EC11_ANALYZE_LONG_RELEASE = 7, // 长按结束
    EC11_ANALYZE_LONG_PRESS = 8,   // 长按
    EC11_ANALYZE_FAST_CW = 9,      // 快速正转
    EC11_ANALYZE_FAST_CCW = 10     // 快速反转
} EC11_AnalyzeResult;

// 编码器配置结构体
typedef struct
{
    gpio_type *gpio_a;   // A相GPIO端口
    uint16_t pin_a;         // A相引脚
    gpio_type *gpio_b;   // B相GPIO端口
    uint16_t pin_b;         // B相引脚
    gpio_type *gpio_key; // 按键GPIO端口
    uint16_t pin_key;       // 按键引脚
    EC11_Type type;         // 编码器类型
    uint8_t scan_period_ms; // 扫描周期(ms)
    
    // 时间参数(基于扫描周期计算)
    uint16_t debounce_count;        // 消抖计数
    uint16_t long_press_count;      // 长按计数
    uint16_t double_click_timeout;  // 双击超时计数
    uint16_t long_repeat_count;     // 长按重复计数
    uint16_t fast_rotation_threshold; // 快速旋转阈值(扫描次数)
} EC11_Config;

typedef struct
{
    // 配置参数
    EC11_Config config;

    // 状态变量
    uint8_t a_last;
    uint8_t b_last;
    uint8_t key_last;
    uint8_t key_current;

    // 按键相关变量
    uint16_t key_count;
    uint16_t double_click_timeout;
    uint8_t click_count;
    uint8_t last_key_state;
    uint16_t tmp_value;
    uint8_t double_click_triggered; // 新增：双击已触发标志

    // 旋转速度检测相关变量
    uint16_t rotation_count_since_last;
    EC11_RotationSpeed current_speed;
} EC11_Encoder;

// 公共方法
void EC11_Init(EC11_Encoder *encoder, EC11_Config *config);
EC11_ScanResult EC11_Scan(EC11_Encoder *encoder);
EC11_AnalyzeResult EC11_Analyze(EC11_Encoder *encoder, EC11_ScanResult value);
static void EC11_ResetKeyState(EC11_Encoder *encoder);
// 私有方法(内部使用)
static uint8_t EC11_ReadPin(gpio_type *gpio, uint16_t pin);
static EC11_RotationSpeed EC11_CheckRotationSpeed(EC11_Encoder *encoder);


extern EC11_Encoder voltage_ec11;
extern EC11_Encoder current_ec11;
#endif // __ENCODER_EC11_H



	