#ifndef __EC11_H__
#define __EC11_H__

#include "stm32f1xx_hal.h"

typedef struct {
    TIM_HandleTypeDef *htim;
    uint16_t min_value;
    uint16_t max_value;
    uint16_t value;
    uint16_t prev_raw;
}EC11_t;

HAL_StatusTypeDef EC11_Init(EC11_t *ec11, TIM_HandleTypeDef *htim, uint16_t min_value, uint16_t max_value);
uint16_t EC11_GetValue(EC11_t *ec11);
void EC11_SetValue(EC11_t *ec11, uint16_t value);


#endif