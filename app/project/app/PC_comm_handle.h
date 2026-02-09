#ifndef __PC_COMM_HANDLE_H
#define __PC_COMM_HANDLE_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "at32f403a_407.h"
#include "at32_Usart.h"

#define PC_HANDLE_TIME   1
#define PC_USART         0
#define PC_DEVECE_ID     0x00
#define LOCAL_DEVECE_ID  0x03
#define PC_RX_BUFF_SIZE  256
#define PC_TX_BUFF_SIZE  256

#define PC_CRC_SIZE      4
#define PC_MAX_SEND_SIZE 22
#define PC_MAX_RECV_SIZE 22

#define PC_CHECK_HEAD(A,B)         ((A==0xD1)&&(B==0xF0)?TRUE:FALSE)

#define PC_HEAD1                0x00
#define PC_CMD1                 0x01
#define PC_CMD2                 0x02
#define PC_ID_H                 0x03
#define PC_ID_L                 0x04
#define PC_DATA_LEN_H           0x05
#define PC_DATA_LEN_L           0x06
#define PC_DATA1_LEN_H          0x07
#define PC_DATA1_LEN_L          0x08
#define PC_DATA2_LEN_H          0x09
#define PC_DATA2_LEN_L          0x0A
#define PC_DATA3_LEN_H          0x0B
#define PC_DATA3_LEN_L          0x0C
#define PC_DATA4_LEN_H          0x0D
#define PC_DATA4_LEN_L          0x0E
#define PC_DATA5_LEN_H          0x0F
#define PC_DATA5_LEN_L          0x10
#define PC_CRC32_1              0x11
#define PC_CRC32_2              0x12
#define PC_CRC32_3              0x13
#define PC_CRC32_4              0x14
#define PC_HEAD2                0x15

typedef enum
{
    PC_HEAD_1 = ((uint8_t)0xD1),
    PC_HEAD_2 = ((uint8_t)0xF0),

} pc_head_e;

typedef enum
{
	PC_GENERAL = 0x01,
	PC_FIRMWARE_UPDATE = 0x02,
    PC_UI_UPDATE = 0x03,
}pc_cmd1_e;

typedef enum
{
	PC_GENERAL_ACK = 0x02,
	PC_GENERAL_READ = 0x05,
	PC_GENERAL_WRITE = 0x06,
	
	PC_FIRMWARE_UPDATE_CONNECT = 0x01, 
	PC_FIRMWARE_UPDATE_ACK     = 0x02,
	PC_FIRMWARE_UPDATE_START   = 0x03,
	PC_FIRMWARE_UPDATE_END     = 0x04,
	
    PC_FIRMWARE_RETURN_OK      = 0x1D,
	PC_FIRMWARE_RETURN_ERROR   = 0x18,
	
}pc_cmd2_e;


typedef enum
{
  CONNECT_PC_EVENT,
  IAP_EVENT,
  PC_END_EVENT,
}pc_event_e;




typedef struct PCObject
{
    bool connect_flag;
    uint8_t rx_buff[PC_RX_BUFF_SIZE];
    uint8_t read_size;
    uint8_t tx_buff[PC_TX_BUFF_SIZE];
    uint32_t check_crc_buff[PC_CRC_SIZE];

} PC_DATA_t;
void pc_comm_handle(void);

#endif


