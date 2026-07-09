#include "oled.h"

// 显存：128*64 = 8192bit = 1024字节
static uint8_t OLED_GRAM[128][8];

/*****************************************************************
 * 写命令
 *****************************************************************/
static void OLED_WriteCmd(uint8_t cmd)
{
    uint8_t buf[2] = {0x00, cmd};
    HAL_I2C_Master_Transmit(&OLED_I2C_HANDLE, OLED_ADDR, buf, 2, 100);
}

/*****************************************************************
 * 写数据
 *****************************************************************/
static void OLED_WriteData(uint8_t data)
{
    uint8_t buf[2] = {0x40, data};
    HAL_I2C_Master_Transmit(&OLED_I2C_HANDLE, OLED_ADDR, buf, 2, 100);
}

/*****************************************************************
 * OLED 初始化
 *****************************************************************/
void OLED_Init(void)
{
    OLED_WriteCmd(0xAE);    // 关闭显示
    OLED_WriteCmd(0xD5);    // 时钟分频
    OLED_WriteCmd(0x80);
    OLED_WriteCmd(0xA8);    // 路数
    OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xDC);    // 显示起始行
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xD3);    // 显示偏移
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0x40);    // 起始行
    OLED_WriteCmd(0x8D);    // 电荷泵
    OLED_WriteCmd(0x14);
    OLED_WriteCmd(0x20);    // 内存地址模式
    OLED_WriteCmd(0x02);
    OLED_WriteCmd(0xA1);    // 段重定义
    OLED_WriteCmd(0xC8);    // 行扫描方向
    OLED_WriteCmd(0xDA);    // 配置COM引脚
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0x81);    // 对比度
    OLED_WriteCmd(0xCF);
    OLED_WriteCmd(0xD9);    // 预充电周期
    OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDB);    // VCOM取消电平
    OLED_WriteCmd(0x30);
    OLED_WriteCmd(0xA4);    // 全局显示开启
    OLED_WriteCmd(0xA6);    // 正常显示
    OLED_WriteCmd(0xAF);    // 开启显示

    OLED_Clear();
    OLED_Update();
}

/*****************************************************************
 * 清空显存
 *****************************************************************/
void OLED_Clear(void)
{
    memset(OLED_GRAM, 0x00, sizeof(OLED_GRAM));
}

/*****************************************************************
 * 将显存刷新到屏幕
 *****************************************************************/
void OLED_Update(void)
{

    uint8_t page, col;

    for(page = 0; page < 8; page++)
    {
        OLED_WriteCmd(0xB0 + page);
        OLED_WriteCmd(0x00);
        OLED_WriteCmd(0x10);

        for(col = 0; col < 128; col++)
        {
            OLED_WriteData(OLED_GRAM[col][page]);
        }
    }

}

/*****************************************************************
 * 画点 (color=1 亮 / 0 灭)
 *****************************************************************/
/*****************************************************************
 * 功能：在OLED显示屏指定位置绘制一个像素点
 * 参数：
 *   x     - 像素点的横坐标（0 ~ OLED_WIDTH-1）
 *   y     - 像素点的纵坐标（0 ~ OLED_HEIGHT-1）
 *   color - 像素颜色：0表示熄灭（黑），非0表示点亮（白）
 * 返回值：无
 * 说明：
 *   OLED显存采用位映射方式存储，每字节存储8个垂直像素
 *   y/8 计算该像素所在的字节行
 *   y%8 计算该像素在字节内的位位置
 *****************************************************************/
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    // 边界检查：坐标超出屏幕范围则直接返回
    if(x >= OLED_WIDTH || y >= OLED_HEIGHT) return;

    if(color)
        // 点亮像素：将对应位置1
        OLED_GRAM[x][y/8] |= 1 << (y%8);
    else
        // 熄灭像素：将对应位置0
        OLED_GRAM[x][y/8] &= ~(1 << (y%8));
}

/*****************************************************************
 * 画线 (Bresenham算法)
 *****************************************************************/
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int ux = ((dx > 0) << 1) - 1;
    int uy = ((dy > 0) << 1) - 1;
    int x = x1, y = y1;
    int eps = 0;

    dx = dx < 0 ? -dx : dx;
    dy = dy < 0 ? -dy : dy;

    if(dx > dy)
    {
        for(x = x1; x != x2; x += ux)
        {
            OLED_DrawPixel(x, y, color);
            eps += dy;
            if((eps << 1) >= dx)
            {
                y += uy;
                eps -= dx;
            }
        }
    }
    else
    {
        for(y = y1; y != y2; y += uy)
        {
            OLED_DrawPixel(x, y, color);
            eps += dx;
            if((eps << 1) >= dy)
            {
                x += ux;
                eps -= dy;
            }
        }
    }
    OLED_DrawPixel(x2, y2, color);
}

/*****************************************************************
 * 画空心矩形
 *****************************************************************/
void OLED_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
    OLED_DrawLine(x, y, x+w, y, color);
    OLED_DrawLine(x, y+h, x+w, y+h, color);
    OLED_DrawLine(x, y, x, y+h, color);
    OLED_DrawLine(x+w, y, x+w, y+h, color);
}

/*****************************************************************
 * 画填充矩形
 *****************************************************************/
void OLED_DrawFillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
    uint8_t i, j;
    for(i = 0; i < h; i++)
    {
        for(j = 0; j < w; j++)
        {
            OLED_DrawPixel(x+j, y+i, color);
        }
    }
}

/*****************************************************************
 * 绘制带圆角的填充矩形（向外凸的圆角）
 * x,y: 矩形左上角坐标 | w: 宽度 | h: 高度 | r: 圆角半径 | color: 1亮/0灭
 * 说明：圆角是向外凸起的，以矩形顶点为圆心绘制四分之一圆
 *****************************************************************/
void OLED_DrawFillRoundedRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color)
{
    if(w == 0 || h == 0) return;
    
    // 圆角半径不能超过宽/高的一半
    if(r > w / 2) r = w / 2;
    if(r > h / 2) r = h / 2;

    // 无圆角时直接绘制普通矩形
    if(r == 0)
    {
        OLED_DrawFillRect(x, y, w, h, color);
        return;
    }

    uint8_t inner_w = w - 2 * r;
    uint8_t inner_h = h - 2 * r;

    // 1. 绘制矩形主体（中间矩形部分）
    if(inner_w > 0 && inner_h > 0)
    {
        OLED_DrawFillRect(x + r, y + r, inner_w, inner_h, color);
    }

    // 2. 绘制四条边（圆角之间的连接部分）
    // 上边（不包括圆角部分）
    if(inner_w > 0)
    {
        OLED_DrawFillRect(x + r, y, inner_w, r, color);
    }
    // 下边（不包括圆角部分）
    if(inner_w > 0)
    {
        OLED_DrawFillRect(x + r, y + h - r, inner_w, r, color);
    }
    // 左边（不包括圆角部分）
    if(inner_h > 0)
    {
        OLED_DrawFillRect(x, y + r, r, inner_h, color);
    }
    // 右边（不包括圆角部分）
    if(inner_h > 0)
    {
        OLED_DrawFillRect(x + w - r, y + r, r, inner_h, color);
    }

    // 3. 绘制四个向外凸的圆角（四分之一圆）
    // 使用圆方程 x² + y² <= r² 判断点是否在圆内
    for(uint8_t dy = 0; dy <= r; dy++)
    {
        for(uint8_t dx = 0; dx <= r; dx++)
        {
            int16_t dist = (int16_t)dx * dx + (int16_t)dy * dy;
            if(dist <= (int16_t)r * r)
            {
                // 左上角圆角（以(x+r, y+r)为圆心）
                OLED_DrawPixel(x + r - dx, y + r - dy, color);
                // 右上角圆角（以(x+w-1-r, y+r)为圆心）
                OLED_DrawPixel(x + w - 1 - r + dx, y + r - dy, color);
                // 左下角圆角（以(x+r, y+h-1-r)为圆心）
                OLED_DrawPixel(x + r - dx, y + h - 1 - r + dy, color);
                // 右下角圆角（以(x+w-1-r, y+h-1-r)为圆心）
                OLED_DrawPixel(x + w - 1 - r + dx, y + h - 1 - r + dy, color);
            }
        }
    }
}

/*****************************************************************
 * 画圆 (中点圆算法)
 *****************************************************************/
void OLED_DrawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color)
{
    int x = -r, y = 0, err = 2 - 2*r;
    do
    {
        OLED_DrawPixel(x0-x, y0+y, color);
        OLED_DrawPixel(x0+x, y0+y, color);
        OLED_DrawPixel(x0+x, y0-y, color);
        OLED_DrawPixel(x0-x, y0-y, color);
        r = err;
        if(r <= y) err += ++y*2+1;
        if(r > x || err > y) err += ++x*2+1;
    } while(x < 0);
}



void OLED_DrawChar(uint8_t x, uint8_t y, char ch, FontSize font, uint8_t color)
{
    int8_t index = Font_GetCharIndex((uint8_t)ch);
    if (index < 0) return; // 字符不在字体范围内
    
    uint8_t i, j;
    
    // 只保留8x8字体绘制，字体数据按行存储
    // 如果字库是 LSB 作为最左侧像素，则按位0到位7绘制
    for (i = 0; i < 8; i++) {
        uint8_t row_data = Font8x8[index][i];
        for (j = 0; j < 8; j++) {
            if (row_data & (1 << j)) {
                OLED_DrawPixel(x + j, y + i, color);
            }
        }
    }
}

/*****************************************************************
 * 在OLED显示屏上显示字符串
 *****************************************************************/
void OLED_DrawString(uint8_t x, uint8_t y, const char *str, FontSize font, uint8_t color)
{
    if (str == NULL) return;
    
    uint8_t font_width = 8;  // 8x8字体宽度
    uint8_t font_height = 8; // 8x8字体高度
    
    while (*str) {
        // 检查是否需要换行
        if (x + font_width > OLED_WIDTH) {
            x = 0;
            y += font_height;
            if (y + font_height > OLED_HEIGHT) {
                break; // 超出屏幕范围
            }
        }
        
        OLED_DrawChar(x, y, *str, font, color);
        x += font_width;
        str++;
    }
}

/*****************************************************************
 * 显示图片（位图格式）
 * x, y: 图片左上角坐标
 * w, h: 图片宽度和高度
 * img_data: 图片数据（每字节8个垂直像素，MSB在上）
 *****************************************************************/
void OLED_DrawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *img_data)
{
    if (img_data == NULL) return;
    
    uint8_t page, col;
    uint8_t start_page = y / 8;
    uint8_t end_page = (y + h - 1) / 8;
    uint8_t y_offset = y % 8;
    
    for (page = start_page; page <= end_page; page++) {
        for (col = 0; col < w; col++) {
            uint8_t data = img_data[(page - start_page) * w + col];
            
            if (page == start_page && y_offset > 0) {
                data >>= y_offset;
            }
            
            uint8_t target_x = x + col;
            if (target_x < OLED_WIDTH && page < 8) {
                OLED_GRAM[target_x][page] = data;
            }
        }
    }
}

/*****************************************************************
 * 显示图片（像素数组格式）
 * x, y: 图片左上角坐标
 * w, h: 图片宽度和高度
 * img_data: 图片数据（每字节一个像素，1=亮，0=灭）
 *****************************************************************/
void OLED_DrawImagePixels(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *img_data)
{
    if (img_data == NULL) return;
    
    uint8_t px, py;
    for (py = 0; py < h; py++) {
        for (px = 0; px < w; px++) {
            uint8_t color = img_data[py * w + px];
            OLED_DrawPixel(x + px, y + py, color);
        }
    }
}


/*****************************************************************
 * 局部刷新屏幕
 * x, y: 区域左上角坐标
 * w, h: 区域宽度和高度
 *****************************************************************/
void OLED_UpdatePartial(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;
    
    uint8_t end_x = x + w - 1;
    uint8_t end_y = y + h - 1;
    
    if (end_x >= OLED_WIDTH) end_x = OLED_WIDTH - 1;
    if (end_y >= OLED_HEIGHT) end_y = OLED_HEIGHT - 1;
    
    uint8_t start_page = y / 8;
    uint8_t end_page = end_y / 8;
    
    for (uint8_t page = start_page; page <= end_page; page++) {
        OLED_WriteCmd(0xB0 + page);
        OLED_WriteCmd(0x00 + (x & 0x0F));
        OLED_WriteCmd(0x10 + ((x >> 4) & 0x0F));
        
        for (uint8_t col = x; col <= end_x; col++) {
            OLED_WriteData(OLED_GRAM[col][page]);
        }
    }
}

/*****************************************************************
 * 局部清除屏幕
 * x, y: 区域左上角坐标
 * w, h: 区域宽度和高度
 *****************************************************************/
void OLED_ClearPartial(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;
    
    uint8_t end_x = x + w - 1;
    uint8_t end_y = y + h - 1;
    
    if (end_x >= OLED_WIDTH) end_x = OLED_WIDTH - 1;
    if (end_y >= OLED_HEIGHT) end_y = OLED_HEIGHT - 1;
    
    for (uint8_t py = y; py <= end_y; py++) {
        for (uint8_t px = x; px <= end_x; px++) {
            OLED_DrawPixel(px, py, 0);
        }
    }
}



