#define SSFN_CONSOLEBITMAP_TRUECOLOR	// use the special renderer for 32 bit truecolor packed pixels
#define SSFN_NOIMPLEMENTATION		

#include <terminal.h>
#include "../../include/string.h"
#include <heap.h>
#include <hkStdio.h>
#include <ssfn.h>
int GFX_MODE = 0;


void textmode::initializeTextMode(stivale2_struct_tag_terminal* term) {
    terminal = term;
}

void textmode::println(const char* string) {
    int length = strlen(string+'\0');
    void *term_write_ptr = (void *)terminal->term_write;
    void (*term_write)(const char *string, size_t length) = (void (*)(const char *string, size_t length))term_write_ptr;
    term_write(string+'\0', length);
} 



void textmode::putchar(const char Char) {
    char buff[1];
    buff[0] = Char;

    println(buff);

}

textmode TextMode;


extern uint8_t _binary_Kernel_src_font_unifont_sfn_start;

void gfxmode::initializeFramebuffer(stivale2_struct_tag_framebuffer* framebuffer) {
    fb_addr = framebuffer->framebuffer_addr;
    fb_width = framebuffer->framebuffer_width;
    fb_height = framebuffer->framebuffer_height;
    fb_pitch = framebuffer->framebuffer_pitch;
    fb_bpp = framebuffer->framebuffer_bpp;
    charHeight = 16;
    charWidth = 8;

    for (int y = 0; y < fb_height; y++)
    {
        for (int x = 0; x < fb_width; x++)
        {
            framebufferDrawPixel(x, y, 0x000000);
        }   
    }
    cursor.x = 0;
    cursor.y = 0;

    ssfn_src = (ssfn_font_t*)&_binary_Kernel_src_font_unifont_sfn_start;
    ssfn_dst.ptr = (uint8_t*)fb_addr;
    ssfn_dst.w = fb_width;
    ssfn_dst.h = fb_height;
    ssfn_dst.p = fb_pitch;
    ssfn_dst.x = 0;
    ssfn_dst.y = 0;
    ssfn_dst.fg = 0xffffff;

    GFX_MODE = 1;
}


void gfxmode::framebuffer_move_one_row_up(void)
{
    uint32_t *fb = (uint32_t *)fb_addr;

    for (int column = charHeight; column < fb_height; column++)
    {
        for (int row = 0; row < fb_width; row++)
        {
            size_t current_index = column * (fb_pitch / sizeof(uint32_t)) + row;
            uint32_t current_color = fb[current_index];
            fb[current_index] = ssfn_dst.bg;

            size_t new_index = (column - charHeight) * (fb_pitch / sizeof(uint32_t)) + row;
            fb[new_index] = current_color;
        }
    }
}

void gfxmode::putcharonTerm(const char Char, int x, int y) {


    if (ssfn_dst.x + charWidth > charHeight) // here too problematic
    {
        ssfn_dst.x = 0;
        ssfn_dst.y += charHeight;
    }


    ssfn_dst.x = x;
    ssfn_dst.y = y;


    if (ssfn_dst.y >= fb_height) // this is problematic cuz don't do this when backspace
    {
        ssfn_dst.x = 0;
        ssfn_dst.y = fb_height - charHeight;
        framebuffer_move_one_row_up();
    }

    ssfn_putc(Char);

}

void gfxmode::putchar(const char Char) {
    switch (Char)
    {
    case '\n':
        cursor.x = 0;
        cursor.y += charHeight;
        break;
    
    default:
        putcharonTerm(Char, cursor.x, cursor.y);
        cursor.x += charWidth;
        break;
    }
}

void gfxmode::framebufferDrawPixel(int x, int y, uint32_t color) {
    size_t fb_index = y * (fb_pitch / sizeof(uint32_t)) + x;
    uint32_t *fb = (uint32_t *)fb_addr;

    fb[fb_index] = color;
}

void gfxmode::changefg(uint32_t color) {
    ssfn_dst.fg = color;
}

void gfxmode::putstring(const char* string){
    while (*string)
    {
        putchar(*string);
        string++;
    }
    
}

gfxmode GfxMode;