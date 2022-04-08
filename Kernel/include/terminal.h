#pragma once
#include <stivale2.h>

class textmode {
    public:
        void initializeTextMode(stivale2_struct_tag_terminal* term);
        void println(const char* string);
        void putchar(const char Char);
    private:
        struct stivale2_struct_tag_terminal* terminal;
};

extern textmode TextMode;