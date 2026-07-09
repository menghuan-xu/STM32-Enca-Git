#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "i2c.h"  
#include <stdint.h>
#include <string.h>
#include "font.h"
#include "image.h"

// ===================== 配置 =====================
#define OLED_I2C_HANDLE    hi2c1    // 硬件I2C1
#define OLED_ADDR          0x78    // SSD1306 I2C地址(最常用)
#define OLED_WIDTH         128     // 屏宽
#define OLED_HEIGHT        64      // 屏高

// ===================== 函数声明 =====================
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Update(void);                           // 刷新显存到屏幕
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color); // 画点
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color); // 画线
void OLED_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);    // 画矩形
void OLED_DrawFillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);    // 填充矩形
void OLED_DrawFillRoundedRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color); // 绘制带圆角的填充矩形
void OLED_DrawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);           // 画圆

void OLED_DrawChar(uint8_t x, uint8_t y, char ch, FontSize font, uint8_t color);
void OLED_DrawString(uint8_t x, uint8_t y, const char *str, FontSize font, uint8_t color);

// ===================== 图片显示相关函数 =====================
void OLED_DrawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *img_data);
void OLED_DrawImagePixels(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *img_data);

// ===================== 局部刷新相关函数 =====================
void OLED_UpdatePartial(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void OLED_ClearPartial(uint8_t x, uint8_t y, uint8_t w, uint8_t h);



#endif