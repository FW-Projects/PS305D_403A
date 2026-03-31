
#ifndef _COLLECT_DATA_HANDLE_H
#define _COLLECT_DATA_HANDLE_H


#include "PS305D_handle.h"

#define COLLECT_DATA_TASK_TIME          100

#define OFFSET_VOLTAGE 1



void collect_data_handle(void);
uint16_t collect_current(void);
#endif