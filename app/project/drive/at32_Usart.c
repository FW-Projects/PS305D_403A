#include "at32_usart.h"
#include "util_queue.h"
#include "userconfig.h"

#define USART_RXFLAG_IDLE 	0
#define USART_RXFLAG_BUSY 	1
#define USART_RXFLAG_FINISH 2
#define USART_TXFLAG_IDLE 	0
#define USART_TXFLAG_BUSY 	1
#define USART_DELAYTIME 4

typedef struct
{
    util_queue_t tRXQueue;
    uint8_t chRXFinishTime;
    uint8_t chRXFlag;

    util_queue_t tTXQueue;
    uint8_t chTXFinishTime;
    uint8_t chTXFlag;

    usart_type *ptUsart;
} usartbuffer_t;

usartbuffer_t tUsart1Buffer;
usartbuffer_t tUsart2Buffer;
usartbuffer_t tUsart3Buffer;
usartbuffer_t tUart4Buffer;
usartbuffer_t tUart5Buffer;
#if (defined USART1_ENABLE) && (USART1_ENABLE == TRUE)
#define USART1_RX_BUFFER_MAX			1024
#define USART1_TX_BUFFER_MAX			1024
uint8_t chUsart1TxBuffer[USART1_TX_BUFFER_MAX];
uint8_t chUsart1RxBuffer[USART1_TX_BUFFER_MAX];
#endif

#if (defined USART2_ENABLE) && (USART2_ENABLE == TRUE)
#define USART2_RX_BUFFER_MAX			1024
#define USART2_TX_BUFFER_MAX			1024
uint8_t chUsart2TxBuffer[USART2_TX_BUFFER_MAX];
uint8_t chUsart2RxBuffer[USART2_TX_BUFFER_MAX];
#endif

#if (defined USART3_ENABLE) && (USART3_ENABLE == TRUE)
#define USART3_RX_BUFFER_MAX			1024
#define USART3_TX_BUFFER_MAX			1024
uint8_t chUsart3TxBuffer[USART3_TX_BUFFER_MAX];
uint8_t chUsart3RxBuffer[USART3_TX_BUFFER_MAX];
#endif



#if (defined UART4_ENABLE) && (UART4_ENABLE == TRUE)
#define UART4_RX_BUFFER_MAX			1024
#define UART4_TX_BUFFER_MAX			1024
uint8_t chUart4TxBuffer[UART4_TX_BUFFER_MAX];
uint8_t chUart4RxBuffer[UART4_TX_BUFFER_MAX];
#endif


#if (defined UART5_ENABLE) && (UART5_ENABLE == TRUE)
#define UART5_RX_BUFFER_MAX			1024
#define UART5_TX_BUFFER_MAX			1024
uint8_t chUart5TxBuffer[UART5_TX_BUFFER_MAX];
uint8_t chUart5RxBuffer[UART5_TX_BUFFER_MAX];
#endif







#if USART1_RS485_ENABLE
#define USART1_ENABLE_PORT 		GPIOA
#define USART1_ENABLE_PIN 		GPIO_Pins_12
#define USART1_SEND_ENABLE 		gpio_bits_set(USART1_ENABLE_PORT, USART1_ENABLE_PIN)
#define USART1_RECEIVE_ENABLE   gpio_bits_reset(USART1_ENABLE_PORT, USART1_ENABLE_PIN)
#endif

void BSP_UsartInit(void)
{
#if (defined USART1_ENABLE)
    queue_init((util_queue_t *)&tUsart1Buffer.tRXQueue, chUsart1RxBuffer, USART1_RX_BUFFER_MAX);
    queue_init((util_queue_t *)&tUsart1Buffer.tTXQueue, chUsart1TxBuffer, USART1_TX_BUFFER_MAX);
    tUsart1Buffer.ptUsart = USART1;
#if USART1_RS485_ENABLE
    UART485_ENR(1);
#endif
#endif
#if (defined USART2_ENABLE) && (USART2_ENABLE == TRUE)
    queue_init((util_queue_t *)&tUsart2Buffer.tRXQueue, chUsart2RxBuffer, USART2_RX_BUFFER_MAX);
    queue_init((util_queue_t *)&tUsart2Buffer.tTXQueue, chUsart2TxBuffer, USART2_TX_BUFFER_MAX);
    tUsart2Buffer.ptUsart = USART2;
#if USART2_RS485_ENABLE
    UART485_ENR(1);
#endif
#endif
#if (defined USART3_ENABLE) && (USART3_ENABLE == TRUE)
    queue_init((util_queue_t *)&tUsart3Buffer.tRXQueue, chUsart3RxBuffer, USART3_RX_BUFFER_MAX);
    queue_init((util_queue_t *)&tUsart3Buffer.tTXQueue, chUsart3RxBuffer, USART3_TX_BUFFER_MAX);
    tUsart3Buffer.ptUsart = USART3;
#if USART3_RS485_ENABLE
    UART485_ENR(1);
#endif
#endif
#if (defined UART4_ENABLE) && (UART4_ENABLE == TRUE)
    queue_init((util_queue_t *)&tUart4Buffer.tRXQueue, chUart4RxBuffer, UART4_RX_BUFFER_MAX);
    queue_init((util_queue_t *)&tUart4Buffer.tTXQueue, chUart4RxBuffer, UART4_TX_BUFFER_MAX);
    tUart4Buffer.ptUsart = UART4;
#if UART4_RS485_ENABLE
    UART485_ENR(1);
#endif
#endif
#if (defined UART5_ENABLE) && (UART5_ENABLE == TRUE)
    queue_init((util_queue_t *)&tUart5Buffer.tRXQueue, chUart5RxBuffer, UART5_RX_BUFFER_MAX);
    queue_init((util_queue_t *)&tUart5Buffer.tTXQueue, chUart5RxBuffer, UART5_TX_BUFFER_MAX);
    tUart5Buffer.ptUsart = UART5;
#if UART5_RS485_ENABLE
    UART485_ENR(1);
#endif
#endif
}

uint16_t usart_sendData(uint8_t chUsartNum, uint8_t *pchSendData, uint16_t hwLength)
{
    usartbuffer_t *ptUsartBuffer;
    uint16_t ret = 0;
    uint16_t hwCounter = 0;
    uint8_t chData;

    if (chUsartNum == 0)
    {
        ptUsartBuffer = (usartbuffer_t *)&tUsart1Buffer;
    }
    else if (chUsartNum == 1)
    {
        ptUsartBuffer = (usartbuffer_t *)&tUsart2Buffer;
    }
    else if (chUsartNum == 2)
    {
        ptUsartBuffer = (usartbuffer_t *)&tUsart3Buffer;
    }
    else if (chUsartNum == 3)
    {
        ptUsartBuffer = (usartbuffer_t *)&tUart4Buffer;
    }
    else if (chUsartNum == 4)
    {
        ptUsartBuffer = (usartbuffer_t *)&tUart5Buffer;
    }

    for (hwCounter = 0; hwCounter < hwLength; hwCounter++)
    {
        if (queue_write((util_queue_t *)&ptUsartBuffer->tTXQueue, *(pchSendData + hwCounter)) != QUEUE_OK)
        {
            return hwCounter;
        }
    }

    if (ptUsartBuffer->chTXFlag == USART_TXFLAG_BUSY)
    {
        return hwCounter;
    }

#if USART1_RS485_ENABLE
    UART485_ENR(0);
#endif
    queue_read((util_queue_t *)&ptUsartBuffer->tTXQueue, (uint8_t *)&chData);
    usart_data_transmit(ptUsartBuffer->ptUsart, chData);
    ptUsartBuffer->chTXFlag = USART_TXFLAG_BUSY;
    return hwCounter;
}


uint16_t usart_receiveData(uint8_t chUsartNum, uint8_t *pchReceiveData)
{
    uint16_t hwCounter = 0;
    usartbuffer_t *ptUsartBuffer;
    qstatus_t tQueueStatus;

    if (chUsartNum == 0)
    {
        ptUsartBuffer = (usartbuffer_t *)&tUsart1Buffer;
    }
    else if (chUsartNum == 1)
    {
        ptUsartBuffer = (usartbuffer_t *)&tUsart2Buffer;
    }
    else if (chUsartNum == 2)
    {
        ptUsartBuffer = (usartbuffer_t *)&tUsart3Buffer;
    }
    else if (chUsartNum == 3)
    {
        ptUsartBuffer = (usartbuffer_t *)&tUart4Buffer;
    }
    else if (chUsartNum == 4)
    {
        ptUsartBuffer = (usartbuffer_t *)&tUart5Buffer;
    }

    if (ptUsartBuffer->chRXFlag != USART_RXFLAG_FINISH)
    {
        return hwCounter;
    }

    do
    {
        tQueueStatus = queue_read((util_queue_t *)&ptUsartBuffer->tRXQueue, pchReceiveData);

        if (QUEUE_OK == tQueueStatus)
        {
            hwCounter++;
            pchReceiveData++;
        }
    }
    while (tQueueStatus != QUEUE_EMPTY);

    ptUsartBuffer->chRXFlag = USART_RXFLAG_IDLE;
    return hwCounter;
}

#ifdef USART1_ENABLE
void USART1_TimeOutCounter(void)
{
    if (tUsart1Buffer.chRXFlag == USART_RXFLAG_BUSY)
    {
        if (tUsart1Buffer.chRXFinishTime)
        {
            tUsart1Buffer.chRXFinishTime--;
        }
        else
        {
            tUsart1Buffer.chRXFlag = USART_RXFLAG_FINISH;
        }
    }

    if (tUsart1Buffer.chTXFinishTime)
    {
        tUsart1Buffer.chTXFinishTime--;
    }
}

#endif
#ifdef USART2_ENABLE
void USART2_TimeOutCounter(void)
{
    if (tUsart2Buffer.chRXFlag == USART_RXFLAG_BUSY)
    {
        if (tUsart2Buffer.chRXFinishTime)
        {
            tUsart2Buffer.chRXFinishTime--;
        }
        else
        {
            tUsart2Buffer.chRXFlag = USART_RXFLAG_FINISH;
        }
    }

    if (tUsart2Buffer.chTXFinishTime)
    {
        tUsart2Buffer.chTXFinishTime--;
    }
}

#endif
#ifdef USART3_ENABLE
void USART3_TimeOutCounter(void)
{
    if (tUsart3Buffer.chRXFlag == USART_RXFLAG_BUSY)
    {
        if (tUsart3Buffer.chRXFinishTime)
        {
            tUsart3Buffer.chRXFinishTime--;
        }
        else
        {
            tUsart3Buffer.chRXFlag = USART_RXFLAG_FINISH;
        }
    }

    if (tUsart3Buffer.chTXFinishTime)
    {
        tUsart3Buffer.chTXFinishTime--;
    }
}

#endif

#ifdef UART4_ENABLE
void UART4_TimeOutCounter(void)
{
    if (tUart4Buffer.chRXFlag == USART_RXFLAG_BUSY)
    {
        if (tUart4Buffer.chRXFinishTime)
        {
            tUart4Buffer.chRXFinishTime--;
        }
        else
        {
            tUart4Buffer.chRXFlag = USART_RXFLAG_FINISH;
        }
    }

    if (tUart4Buffer.chTXFinishTime)
    {
        tUart4Buffer.chTXFinishTime--;
    }
}

#endif

#ifdef UART5_ENABLE
void UART5_TimeOutCounter(void)
{
    if (tUart5Buffer.chRXFlag == USART_RXFLAG_BUSY)
    {
        if (tUart5Buffer.chRXFinishTime)
        {
            tUart5Buffer.chRXFinishTime--;
        }
        else
        {
            tUart5Buffer.chRXFlag = USART_RXFLAG_FINISH;
        }
    }

    if (tUart5Buffer.chTXFinishTime)
    {
        tUart5Buffer.chTXFinishTime--;
    }
}

#endif

#ifdef USART1_ENABLE
void USART1_IRQHandler(void)
{
    uint8_t chData;

    if (USART1->ctrl1_bit.rdbfien != RESET)
    {
        if (usart_flag_get(USART1, USART_RDBF_FLAG) != RESET)
        {
            queue_write((util_queue_t *)&tUsart1Buffer.tRXQueue, usart_data_receive(USART1));
            tUsart1Buffer.chRXFinishTime = USART_DELAYTIME;
            tUsart1Buffer.chRXFlag = USART_RXFLAG_BUSY;
        }
    }

    if (USART1->ctrl1_bit.tdcien != RESET)
    {
        if (usart_flag_get(USART1, USART_TDC_FLAG) != RESET)
        {
            usart_flag_clear(USART1, USART_TDC_FLAG);

            if (queue_read((util_queue_t *)&tUsart1Buffer.tTXQueue, (uint8_t *)&chData) == QUEUE_OK)
            {
                usart_data_transmit(USART1, chData);
            }
            else
            {
                tUsart1Buffer.chTXFlag = USART_TXFLAG_IDLE;
#if USART1_RS485_ENABLE
                UART485_ENR(1);
#endif
            }
        }
    }
}

#endif

#ifdef USART2_ENABLE
void USART2_IRQHandler(void)
{
    uint8_t chData;

    if (USART2->ctrl1_bit.rdbfien != RESET)
    {
        if (usart_flag_get(USART2, USART_RDBF_FLAG) != RESET)
        {
            queue_write((util_queue_t *)&tUsart2Buffer.tRXQueue, usart_data_receive(USART2));
            tUsart2Buffer.chRXFinishTime = USART_DELAYTIME;
            tUsart2Buffer.chRXFlag = USART_RXFLAG_BUSY;
        }
    }

    if (USART2->ctrl1_bit.tdcien != RESET)
    {
        if (usart_flag_get(USART2, USART_TDC_FLAG) != RESET)
        {
            usart_flag_clear(USART2, USART_TDC_FLAG);

            if (queue_read((util_queue_t *)&tUsart2Buffer.tTXQueue, (uint8_t *)&chData) == QUEUE_OK)
            {
                usart_data_transmit(USART2, chData);
            }
            else
            {
                tUsart2Buffer.chTXFlag = USART_TXFLAG_IDLE;
            }
        }
    }
}

#endif

#ifdef USART3_ENABLE
void USART3_IRQHandler(void)
{
    uint8_t chData;

    if (USART3->ctrl1_bit.rdbfien != RESET)
    {
        if (usart_flag_get(USART3, USART_RDBF_FLAG) != RESET)
        {
            usart_flag_clear(USART3, USART_RDBF_FLAG);
            queue_write((util_queue_t *)&tUsart3Buffer.tRXQueue, usart_data_receive(USART3));
            tUsart3Buffer.chRXFinishTime = USART_DELAYTIME;
            tUsart3Buffer.chRXFlag = USART_RXFLAG_BUSY;
        }
    }

    if (USART3->ctrl1_bit.tdcien != RESET)
    {
        if (usart_flag_get(USART3, USART_TDC_FLAG) != RESET)
        {
            usart_flag_clear(USART3, USART_TDC_FLAG);

            if (queue_read((util_queue_t *)&tUsart3Buffer.tTXQueue, (uint8_t *)&chData) == QUEUE_OK)
            {
                usart_data_transmit(USART3, chData);
            }
            else
            {
                tUsart3Buffer.chTXFlag = USART_TXFLAG_IDLE;
            }
        }
    }
}

#endif

#ifdef UART4_ENABLE
void UART4_IRQHandler(void)
{
    uint8_t chData;

    if (UART4->ctrl1_bit.rdbfien != RESET)
    {
        if (usart_flag_get(UART4, USART_RDBF_FLAG) != RESET)
        {
            usart_flag_clear(UART4, USART_RDBF_FLAG);
            queue_write((util_queue_t *)&tUart4Buffer.tRXQueue, usart_data_receive(UART4));
            tUart4Buffer.chRXFinishTime = USART_DELAYTIME;
            tUart4Buffer.chRXFlag = USART_RXFLAG_BUSY;
        }
    }

    if (UART4->ctrl1_bit.tdcien != RESET)
    {
        if (usart_flag_get(UART4, USART_TDC_FLAG) != RESET)
        {
            usart_flag_clear(UART4, USART_TDC_FLAG);

            if (queue_read((util_queue_t *)&tUart4Buffer.tTXQueue, (uint8_t *)&chData) == QUEUE_OK)
            {
                usart_data_transmit(UART4, chData);
            }
            else
            {
                tUart4Buffer.chTXFlag = USART_TXFLAG_IDLE;
            }
        }
    }
}

#endif

#ifdef UART5_ENABLE
void UART5_IRQHandler(void)
{
    uint8_t chData;

    if (UART5->ctrl1_bit.rdbfien != RESET)
    {
        if (usart_flag_get(UART5, USART_RDBF_FLAG) != RESET)
        {
            usart_flag_clear(UART5, USART_RDBF_FLAG);
            queue_write((util_queue_t *)&tUart5Buffer.tRXQueue, usart_data_receive(UART5));
            tUart5Buffer.chRXFinishTime = USART_DELAYTIME;
            tUart5Buffer.chRXFlag = USART_RXFLAG_BUSY;
        }
    }

    if (UART5->ctrl1_bit.tdcien != RESET)
    {
        if (usart_flag_get(UART5, USART_TDC_FLAG) != RESET)
        {
            usart_flag_clear(UART5, USART_TDC_FLAG);

            if (queue_read((util_queue_t *)&tUart5Buffer.tTXQueue, (uint8_t *)&chData) == QUEUE_OK)
            {
                usart_data_transmit(UART5, chData);
            }
            else
            {
                tUart5Buffer.chTXFlag = USART_TXFLAG_IDLE;
            }
        }
    }
}

#endif

uint32_t convert_data(const uint8_t *input_array, uint32_t *output_array,
                      uint32_t start_index, uint32_t end_index)
{
    if (input_array == NULL || output_array == NULL)
    {
        return 0;
    }

    if (start_index > end_index)
    {
        return 0;
    }

    uint32_t byte_count = end_index - start_index + 1;
    const uint8_t *ptr = &input_array[start_index];
    uint32_t i;
    uint32_t output_index = 0;

    for (i = 0; i < byte_count / 4; i++)
    {
        output_array[output_index++] = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
        ptr += 4;
    }

    if (byte_count % 4 != 0)
    {
        uint32_t word = 0;
        uint8_t remaining = byte_count % 4;

        for (uint8_t j = 0; j < remaining; j++)
        {
            word |= ptr[j] << (24 - (j * 8));
        }

        output_array[output_index++] = word;
    }

    return output_index;
}
