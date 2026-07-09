#include "ec11.h"
#include "stm32f1xx_hal_tim.h"

HAL_StatusTypeDef EC11_Init(EC11_t *ec11, TIM_HandleTypeDef *htim, uint16_t min_value, uint16_t max_value)
{
    ec11->htim = htim;
    ec11->min_value = min_value;
    ec11->max_value = max_value;
    ec11->value = min_value;
    ec11->prev_raw = __HAL_TIM_GET_COUNTER(htim);
    return HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
}

uint16_t EC11_GetValue(EC11_t *ec11)
{
    uint16_t raw = __HAL_TIM_GET_COUNTER(ec11->htim);
    int32_t diff = (int32_t)raw - (int32_t)ec11->prev_raw;
    if(diff > 32767) {
        diff -= 65536;
    } else if(diff < -32767) {
        diff += 65536;
    }
    if(diff != 0){
        int32_t range = (int32_t)ec11->max_value - (int32_t)ec11->min_value + 1;
        int32_t new_value = (int32_t)ec11->value + diff;
        new_value -= ec11->min_value;
        new_value %= range;
        if(new_value < 0) {
            new_value += range;
        }
        new_value += ec11->min_value;
        ec11->value = (uint16_t)new_value;
        ec11->prev_raw = raw;
    }
    return ec11->value;
}

void EC11_SetValue(EC11_t *ec11, uint16_t value)
{
    if(value < ec11->min_value) {
        value = ec11->min_value;
    } else if(value > ec11->max_value) {
        value = ec11->max_value;
    }
    ec11->value = value;
    ec11->prev_raw = __HAL_TIM_GET_COUNTER(ec11->htim);
}