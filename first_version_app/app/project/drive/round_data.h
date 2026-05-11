#ifndef _ROUND_DATA_H
#define _ROUND_DATA_H


// 宏定义实现通用四舍五入
#define ROUND_TO_INT(x) (((x) >= 0) ? ((int)((x) + 0.5)) : ((int)((x) - 0.5)))
#define ROUND_TO_DECIMAL(x, places) (ROUND_TO_INT((x) * pow(10, (places))) / pow(10, (places)))


// 函数模板实现
typedef enum {
    ROUND_TYPE_FLOAT,
    ROUND_TYPE_DOUBLE,
    ROUND_TYPE_LONG_DOUBLE
} Round_Type;


#endif