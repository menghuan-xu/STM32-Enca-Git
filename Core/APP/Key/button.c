#include "button.h"
#include "stm32f1xx_hal_gpio.h"

#define DEBOUNCE_TIME_MS  20   // 消抖时间
#define LONG_PRESS_TIME_MS 500 // 长按阈值

// 内部状态机状态定义
enum {
    KEY_STATE_IDLE = 0,
    KEY_STATE_DEBOUNCE, // 消抖状态
    KEY_STATE_PRESSED,  // 按下状态
    KEY_STATE_RELEASE    // 释放状态
};

static Button_t* g_btn_list = NULL; // 全局按键链表头指针

// 初始化按键实例
void Button_Init(Button_t* btn, GPIO_TypeDef* port, uint16_t pin, uint8_t active_level) {

    btn->port = port;
    btn->pin = pin;
    btn->active_level = active_level;
    btn->state = KEY_STATE_IDLE;
    btn->last_tick = 0;
    btn->press_start_tick = 0;
    btn->long_press_flag = 0;
    btn->next = NULL;
    Button_Register(btn);

}

// 注册按键实例到全局链表
void Button_Register(Button_t* btn) {

    if(!btn) return;
    // 注册到链表头
    btn->next = g_btn_list;
    g_btn_list = btn;
}


// 核心：获取按键事件（需在主循环或周期任务中调用）
BtnEvent_t  Button_GetEvent(Button_t* btn) {
    uint32_t now = HAL_GetTick();
    uint8_t current_level = HAL_GPIO_ReadPin(btn->port, btn->pin); // 读取当前引脚电平
    uint8_t is_pressed = (current_level == btn->active_level) ? 1 : 0;  // 按键是否按下
    BtnEvent_t event = BTN_EVENT_NONE; // 默认无事件

    switch (btn->state) {
        case KEY_STATE_IDLE:
            if (is_pressed) {
                btn->last_tick = now;
                btn->state = KEY_STATE_DEBOUNCE;
            }
            break;

        case KEY_STATE_DEBOUNCE:
            if ((now - btn->last_tick) >= DEBOUNCE_TIME_MS) {
                if (is_pressed) {
                    btn->state = KEY_STATE_PRESSED;
                    btn->press_start_tick = now;
                    btn->long_press_flag = 0;
                } else {
                    btn->state = KEY_STATE_IDLE; // 抖动，退回空闲
                }
            }
            break;

        case KEY_STATE_PRESSED:
            if (!is_pressed) {
                // 检测到松手，进入释放消抖
                btn->state = KEY_STATE_RELEASE;
                btn->last_tick = now;
            } else {
                // 持续按下，检测长按
                if (!btn->long_press_flag && ((now - btn->press_start_tick) >= LONG_PRESS_TIME_MS)) {
                    btn->long_press_flag = 1;
                    event = BTN_EVENT_LONG_PRESS; // 返回长按事件
                }
            }
            break;

        case KEY_STATE_RELEASE:
            if ((now - btn->last_tick) >= DEBOUNCE_TIME_MS) {
                if (!is_pressed) {
                    // 确认释放：如果期间没触发过长按，则判定为短按
                    if (!btn->long_press_flag) {
                        event = BTN_EVENT_CLICK; 
                    }
                    btn->state = KEY_STATE_IDLE;
                } else {
                    btn->state = KEY_STATE_PRESSED; // 释放抖动，退回按下
                }
            }
            break;
    }
    return event;
}
/**
 * @brief  外部中断统一回调函数（所有按键共用）
 * 
 * @details 当外部中断触发时，此函数负责遍历全局按键链表，
 *          找到与触发引脚匹配的按键实例，并更新其状态机。
 *          采用链表遍历方式实现自动分发，无需在回调中显式引用按键实例。
 * 
 * @param GPIO_Pin 触发中断的GPIO引脚（如 GPIO_PIN_12）
 * 
 * @note   此函数应在 HAL_GPIO_EXTI_Callback 中调用，实现中断分发
 * @note   函数名中的 EXIT 应为 EXTI（外部中断），建议更正为 Button_EXTI_Callback
 */
void Button_EXTI_Callback(uint16_t GPIO_Pin){
    uint32_t now = HAL_GetTick();           // 获取当前系统时间戳（ms）
    Button_t* it = g_btn_list;              // 从链表头开始遍历
    
    while(it){                              // 遍历所有已注册的按键
        if(it->pin == GPIO_Pin){            // 找到与中断引脚匹配的按键
            it->last_tick = now;            // 更新状态切换时间戳
            if(it->state == KEY_STATE_IDLE){// 仅在空闲状态时触发消抖
                it->state = KEY_STATE_DEBOUNCE; // 进入消抖状态
            }
        }
        it = it->next;                      // 移动到下一个按键节点
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  Button_EXTI_Callback(GPIO_Pin);
}