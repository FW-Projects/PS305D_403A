#ifndef __TM1680_H
#define __TM1680_H

#include "at32f403a_407_gpio.h"

#define TM1680ID 0xe4 // 1110-01A1A0
#define SYSDIS 0x80   // 1000-0000 关闭系统时钟和LED循环
#define RCMODE1 0x9a
#define RCMODE0 0x98
#define COM8NOMS 0xa0 // 1010-0000 8COM,NMOS
#define COM8POMS 0xa8 // 1010-0000 8COM,NMOS
#define SYSEN 0x81
#define LEDON 0x83
#define LEDOFF 0x82
#define PWM16 0xbf
#define BLINKOFF 0x88


#define SDA_PIN  GPIO_PINS_8
#define SDA_PORT GPIOC

#define SCK_PIN  GPIO_PINS_9
#define SCK_PORT GPIOC

//模式掩码（2位模式位）
#define GPIO_MODE_MASK 0x03

//输出模式 ：推挽输出 0b0011
#define GPIO_OUTPUT_MODE 0x03

//输入模式 ： 浮空输入 0b0000
#define GPIO_INPUT_MODE 0x00


//切换为输出模式
#define SET_PIN_OUTPUT  do { \
    SDA_PORT->cfghr &= ~(GPIO_MODE_MASK << 0); \
    SDA_PORT->cfghr |= (GPIO_OUTPUT_MODE << 0); \
} while(0)


//切换为输入模式
#define SET_PIN_INPUT do{  \
	SDA_PORT->cfghr &= ~(GPIO_MODE_MASK << 0); \
	SDA_PORT->cfghr |= (GPIO_INPUT_MODE << 0); \
}while(0)

#define SDA_HIGH gpio_bits_set(GPIOC,GPIO_PINS_8)
#define SDA_LOW  gpio_bits_reset(GPIOC,GPIO_PINS_8)
#define SCK_HIGH gpio_bits_set(GPIOC,GPIO_PINS_9)
#define SCK_LOW  gpio_bits_reset(GPIOC,GPIO_PINS_9)

void TM1680WriteByte(uint8_t addr,uint8_t high_dat,uint8_t low_dat);
void TM1680Write4bit(uint8_t addr, uint8_t dat);
void TM1680PageAllWrite(uint8_t faddr, uint8_t dat1, uint8_t dat2, uint8_t cnt);
void Disp_one_person(uint8_t addr, uint8_t dat);
void TM1680Init(void);


#endif 