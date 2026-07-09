#ifndef __FONT_H
#define __FONT_H

#include <stdint.h>

// ===================== 字体配置 =====================

// 字体大小枚举
typedef enum {
    FONT_8X8     // 8x8 点阵字体
} FontSize;

// ===================== 字体数据声明 =====================

// 8x8 ASCII 字体数据 (ASCII: 0x20-0x7E, 共95个字符)
extern const uint8_t Font8x8[95][8];

// ===================== 函数声明 =====================
/**
 * @brief 获取字符在字体数组中的索引
 * @param ch: ASCII 字符
 * @return 字符索引（-1 表示字符不在字体范围内）
 */
int8_t Font_GetCharIndex(uint8_t ch);






#endif /* __FONT_H */