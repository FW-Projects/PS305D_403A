#include "EncoderEC11.h"
#include <stdio.h>
// 在初始化结构体中添加新字段


//voltage_ec11
EC11_Encoder voltage_ec11 =
{
    .config.gpio_a                         = VOL_EC11_A_GPIO_PORT,
    .config.gpio_b                         = VOL_EC11_B_GPIO_PORT,
    .config.gpio_key                       = VOL_EC11_KEY_GPIO_PORT,
    .config.pin_a                          = VOL_EC11_A_GPIO_PIN,
    .config.pin_b                          = VOL_EC11_B_GPIO_PIN,
    .config.pin_key                        = VOL_EC11_KEY_GPIO_PIN,
    .config.type                           = EC11_TYPE_SINGLE_PULSE,
    .config.scan_period_ms                 = 4,

    // 时间参数(基于扫描周期计算)
    .config.debounce_count = 5,        // 20ms / 5ms = 4次
    .config.long_press_count = 150,    // 600ms / 5ms = 120次
    .config.double_click_timeout = 50, // 200ms / 5ms = 40次
    .config.long_repeat_count = 150,    // 200ms / 5ms = 40次
    .config.fast_rotation_threshold = 15, // 快速旋转阈值(50ms / 5ms = 10次)

    .a_last                                = 0,
    .b_last                                = 0,
    .key_last                              = 0,
    .key_current                           = 0,

    .key_count                             = 0,
    .double_click_timeout                  = 0,
    .click_count                           = 0,
    .last_key_state                        = 1,
    .tmp_value                             = 0,
    .double_click_triggered                = 0, // 新增字段

    .rotation_count_since_last             = 0,
    .current_speed                         = EC11_SPEED_NORMAL
};


//current_ec11
EC11_Encoder current_ec11 =
{
    .config.gpio_a                         = CUR_EC11_A_GPIO_PORT,
    .config.gpio_b                         = CUR_EC11_B_GPIO_PORT,
    .config.gpio_key                       = CUR_EC11_KEY_GPIO_PORT,
    .config.pin_a                          = CUR_EC11_A_GPIO_PIN,
    .config.pin_b                          = CUR_EC11_B_GPIO_PIN,
    .config.pin_key                        = CUR_EC11_KEY_GPIO_PIN,
    .config.type                           = EC11_TYPE_SINGLE_PULSE,
    .config.scan_period_ms                 = 4,

    // 时间参数(基于扫描周期计算)
    .config.debounce_count = 5,        // 20ms / 5ms = 4次
    .config.long_press_count = 150,    // 600ms / 5ms = 120次
    .config.double_click_timeout = 50, // 200ms / 5ms = 40次
    .config.long_repeat_count = 50,    // 200ms / 5ms = 40次
    .config.fast_rotation_threshold = 15, // 快速旋转阈值(50ms / 5ms = 10次)

    .a_last                                = 0,
    .b_last                                = 0,
    .key_last                              = 0,
    .key_current                           = 0,

    .key_count                             = 0,
    .double_click_timeout                  = 0,
    .click_count                           = 0,
    .last_key_state                        = 1,
    .tmp_value                             = 0,
    .double_click_triggered                = 0, // 新增字段

    .rotation_count_since_last             = 0,
    .current_speed                         = EC11_SPEED_NORMAL
};



// 扫描EC11编码器（保持不变）
EC11_ScanResult EC11_Scan(EC11_Encoder *encoder)
{
	static uint8_t a_now = 0,b_now = 0,key_now = 0;
    EC11_ScanResult result = EC11_NO_ACTION;

    // 读取当前引脚状态
    a_now = EC11_ReadPin(encoder->config.gpio_a, encoder->config.pin_a);
    b_now = EC11_ReadPin(encoder->config.gpio_b, encoder->config.pin_b);
    key_now = EC11_ReadPin(encoder->config.gpio_key, encoder->config.pin_key);

    // 更新按键状态
    encoder->key_current = key_now;
    encoder->key_last = key_now;

    // 根据编码器类型处理旋转（保持不变）
    if (encoder->config.type == EC11_TYPE_SINGLE_PULSE)
    {
        // 一定位对应一脉冲

        if (a_now != encoder->a_last)
        {
            if (a_now == 0)
            {
                if (b_now == 1)
                {
                    result = EC11_CLOCKWISE; // 正转
                }
                else
                {
                    result = EC11_COUNTERCLOCKWISE; // 反转
                }
            }
            encoder->a_last = a_now;
            encoder->b_last = b_now;
        }
    }
    else
    {
        // 两定位对应一脉冲
        if (a_now != encoder->a_last)
        {
            if (a_now == 1)
            { // A上升沿
                if ((encoder->b_last == 1) && (b_now == 0))
                {
                    result = EC11_CLOCKWISE; // 正转
                }
                else if ((encoder->b_last == 0) && (b_now == 1))
                {
                    result = EC11_COUNTERCLOCKWISE; // 反转
                }
                else if ((encoder->b_last == b_now) && (b_now == 0))
                {
                    result = EC11_CLOCKWISE; // 正转
                }
                else if ((encoder->b_last == b_now) && (b_now == 1))
                {
                    result = EC11_COUNTERCLOCKWISE; // 反转
                }
            }
            else
            { // A下降沿
                if ((encoder->b_last == 1) && (b_now == 0))
                {
                    result = EC11_COUNTERCLOCKWISE; // 反转
                }
                else if ((encoder->b_last == 0) && (b_now == 1))
                {
                    result = EC11_CLOCKWISE; // 正转
                }
                else if ((encoder->b_last == b_now) && (b_now == 0))
                {
                    result = EC11_COUNTERCLOCKWISE; // 反转
                }
                else if ((encoder->b_last == b_now) && (b_now == 1))
                {
                    result = EC11_CLOCKWISE; // 正转
                }
            }
            encoder->a_last = a_now;
            encoder->b_last = b_now;
        }
    }

    // 处理按键
    if (key_now == 0)
    { // 按键按下
        if (result == EC11_NO_ACTION)
        {
            result = EC11_KEY_PRESSED; // 只按下按键
        }
        else if (result == EC11_CLOCKWISE)
        {
            result = EC11_KEY_CW; // 按下并正转
        }
        else if (result == EC11_COUNTERCLOCKWISE)
        {
            result = EC11_KEY_CCW; // 按下并反转
        }
    }

    return result;
}

// 分析EC11编码器动作 - 完全修复双击后触发单击的问题
EC11_AnalyzeResult EC11_Analyze(EC11_Encoder *encoder, EC11_ScanResult value)
{
    EC11_AnalyzeResult result = EC11_ANALYZE_NO_ACTION;
    
    // 更新旋转计数（用于速度检测）
    if (encoder->rotation_count_since_last < 0xFFFF) {
        encoder->rotation_count_since_last++;
    }

    // 处理旋转事件
    switch (value)
    {
    case EC11_CLOCKWISE:
        encoder->current_speed = EC11_CheckRotationSpeed(encoder);
        result = (encoder->current_speed == EC11_SPEED_FAST) ? 
                 EC11_ANALYZE_FAST_CW : EC11_ANALYZE_CW;
        break;

    case EC11_COUNTERCLOCKWISE:
        encoder->current_speed = EC11_CheckRotationSpeed(encoder);
        result = (encoder->current_speed == EC11_SPEED_FAST) ? 
                 EC11_ANALYZE_FAST_CCW : EC11_ANALYZE_CCW;
        break;

    case EC11_KEY_CW:
        result = EC11_ANALYZE_KEY_CW;
        // 重置所有按键状态
        EC11_ResetKeyState(encoder);
        break;

    case EC11_KEY_CCW:
        result = EC11_ANALYZE_KEY_CCW;
        // 重置所有按键状态
        EC11_ResetKeyState(encoder);
        break;

    default:
        break;
    }

    // 独立的按键状态处理
    uint8_t key_pressed = (encoder->key_current == 0);
    uint8_t key_released = (encoder->last_key_state == 0 && encoder->key_current == 1);
    
    // 更新上一次按键状态
    encoder->last_key_state = encoder->key_current;

    // 如果双击已触发，则忽略所有后续按键处理
    if (encoder->double_click_triggered) {
        if (key_released) {
            // 双击后的第一次释放，重置状态
            EC11_ResetKeyState(encoder);
        }
        return result;
    }

    // 双击超时处理 - 每次扫描都递减
    if (encoder->double_click_timeout > 0) {
        encoder->double_click_timeout--;
        
        // 双击超时，触发短按
        if (encoder->double_click_timeout == 0 && encoder->click_count == 1) {
            result = EC11_ANALYZE_SHORT_CLICK;
            EC11_ResetKeyState(encoder);
        }
    }

    if (key_pressed)
    {
        // 按键按下处理
        encoder->key_count++;
        
        // 消抖确认
        if (encoder->key_count == encoder->config.debounce_count)
        {
            // 在双击超时时间内第二次按键
            if (encoder->double_click_timeout > 0 && encoder->click_count == 1)
            {
                // 确认为双击
                result = EC11_ANALYZE_DOUBLE_CLICK;
                // 设置双击已触发标志
                encoder->double_click_triggered = 1;
                // 不重置状态，等待按键释放后再重置
            }
            else
            {
                // 第一次按键
                encoder->click_count = 1;
            }
        }
        
        // 长按检测
        if (encoder->key_count >= encoder->config.long_press_count)
        {
            // 长按重复
            encoder->tmp_value++;
            if (encoder->tmp_value % encoder->config.long_repeat_count == 0)
            {
                encoder->tmp_value = 0;
                result = EC11_ANALYZE_LONG_PRESS;
            }
        }
    }
    else if (key_released)
    {
        // 按键释放处理
        if (encoder->key_count >= encoder->config.debounce_count)
        {
            // 短按释放（未达到长按时间）
            if (encoder->key_count < encoder->config.long_press_count)
            {
                // 第一次点击释放，启动双击超时
                if (encoder->click_count == 1 && encoder->double_click_timeout == 0)
                {
                    encoder->double_click_timeout = encoder->config.double_click_timeout;
                }
            }
            // 长按释放
            else
            {
                result = EC11_ANALYZE_LONG_RELEASE;
                // 重置状态
                EC11_ResetKeyState(encoder);
            }
        }
        // 无论是否消抖，释放时都重置key_count
        encoder->key_count = 0;
    }

    return result;
}
// 重置按键状态
static void EC11_ResetKeyState(EC11_Encoder *encoder)
{
    encoder->key_count = 0;
    encoder->double_click_timeout = 0;
    encoder->click_count = 0;
    encoder->tmp_value = 0;
    encoder->double_click_triggered = 0;
}
// 读取GPIO引脚状态
static uint8_t EC11_ReadPin(gpio_type *gpio, uint16_t pin)
{
    return gpio_input_data_bit_read(gpio, pin) == 1 ? 1 : 0;
}

// 检查旋转速度
static EC11_RotationSpeed EC11_CheckRotationSpeed(EC11_Encoder *encoder)
{
    // 判断是否为快速旋转
    if (encoder->rotation_count_since_last < encoder->config.fast_rotation_threshold)
    {
        encoder->rotation_count_since_last = 0;
        return EC11_SPEED_FAST;
    }
    else
    {
        encoder->rotation_count_since_last = 0;
        return EC11_SPEED_NORMAL;
    }
}


