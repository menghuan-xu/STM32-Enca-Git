#ifndef __BUTTON_H
#define __BUTTON_H


#include "main.h"




// 1. 定义按键事件枚举
typedef enum {
    BTN_EVENT_NONE = 0,     // 无事件
    BTN_EVENT_CLICK,        // 短按（单击）事件
    BTN_EVENT_LONG_PRESS,   // 长按事件
} BtnEvent_t;

typedef struct Button_t Button_t;
// 2. 定义按键对象结构体（OOP 实例）
struct Button_t{
    // 硬件引脚
    GPIO_TypeDef* port;
    uint16_t pin;
    uint8_t active_level;      // 有效电平 (0:低电平按下, 1:高电平按下)
    
    // 内部状态（对外隐藏）
    uint8_t state;
    uint32_t last_tick;        // 状态切换时间戳
    uint32_t press_start_tick; // 按下起始时间戳
    uint8_t long_press_flag;   // 长按是否已触发标志（防止长按期间重复触发短按）
    Button_t* next;            // 链表节点，用于全局注册按键实例
};

// 3. 公共接口
void Button_Init(Button_t* btn, GPIO_TypeDef* port, uint16_t pin, uint8_t active_level);
void Button_Register(Button_t* btn);    // 注册按键实例到全局链表
BtnEvent_t Button_GetEvent(Button_t* btn); // 核心：获取按键事件
void Button_EXIT_Callback(uint16_t GPIO_Pin); //统一外部中断回调 自动分发

#endif