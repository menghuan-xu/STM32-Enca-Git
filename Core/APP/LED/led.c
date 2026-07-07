#include "led.h"

// 内部函数：实现点亮
static void led_on(LED_Object *me){
    if(me->ActiveLevel == LED_ACTIVE_LOW){
        HAL_GPIO_WritePin(me->GPIOx, me->Pin, GPIO_PIN_RESET);
    }
    else{
        HAL_GPIO_WritePin(me->GPIOx, me->Pin, GPIO_PIN_SET);
    }
}



// 内部函数：实现熄灭
static void led_off(LED_Object *me){
    if(me->ActiveLevel == LED_ACTIVE_LOW){
        HAL_GPIO_WritePin(me->GPIOx, me->Pin, GPIO_PIN_SET);
    }
    else{
        HAL_GPIO_WritePin(me->GPIOx, me->Pin, GPIO_PIN_RESET);
    }
}

// 内部函数：实现切换状态
static void led_toggle(LED_Object *me){
    HAL_GPIO_TogglePin(me->GPIOx, me->Pin);
}

// 内部函数：实现获取状态
static uint8_t led_get_state(LED_Object *me){
    return HAL_GPIO_ReadPin(me->GPIOx, me->Pin);
}

void LED_Init(LED_Object *me, GPIO_TypeDef *GPIOx, uint16_t Pin, LED_ActiveLevel ActiveLevel){

    // 保存led属性
    me->GPIOx = GPIOx;
    me->Pin = Pin;
    me->ActiveLevel = ActiveLevel;
    
    //绑定方法（函数指针）
    me->on = led_on;
    me->off = led_off;
    me->toggle = led_toggle;
    me->get_state = led_get_state;

    // 使能GPIO时钟
    if(GPIOx == GPIOA)  __HAL_RCC_GPIOA_CLK_ENABLE();
    if(GPIOx == GPIOB)  __HAL_RCC_GPIOB_CLK_ENABLE();
    if(GPIOx == GPIOC)  __HAL_RCC_GPIOC_CLK_ENABLE();
    if(GPIOx == GPIOD)  __HAL_RCC_GPIOD_CLK_ENABLE();

    // 初始化GPIO引脚
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

    //默认关闭
    me->off(me);
}
