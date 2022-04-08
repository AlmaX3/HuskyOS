#pragma once
#include <stdarg.h>
#include <stdint.h>
#include <stdarg.h>

class hkStdIo {
    public: 
        void vkprint(const char *fmt, va_list args);
        void kprint(const char *fmt, ...);
};


extern hkStdIo HuskyStandardOutput;