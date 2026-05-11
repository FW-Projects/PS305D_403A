#ifndef __LCD_HANDLE_H
#define __LCD_HANDLE_H
#include "TM1680.h"

#define LCD_TASK_TIME          10




#define LCDSEG_A 0x40
#define LCDSEG_B 0x10
#define LCDSEG_C 0x04
#define LCDSEG_D 0x01
#define LCDSEG_E 0x80
#define LCDSEG_F 0x20
#define LCDSEG_G 0x08
#define LCDSEG_DP 0x02

#define CHAR_0 LCDSEG_A + LCDSEG_B + LCDSEG_C + LCDSEG_D + LCDSEG_E + LCDSEG_F
#define CHAR_1 LCDSEG_B + LCDSEG_C
#define CHAR_2 LCDSEG_A + LCDSEG_B + LCDSEG_D + LCDSEG_E + LCDSEG_G
#define CHAR_3 LCDSEG_A + LCDSEG_B + LCDSEG_C + LCDSEG_D + LCDSEG_G
#define CHAR_4 LCDSEG_B + LCDSEG_C + LCDSEG_F + LCDSEG_G
#define CHAR_5 LCDSEG_A + LCDSEG_C + LCDSEG_D + LCDSEG_F + LCDSEG_G
#define CHAR_6 LCDSEG_A + LCDSEG_C + LCDSEG_D + LCDSEG_E + LCDSEG_F + LCDSEG_G
#define CHAR_7 LCDSEG_A + LCDSEG_B + LCDSEG_C
#define CHAR_8 LCDSEG_A + LCDSEG_B + LCDSEG_C + LCDSEG_D + LCDSEG_E + LCDSEG_F + LCDSEG_G
#define CHAR_9 LCDSEG_A + LCDSEG_B + LCDSEG_C + LCDSEG_D + LCDSEG_F + LCDSEG_G
#define CHAR_r LCDSEG_E + LCDSEG_G
#define CHAR_b LCDSEG_C + LCDSEG_D + LCDSEG_E + LCDSEG_F + LCDSEG_G
#define CHAR_C LCDSEG_A + LCDSEG_D + LCDSEG_E + LCDSEG_F
#define CHAR_d LCDSEG_B + LCDSEG_C + LCDSEG_D + LCDSEG_E + LCDSEG_G
#define CHAR_E LCDSEG_A + LCDSEG_D + LCDSEG_E + LCDSEG_F + LCDSEG_G
#define CHAR_F LCDSEG_A + LCDSEG_E + LCDSEG_F + LCDSEG_G
#define CHAR_H LCDSEG_B + LCDSEG_C + LCDSEG_E + LCDSEG_F + LCDSEG_G
#define CHAR_n LCDSEG_A + LCDSEG_B + LCDSEG_C + LCDSEG_E + LCDSEG_F

typedef enum
{
	CV_CONTOCP = 0xa0,/* CV & CONT_OCP */
	CV_ONCEOCP = 0x60,/* CV & ONCE_OCP */
	CV_NOOCP = 0x20,  /* CV & NO_OCP */
	CC_NOOCP = 0x10,  /* CC & NO_OCP */
} led_postion1_e;

typedef enum
{
	VLimin_ALimin_Lock = 0xd0,/* 8.4Vlimin & 500MAlimin & lock */
	VLimin_Lock = 0x50,       /* 8.4Vlimin & lock */
	ALimin_Lock = 0x90,       /* 500MAlimin & lock */
	Lock = 0x10,              /* lock */
	VLimin_ALimin = 0xc0,     /* 8.4Vlimin & 500MAlimin */
	VLimin = 0x40,            /* 8.4Vlimin  */
	ALimin = 0x80,            /* 500MAlimin  */
	NOTHING = 0x00,           /* NOTHING  */
} led_postion2_e;


extern uint8_t table[];
extern int actual_display_times;


void lcd_handle(void);

#endif 