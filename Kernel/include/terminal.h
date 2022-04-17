#pragma once
#include <stivale2.h>
extern int GFX_MODE;


class textmode {
    public:
        void initializeTextMode(stivale2_struct_tag_terminal* term);
        void println(const char* string);
        void putchar(const char Char);
    private:
        struct stivale2_struct_tag_terminal* terminal;
};



class Cursor {
    public:
        uint32_t x;
        uint32_t y;
};

enum gfxColor {
    BACKGROUND = 0x002b36,
    TEXT = 0x93a1a1,
    YELLOW = 0xb58900,
    ORANGE = 0xcb4b16,
    RED = 0xdc322f,
    MAGENTA = 0xd33682,
    VIOLET = 0x6c71c4,
    BLUE = 0x268bd2,
    CYAN = 0x2aa198,
    GREEN = 0x859900  
};


class gfxmode {
    public:
        void initializeFramebuffer(stivale2_struct_tag_framebuffer* framebuffer);
        void framebufferDrawPixel(int x, int y, uint32_t color);
        void putcharonTerm(const char Char, int x, int y);
        void putchar(const char Char);
        void framebuffer_move_one_row_up(void);
        void changefg(uint32_t color);
        void putstring(const char* string);
        void BackspaceDrawOver(int column, int row);
        void DrawLine(uint32_t color, int length, int thickness, int x, int y);
        uint8_t charHeight;
        uint8_t charWidth;
        uint32_t textColor;
        uint32_t bgColor;
    private:
        uint64_t fb_addr;
        uint32_t fb_width;
        uint32_t fb_height;
        uint32_t fb_pitch;
        uint32_t fb_bpp;
};
extern textmode TextMode;
extern gfxmode GfxMode;
extern Cursor cursor;
extern Cursor limit;