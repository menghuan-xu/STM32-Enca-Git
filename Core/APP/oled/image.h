#ifndef __IMAGE_H__
#define __IMAGE_H__
#include "stdint.h"



// 动图帧数据（64x64 位图，每帧512字节）
extern const uint8_t anim_frames[10][512];
extern const uint8_t cat_frames[10][512];

// 小图标（像素数组格式）
extern const uint8_t small_icon_pixels[];

#endif /* __IMAGE_H__ */