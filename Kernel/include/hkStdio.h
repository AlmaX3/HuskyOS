#pragma once
#include <stdarg.h>
#include <stdint.h>
#include <stdarg.h>
#include <isr.h>

class hkStdIo {
    public: 
        void vkprint(const char *fmt, va_list args);
        void kprint(const char *fmt, ...);
        void statuslog(uint32_t color, const char* status, const char*fmt, ...);
        void panic(s_registers* regs,const char* fmt, ...);
};


extern hkStdIo HuskyStandardOutput;