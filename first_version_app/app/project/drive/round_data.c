#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "round_data.h"

/**
 * @brief 高精度四舍五入（通过字符串处理避免浮点误差）
 * @param value 输入浮点数
 * @param decimal_places 小数位数
 * @return 四舍五入后的浮点数
 */
double precise_round(double value, int decimal_places) {
    if (decimal_places < 0 || decimal_places > 15) {
        return value;
    }
    
    char buffer[64];
    int int_part = (int)value;
    double frac_part = value - int_part;
    
    // 格式化输出，保留足够多的小数位
    snprintf(buffer, sizeof(buffer), "%.*f", decimal_places + 1, value);
    
    // 手动解析并实现四舍五入逻辑
    char *dot_pos = strchr(buffer, '.');
    if (dot_pos != NULL) {
        char *digit_after_decimal = dot_pos + decimal_places + 1;
        if (*digit_after_decimal >= '5') {
            // 需要进位
            *(dot_pos + decimal_places + 1) = '\0'; // 截断
            double truncated = atof(buffer);
            double increment = 1.0;
            for (int i = 0; i < decimal_places; i++) {
                increment /= 10.0;
            }
            return truncated + increment;
        } else {
            // 直接截断
            *(dot_pos + decimal_places + 1) = '\0';
            return atof(buffer);
        }
    }
    
    return value; // 没有小数点，返回原值
}



double generic_round(double value, int decimal_places, Round_Type type) {
    switch (type) {
        case ROUND_TYPE_FLOAT:
            // float精度限制，最多保留6-7位有效数字
            if (decimal_places > 6) decimal_places = 6;
            break;
        case ROUND_TYPE_DOUBLE:
            // double精度限制，最多保留15-16位有效数字
            if (decimal_places > 15) decimal_places = 15;
            break;
        case ROUND_TYPE_LONG_DOUBLE:
            // long double精度更高，但实现依赖编译器
            if (decimal_places > 18) decimal_places = 18;
            break;
    }
    
    double multiplier = pow(10.0, decimal_places);
    double scaled = value * multiplier;
    double rounded = ROUND_TO_INT(scaled);
    return rounded / multiplier;
}