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


class gfxmode {
    public:
        void initializeFramebuffer(stivale2_struct_tag_framebuffer* framebuffer);
        void framebufferDrawPixel(int x, int y, uint32_t color);
        void putcharonTerm(const char Char, int x, int y);
        void putchar(const char Char);
        void framebuffer_move_one_row_up(void);
        void changefg(uint32_t color);
        void putstring(const char* string);
    private:
        Cursor cursor;
        uint64_t fb_addr;
        uint32_t fb_width;
        uint32_t fb_height;
        uint32_t fb_pitch;
        uint32_t fb_bpp;
        uint8_t charHeight;
        uint8_t charWidth;
};
extern textmode TextMode;
extern gfxmode GfxMode;