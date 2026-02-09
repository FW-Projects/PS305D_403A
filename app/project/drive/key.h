#ifndef _KEY_H
#define _KEY_H

#include <stdio.h>
#include "at32f403a_407.h"

/* input */
#define MOD1_PORT                GPIOC
#define MOD1_PIN                 GPIO_PINS_10
#define READ_MOD1                gpio_input_data_bit_read(MOD1_PORT, MOD1_PIN)

#define MOD2_PORT                GPIOC
#define MOD2_PIN                 GPIO_PINS_11
#define READ_MOD2                gpio_input_data_bit_read(MOD2_PORT, MOD2_PIN)

#define MOD3_PORT                GPIOA
#define MOD3_PIN                 GPIO_PINS_15
#define READ_MOD3                gpio_input_data_bit_read(MOD3_PORT, MOD3_PIN)

#define MOD4_PORT                GPIOC
#define MOD4_PIN                 GPIO_PINS_12
#define READ_MOD4                gpio_input_data_bit_read(MOD4_PORT, MOD4_PIN)

#define A_LIMIN_PORT             GPIOA
#define A_LIMIN_PIN              GPIO_PINS_8
#define READ_A_LIMIN             gpio_input_data_bit_read(A_LIMIN_PORT, A_LIMIN_PIN)

#define V_LIMIN_PORT             GPIOC
#define V_LIMIN_PIN              GPIO_PINS_14
#define READ_V_LIMIN             gpio_input_data_bit_read(V_LIMIN_PORT, V_LIMIN_PIN)

#define REACH_5V_PORT            GPIOD
#define REACH_5V_PIN             GPIO_PINS_2
#define READ_REACH_5V            gpio_input_data_bit_read(REACH_5V_PORT, REACH_5V_PIN)

#define CC_CV_PORT               GPIOB
#define CC_CV_PIN                GPIO_PINS_14
#define READ_CC_CV               gpio_input_data_bit_read(CC_CV_PORT, CC_CV_PIN)

#define CONT_OCP_PORT            GPIOB
#define CONT_OCP_PIN             GPIO_PINS_15
#define READ_CONT_OCP            gpio_input_data_bit_read(CONT_OCP_PORT, CONT_OCP_PIN)

#define ONCE_OCP_PORT            GPIOC
#define ONCE_OCP_PIN             GPIO_PINS_15
#define READ_ONCE_OCP            gpio_input_data_bit_read(ONCE_OCP_PORT, ONCE_OCP_PIN)

#define OUTPUT_PORT              GPIOB
#define OUTPUT_PIN               GPIO_PINS_13
#define READ_OUTPUT              gpio_input_data_bit_read(OUTPUT_PORT, OUTPUT_PIN)

#define LONG_PRESS_TIME          40 
#define KEY_CYCLE_TIME           50
#define KEY_NUMBER               11
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    K_RELEASE,
    K_PRESS,
} KEY_VALUE;

typedef enum {
    KS_RELEASE,
	//KS_LONG_RELEASE,
	KS_CHECK,
    KS_PRESS,
	KS_LONG_PRESS,
} KEY_STATUS;

typedef enum {
    KE_PRESS,
    KE_RELEASE,
    KE_LONG_PRESS,
    KE_LONG_RELEASE,
    KE_NONE,
} KEY_EVENT;

typedef struct {
    KEY_STATUS status;
    int count;
	int key_cycle_time;
    KEY_VALUE (*get)(void);
} KEY;

KEY_EVENT key_event_check(KEY *key, int interval);

#ifdef __cplusplus
}
#endif
#endif


