#ifndef _WORK_HANDLE_H
#define _WORK_HANDLE_H
#include "key.h"
#define WORK_TASK_TIME          5


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