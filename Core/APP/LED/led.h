#ifndef __LED_H
#define __LED_H


#include "stm32f1xx_hal.h"



// 电平类型配置
typedef enum {
    LED_ACTIVE_LOW  = 0,  // 低电平点亮
    LED_ACTIVE_HIGH = 1   // 高电平点亮
} LED_ActiveLevel;


typedef struct LED_Object LED_Object;
struct LED_Object {
    // 成员变量
    GPIO_TypeDef* GPIOx;
    uint16_t Pin;
    LED_ActiveLevel ActiveLevel;

    // 成员函数
    void (*on)(LED_Object* me);
    void (*off)(LED_Object* me);
    void (*toggle)(LED_Object* me);
    uint8_t (*get_state)(LED_Object* me);
};

void LED_Init(
    LED_Object* me, 
    GPIO_TypeDef* GPIOx,
    uint16_t Pin, 
    LED_ActiveLevel ActiveLevel
);


#endif



