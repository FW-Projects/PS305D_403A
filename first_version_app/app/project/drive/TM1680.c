#include "TM1680.h"
#include "perf_counter.h"

/* tm1680起始信号 */
void tm1680_start(void)
{
	SDA_HIGH;
	SCK_HIGH;
	SDA_HIGH;
	delay_us(1);
	SDA_LOW;
	delay_us(5);// 起始信号必须大于4.7us
	SCK_LOW;
}

/* tm1680停止信号 */
void tm1680_stop(void)
{
	SDA_LOW;
	SCK_HIGH;
	delay_us(1);
	SDA_HIGH;
	delay_us(1);
	SCK_HIGH;
	SDA_HIGH;
}


/* 接受应答信号 */
void ACK(void) 
{
	uint8_t i;
	SET_PIN_INPUT;
	SCK_LOW;
	delay_us(1);
	SCK_HIGH;
	delay_us(1);
	
	for(i = 0; i < 50; i++)
	{
		if(!gpio_input_data_bit_read(SDA_PORT,SDA_PIN))
			break;
		delay_us(5);
	}
	SCK_LOW;
	delay_us(1);
	SET_PIN_OUTPUT;	
}

/* 发送应答信号 */
void send_ack(bool ack)
{
	SCK_LOW;
	delay_us(5);
	if(ack)
		SDA_HIGH;
	else
		SDA_LOW;
	SCK_HIGH;
	delay_us(5);
	SCK_LOW;
	delay_us(5);
}


/* send 1 byte */
void tm1680SendByte(uint8_t dat)
{
	uint8_t a = 0;
	
	for(a = 0; a < 8; a++)
	{
		if(dat & 0x80)
			SDA_HIGH;
		else
			SDA_LOW;
		delay_us(1);
		SCK_HIGH;
		delay_us(1);
		SCK_LOW;
		delay_us(1);
		dat <<= 1;
	}
	ACK();
}


/* send 4bit */
void tm1680Send_4bit(uint8_t dat)
{
	uint8_t b = 0;
	
	for(b = 0; b < 4; b++)
	{
		if(dat & 0x80)
			SDA_HIGH;
		else 
			SDA_LOW;
		delay_us(1);
		SCK_HIGH;
		delay_us(1);
		SCK_LOW;
		dat <<= 1;
	}
}

/* read 1 byte */
uint8_t tm1680Read_1Byte(void)
{
	uint8_t i = 0;
	uint8_t outdata = 0;
	
	SDA_HIGH;
	delay_us(5);
	
	SET_PIN_INPUT;
	
	for(i = 0; i < 8; i++)	
	{
		SCK_LOW;
		delay_us(5);
		if(gpio_input_data_bit_read(SDA_PORT,SDA_PIN))
			outdata |= 0x01;
		else
			outdata &= 0xfe;
		delay_us(5);
		SCK_HIGH;
		delay_us(5);
		outdata <<= 1;
	}
	SCK_LOW;
	
	SET_PIN_OUTPUT;
	return outdata;
}

/*单字节写操作函数
 *写命令函数：开始-ID-ACK-命令-ACK-结束
*/

void TM1680WriteCmd(uint8_t cmd)
{
    tm1680_start();
    tm1680SendByte(TM1680ID); // 写入从机地址  A1、A0都为0
//    ACK();
    tm1680SendByte(cmd);      // 写入命令
//    ACK();
    tm1680_stop();
}



/*写4bit数据
 *开始-id-ack0-内部地址-ack-数据-ack-结束
*/
void TM1680Write4bit(uint8_t addr, uint8_t dat)
{
	tm1680_start();
	tm1680SendByte(TM1680ID); // 写TM1680器件地址
//	ACK();
	tm1680SendByte(addr);     // eeprom内部地址
//	ACK();
	tm1680Send_4bit(dat);      // 写数据
	ACK();
	tm1680_stop();
}

/*写1byte数据
 *开始-id-ack0-内部地址-ack-数据-ack-结束
*/
void TM1680WriteByte(uint8_t addr,uint8_t high_dat,uint8_t low_dat)
{	
	tm1680_start();

	tm1680SendByte(TM1680ID); // 写TM1680器件地址

	tm1680SendByte(addr);     // eeprom内部地址

	tm1680Send_4bit(high_dat);

	tm1680Send_4bit(low_dat);

	ACK();

	tm1680_stop();
}

/* 写一位数码管 */
void Disp_one_person(uint8_t addr, uint8_t dat) 
{
    uint8_t high_data = dat, low_data = dat;
    high_data = high_data & 0xF0;
    low_data = (low_data & 0x0F) << 4;
	TM1680WriteByte(addr, high_data,low_data);
}

 
/* 写页操作 */
void TM1680PageAllWrite(uint8_t faddr, uint8_t dat1, uint8_t dat2, uint8_t cnt)
{
	uint8_t b = 0;
	tm1680_start();

	tm1680SendByte(TM1680ID);

	tm1680SendByte(faddr);

	for (b = 0; b < cnt; b++)
	{
		tm1680SendByte(dat1); // 写数据

		tm1680SendByte(dat2); // 写数据
	}
	tm1680_stop();
}


void TM1680Init(void)
{
	uint8_t i = 0;
	tm1680_start();
	tm1680SendByte(TM1680ID);

	tm1680SendByte(SYSDIS); // 关闭系统时钟和LED循环

	tm1680SendByte(COM8NOMS);

	tm1680SendByte(RCMODE1);

	tm1680SendByte(SYSEN); // 打开系统振荡器

	tm1680SendByte(LEDON); // 打开LED循环

	tm1680SendByte(PWM16); // PWM 16级亮度

	tm1680SendByte(BLINKOFF); // 关闭闪烁

	tm1680_stop();
	delay_ms(10);
	
	TM1680PageAllWrite(0x00, 0x00, 0x00, 48); // 上电清零
}

/* PWM亮度测试 */

void PWMTEST(uint8_t stime) 
{
    uint8_t i = 0;

    for (i = 0; i < 16; i++)
    {
        TM1680WriteCmd(0xb0 | i);
        delay_ms(stime);
        delay_ms(stime);
    }
}


