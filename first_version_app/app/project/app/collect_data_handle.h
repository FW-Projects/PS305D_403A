
#ifndef _COLLECT_DATA_HANDLE_H
#define _COLLECT_DATA_HANDLE_H


#include "PS305D_handle.h"

#define COLLECT_DATA_TASK_TIME          10

#define DIV_0_6825 (1.0f / 0.6825f)  // 编译时直接算好



void collect_data_handle(void);
uint16_t collect_current(void);
#endif